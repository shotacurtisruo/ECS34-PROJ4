
#include "DijkstraTransportationPlanner.h"
#include "DijkstraPathRouter.h"
#include "GeographicUtils.h"
#include <queue>
#include <unordered_map>
#include <set>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

struct CDijkstraTransportationPlanner::SImplementation
{
    
    std::shared_ptr<SConfiguration> Config;
    //this is the config data that contain the streetmap, bus systems, and speed
    std::vector<std::shared_ptr<CStreetMap::SNode>> SortedNodes;
    //nodes arranged by id
    std::shared_ptr<CDijkstraPathRouter> DistanceRouter;
    //this is path routers for dist and time
    std::shared_ptr<CDijkstraPathRouter> TimeRouter;
     //this is path routers for dist and time

    std::unordered_map<CStreetMap::TNodeID, CPathRouter::TVertexID> NodeIDToDistanceVertexID;
    std::unordered_map<CStreetMap::TNodeID, CPathRouter::TVertexID> NodeIDToTimeVertexID;
    std::unordered_map<CPathRouter::TVertexID, CStreetMap::TNodeID> DistanceVertexIDToNodeID;
    std::unordered_map<CPathRouter::TVertexID, CStreetMap::TNodeID> TimeVertexIDToNodeID;
    //these are mappings between street map node id and router vertex id


    std::unordered_map<CStreetMap::TNodeID, size_t> NodeIDToIndex;
    //this looks up node position in sorted node list or vector


//the ones belwo are for the bus system
    std::unordered_map<CBusSystem::TStopID, CStreetMap::TNodeID> StopIDToNodeID;//this maps stop id to node id
    std::unordered_map<CBusSystem::TStopID, std::string> StopIDToStopName;//this maps stop id to stop name
    std::unordered_map<CStreetMap::TNodeID, CBusSystem::TStopID> NodeIDToStopID;//rest self explanatory
    std::unordered_map<CStreetMap::TNodeID, std::set<std::pair<std::string, CStreetMap::TNodeID>>> BusRouteInfo;


    //constructor initliazes the ds and goes through the map data
    SImplementation(std::shared_ptr<SConfiguration> config) : Config(config)
    {
        InitializeNodes(); // sorts/indexes nodes
        CreateRouterVertices();//sets up routes
        ProcessBusSystem();//bus stop datas
        ProcessAllWays();//loads road infos
        AddBusEdges(); //this adds bus routes onto the graph
    }

private:
//first  we need to try to organize noes from the street map
    void InitializeNodes()
    {
        auto sm = Config->StreetMap();
        SortedNodes.reserve(sm->NodeCount());
        //retrieves all the nodes from streeetmap
        for (size_t i = 0; i < sm->NodeCount(); ++i)
        {
            SortedNodes.push_back(sm->NodeByIndex(i));
        }

        //this sorts nodse by id for faster access
        std::sort(SortedNodes.begin(), SortedNodes.end(),
                  [](auto &a, auto &b)
                  { return a->ID() < b->ID(); });
        //this creates ID for index mapping for 0(1) TC lookups
        for (size_t i = 0; i < SortedNodes.size(); ++i)
        {
            NodeIDToIndex[SortedNodes[i]->ID()] = i;
        }
    }


    //this creates vertices for both routers for every nodes
    void CreateRouterVertices()
    {
        //first initilaize empy routing graphs
        DistanceRouter = std::make_shared<CDijkstraPathRouter>();
        TimeRouter = std::make_shared<CDijkstraPathRouter>();

        //add vertices to both of the routers
        for (auto &node : SortedNodes)
        {
            auto nodeID = node->ID();
            auto distVID = DistanceRouter->AddVertex(nodeID);
            auto timeVID = TimeRouter->AddVertex(nodeID);


            //make sure to hold bidirectional mapping
            NodeIDToDistanceVertexID[nodeID] = distVID;
            NodeIDToTimeVertexID[nodeID] = timeVID;
            DistanceVertexIDToNodeID[distVID] = nodeID;
            TimeVertexIDToNodeID[timeVID] = nodeID;
        }
    }
    // this function should process bus system data and creates stop-node mappings
    void ProcessBusSystem()
    {
        auto bs = Config->BusSystem();
        //loop through all the bus stops
        for (size_t i = 0; i < bs->StopCount(); ++i)
        {
            auto stop = bs->StopByIndex(i);
            //for each map stopid to physical node
            StopIDToNodeID[stop->ID()] = stop->NodeID();

            // now track 1st bus stop fo reach node and lowest ID >
            auto &existing = NodeIDToStopID[stop->NodeID()];
            if (!existing || stop->ID() < existing)
            {
                existing = stop->ID();
                //change it to existing if it is not there or if the new one is smaller
            }
        }
    }


    //this function processes all the ways 
    void ProcessAllWays()
    {
        //first, loop through all the ways in the street map
        auto sm = Config->StreetMap();

        for (size_t i = 0; i < sm->WayCount(); ++i)
        {
            ProcessWay(sm->WayByIndex(i));
        }
    }


    //this function will go through each and every individual ways
    void ProcessWay(const std::shared_ptr<CStreetMap::SWay> &way)
    {
        const bool isOneway = way->HasAttribute("oneway") &&
                              (way->GetAttribute("oneway") == "yes" ||
                               way->GetAttribute("oneway") == "1");


        // this processes consec node pairs along the way                       
        for (size_t j = 1; j < way->NodeCount(); ++j)
        {
            auto srcID = way->GetNodeID(j - 1);
            auto destID = way->GetNodeID(j);


            if (srcID == CStreetMap::InvalidNodeID || destID == CStreetMap::InvalidNodeID)
                continue;//this skips invalid nodes so continue to next node
            //this adds forward edge
            AddEdgesBetweenNodes(srcID, destID, way, isOneway);
            //if it sint oneway then add the reverse edge
            if (!isOneway)
            {
                AddEdgesBetweenNodes(destID, srcID, way, false);
            }
        }
    }
    //this creates routing edges between two nodes
    void AddEdgesBetweenNodes(CStreetMap::TNodeID src, CStreetMap::TNodeID dest,
                              const std::shared_ptr<CStreetMap::SWay> &way, bool oneway)
    {
        //now grab the actual node objs
        auto srcNode = Config->StreetMap()->NodeByID(src);
        auto destNode = Config->StreetMap()->NodeByID(dest);
        if (!srcNode || !destNode)
            return;
        //make sure to double cuz of decimnal
        //this calculate the distance between the nodes
        const double distance = SGeographicUtils::HaversineDistanceInMiles(
            srcNode->Location(), destNode->Location());
        if (distance <= 0.0) //if there is no distance it should be invalid so skip it
            return;

        // Distance router
        DistanceRouter->AddEdge(NodeIDToDistanceVertexID[src],NodeIDToDistanceVertexID[dest],distance);

        // Time router calculations
        const auto AddTimeEdge = [&](double speed)
        {
            TimeRouter->AddEdge(NodeIDToTimeVertexID[src], NodeIDToTimeVertexID[dest], distance / speed);
        };
        //these adds the edges for the diff kinds of transportation modes // fo rwalking and biking
        AddTimeEdge(Config->WalkSpeed());
        AddTimeEdge(Config->BikeSpeed());
        AddTimeEdge(way->HasAttribute("maxspeed") ? ParseSpeedLimit(way->GetAttribute("maxspeed")) : Config->DefaultSpeedLimit());
    }

    double ParseSpeedLimit(const std::string &speed) const
    {
        //use try / catch to parse the speed limit
        try
        {
            size_t spacePos = speed.find(' ');
            return std::stod(spacePos != std::string::npos ? speed.substr(0, spacePos) : speed);//this handles where it shows like "30 mph"
        }
        catch (...) // this is the fallback for if parsing fails, and it just returns the default speed lmitm 
        {
            return Config->DefaultSpeedLimit();
        }
    }
    // this function will add the bus edges to the graph
    void AddBusEdges()
    {
        auto bs = Config->BusSystem();
        //processes all the bus routes
        for (size_t r = 0; r < bs->RouteCount(); ++r)
        {
            auto route = bs->RouteByIndex(r);
            const auto routeName = route->Name();
            
            //thsi connects the consecutive stops in the route 
            for (size_t i = 0; i < route->StopCount() - 1; ++i)
            {
                auto currentStopID = route->GetStopID(i);
                auto nextStopID = route->GetStopID(i + 1);
                

                //gets the phsyical nodes for the stops 
                auto currentStop = bs->StopByID(currentStopID);
                auto nextStop = bs->StopByID(nextStopID);
                if (!currentStop || !nextStop)
                    continue;

                auto srcID = currentStop->NodeID();
                auto destID = nextStop->NodeID();

                //records route information here 
                BusRouteInfo[srcID].insert({routeName, destID});

                auto srcNode = Config->StreetMap()->NodeByID(srcID);
                auto destNode = Config->StreetMap()->NodeByID(destID);
                if (!srcNode || !destNode)
                    continue;
                //thsi calculates the bus travel times which also adds the bus stop time too
                const double distance = SGeographicUtils::HaversineDistanceInMiles(srcNode->Location(), destNode->Location());
                const double busTime = (distance / Config->DefaultSpeedLimit()) +(Config->BusStopTime() / 3600.0);// here we have to add the bus stop time

                TimeRouter->AddEdge(NodeIDToTimeVertexID[srcID],NodeIDToTimeVertexID[destID],busTime); // add s teh time edge for route of bus
            }
        }
    }

public:
// this next functino we need to make shoould find the bus routes between the two ndoes
    std::string FindBusRouteBetweenNodes(const CStreetMap::TNodeID &src,const CStreetMap::TNodeID &dest) const
    {
        if (BusRouteInfo.count(src) == 0)
            return "";
        std::vector<std::string> directRoutes;

        //this loops and checks through all routes laeving the inital node
        for (const auto &[routeName, nextNodeID] : BusRouteInfo.at(src))
        {
            if (nextNodeID == dest)
                directRoutes.push_back(routeName);
        }



        // this should return alphabetically first route name if there are more than one
        if (!directRoutes.empty())
        {
            std::sort(directRoutes.begin(), directRoutes.end());
            return directRoutes[0];
        }
        return "";
    }
// we have to make suee we can format the location or coords we get into readable strings
    std::string FormatLocation(const std::shared_ptr<CStreetMap::SNode> &node) const
    {
        double lat = node->Location().first; // convert latitude 
        double lon = node->Location().second;// convert longitude
        int lat_deg = static_cast<int>(std::floor(std::abs(lat))); //convert it to degrees
        double lat_min_full = (std::abs(lat) - lat_deg) * 60.0;
        int lat_min = static_cast<int>(std::floor(lat_min_full)); 
        int lat_sec = static_cast<int>(std::round((lat_min_full - lat_min) * 60.0));
        if (lat_sec == 60)
        //now convert the seconds to minutes 
        {
            lat_min++;
            lat_sec = 0;
        }
        if (lat_min == 60)
        //here min to degrees
        {
            lat_deg++;
            lat_min = 0;
        }
        int lon_deg = static_cast<int>(std::floor(std::abs(lon)));
        //now we do the same for longitude
        double lon_min_full = (std::abs(lon) - lon_deg) * 60.0;
        int lon_min = static_cast<int>(std::floor(lon_min_full));
        int lon_sec = static_cast<int>(std::round((lon_min_full - lon_min) * 60.0));
        if (lon_sec == 60)
        {
            lon_min++;
            lon_sec = 0;
        }
        if (lon_min == 60)
        {
            lon_deg++;
            lon_min = 0;
        }

        //now format to standard coordinate strings
        std::stringstream ss;
        if (lat < 0)
            lat_deg = -lat_deg;
        ss << lat_deg << "d " << lat_min << "' " << lat_sec << "\" "
           << (lat >= 0 ? "N" : "S") << ", "
           << lon_deg << "d " << lon_min << "' " << lon_sec << "\" "
           << (lon >= 0 ? "E" : "W");
        return ss.str();
    }
    
    // thsi calculates the compass bearing between the two nodes or basically
    // direction in degrees
    double CalculateBearing(const std::shared_ptr<CStreetMap::SNode> &src,
                            const std::shared_ptr<CStreetMap::SNode> &dest) const
    {
        return SGeographicUtils::CalculateBearing(src->Location(), dest->Location());
    }
// converts bearing to cardinal directions
    std::string GetDirectionString(double angle) const
    {
        return SGeographicUtils::BearingToDirection(angle);
    }
    // this function will return the street name between two nodes
    std::string GetStreetName(const std::shared_ptr<CStreetMap::SNode> &node1,
                              const std::shared_ptr<CStreetMap::SNode> &node2) const
    {
        auto StreetMap = Config->StreetMap();
        for (size_t i = 0; i < StreetMap->WayCount(); ++i)
        {
            auto way = StreetMap->WayByIndex(i); // now check for consecutive nodes 
            for (size_t j = 0; j < way->NodeCount() - 1; ++j)// check both directions its a oneway
            {
                if ((way->GetNodeID(j) == node1->ID() && way->GetNodeID(j + 1) == node2->ID())||(way->GetNodeID(j) == node2->ID() && way->GetNodeID(j + 1) == node1->ID()))
                {
                    if (way->HasAttribute("name"))
                        return way->GetAttribute("name");
                    //if there is no name attribute then we return the default or unnamed in this case
                    return "unnamed street";
                }
            }
        }
        return "unnamed street";
    }
};
// tking a break left it off at here

CDijkstraTransportationPlanner::CDijkstraTransportationPlanner(std::shared_ptr<SConfiguration> config)
    : DImplementation(std::make_unique<SImplementation>(config)) // construvtor
{
}

CDijkstraTransportationPlanner::~CDijkstraTransportationPlanner() = default; // this is the destructor

std::size_t CDijkstraTransportationPlanner::NodeCount() const noexcept
{
    return DImplementation->SortedNodes.size();
}

std::shared_ptr<CStreetMap::SNode> CDijkstraTransportationPlanner::SortedNodeByIndex(std::size_t index) const noexcept
{
    if (index < DImplementation->SortedNodes.size())
    {
        return DImplementation->SortedNodes[index];
    }
    return nullptr;
}

double CDijkstraTransportationPlanner::FindShortestPath(TNodeID src, TNodeID dest, std::vector<TNodeID> &path)
{
    // always clear the output path
    path.clear();

    // Make sure source and destination are valid again if not throw nopathexists
    if (DImplementation->NodeIDToDistanceVertexID.find(src) == DImplementation->NodeIDToDistanceVertexID.end() ||
        DImplementation->NodeIDToDistanceVertexID.find(dest) == DImplementation->NodeIDToDistanceVertexID.end())
    {
        return CPathRouter::NoPathExists;
    }

    // vertex id for the source and destination
    auto destVertex = DImplementation->NodeIDToDistanceVertexID[dest];
    auto sourceVertex = DImplementation->NodeIDToDistanceVertexID[src];

    // Find shortest path using the distance router
    std::vector<CPathRouter::TVertexID> routerPath;
    double dist = DImplementation->DistanceRouter->FindShortestPath(sourceVertex, destVertex, routerPath);

    // no path or dist is infiniite
    if (dist < 0.0)
    {
        return CPathRouter::NoPathExists;
    }

    // Convert router path (vertex IDs) back to node IDs
    for (const auto &vertexID : routerPath)
    {
        path.push_back(DImplementation->DistanceVertexIDToNodeID[vertexID]);
    }

    return dist;
}
// this function will find the fastest path between two nodes and return the time it takes to travel that path
// i think got this down to earth

// important assumptions : For the fastest path, assume you can walk both directions regardless of "oneway",
// bike/bus must follow "oneway". Also, you cannot bike along paths that specify
//"bicycle" as "no"

// assume bus route takes shortest path
// you cannot take your bike on bus so if you take bus
// you must walk to it.
// and you cant ride a bike explicitly after you get off the bus
double CDijkstraTransportationPlanner::FindFastestPath(TNodeID src, TNodeID dest, std::vector<TTripStep> &path)
{
    path.clear();
    if (DImplementation->NodeIDToTimeVertexID.find(src) == DImplementation->NodeIDToTimeVertexID.end() ||
        DImplementation->NodeIDToTimeVertexID.find(dest) == DImplementation->NodeIDToTimeVertexID.end())
    {
        return CPathRouter::NoPathExists; //if src or dest not exist, then return nopathexists
    }

    auto srcVertex = DImplementation->NodeIDToTimeVertexID[src];
    auto destVertex = DImplementation->NodeIDToTimeVertexID[dest];
    std::vector<CPathRouter::TVertexID> routerPath;
    double time = DImplementation->TimeRouter->FindShortestPath(srcVertex, destVertex, routerPath);

    if (time < 0.0)
        return CPathRouter::NoPathExists;

    auto StreetMap = DImplementation->Config->StreetMap();
    ETransportationMode prevMode = ETransportationMode::Walk;

    for (size_t i = 0; i < routerPath.size(); ++i)
    {
        auto currentNodeID = DImplementation->TimeVertexIDToNodeID[routerPath[i]];
        ETransportationMode tMode = ETransportationMode::Walk;

        if (i > 0)
        {
            auto prevNodeID = DImplementation->TimeVertexIDToNodeID[routerPath[i - 1]];
            auto prevNode = StreetMap->NodeByID(prevNodeID);
            auto currentNode = StreetMap->NodeByID(currentNodeID);
            std::string busRoute = DImplementation->FindBusRouteBetweenNodes(prevNodeID, currentNodeID);

            // Critical: Restrict mode transitions
            if (prevMode == ETransportationMode::Bus)
            {
                // After bus: Can only continue bus or walk
                tMode = (!busRoute.empty()) ? ETransportationMode::Bus : ETransportationMode::Walk;
            }
            else if (prevMode == ETransportationMode::Bike)
            {
                // After bike: Can only continue biking or walk
                double dist = SGeographicUtils::HaversineDistanceInMiles(
                    prevNode->Location(), currentNode->Location());
                double bikeTime = dist / DImplementation->Config->BikeSpeed();
                double walkTime = dist / DImplementation->Config->WalkSpeed();

                tMode = (bikeTime < walkTime) ? ETransportationMode::Bike : ETransportationMode::Walk;
            }
            else
            {
                // After walking: Free to choose any mode
                if (!busRoute.empty())
                {
                    tMode = ETransportationMode::Bus;
                }
                else
                {
                    double dist = SGeographicUtils::HaversineDistanceInMiles(
                        prevNode->Location(), currentNode->Location());
                    double bikeTime = dist / DImplementation->Config->BikeSpeed();
                    double walkTime = dist / DImplementation->Config->WalkSpeed();

                    if (bikeTime < walkTime) {
                        tMode = ETransportationMode::Bike;
                        //if the biking route is faster change tmode to bike
                    } else {
                        //other than that if walk is faster it should be set to walk
                        tMode = ETransportationMode::Walk;
                    }
                }
            }

            prevMode = tMode; // Update for next iteration
        }

        path.push_back({tMode, currentNodeID});
    }

    // Merge consecutive same-mode steps
    if (path.size() > 1)
    {
        for (size_t i = 1; i < path.size(); ++i)
        {
            if (path[i].first == path[i - 1].first)
            {
                path[i - 1].second = path[i].second;
                path.erase(path.begin() + i);
                i--; // Re-check current index
            }
        }
    }

    return time;
}
// this functino will return a description of the path so we can read set of steps and rit returns true if the path description
// is created in the process //fixed this i think

// not working kind of give up on this lost cause

bool CDijkstraTransportationPlanner::GetPathDescription(const std::vector<TTripStep> &path, std::vector<std::string> &desc) const
{
   

    return true;
}