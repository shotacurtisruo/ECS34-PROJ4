
#include "CSVBusSystem.h"
#include "DSVReader.h"
#include <memory> //provides std::shared_ptr and std::make_shared for memory management
#include <vector>
#include <string> // thihs allows use for string:: stuff
#include <unordered_map>
#include <iostream> //i need to print bus system details using operator <<

class CCSVBusSystem::SStop : public CBusSystem::SStop
{
public:
    TStopID StopID;
    CStreetMap::TNodeID NodeIDValue;

    CStreetMap::TNodeID NodeID() const noexcept override
    {
        return NodeIDValue;
    }
    TStopID ID() const noexcept override
    {
        return StopID;
    }
};

class CCSVBusSystem::SRoute : public CBusSystem::SRoute
{
public:
    std::string RouteName;

    std::vector<TStopID> rStops;

    std::string Name() const noexcept override
    {
        return RouteName;
    }

    std::size_t StopCount() const noexcept override
    {
        return rStops.size();
    }

    // gets stop ID at the given index
    TStopID GetStopID(std::size_t index) const noexcept override
    {
        if (index >= rStops.size())
        {
            // return an invalid ID if index is out of bounds
            return CBusSystem::InvalidStopID;
        }
        return rStops[index];
    }
};

struct CCSVBusSystem::SImplementation
{
    std::vector<std::shared_ptr<SRoute>> RoutesByIndex;
    std::unordered_map<std::string, std::shared_ptr<SRoute>> Routes;
    std::vector<std::shared_ptr<SStop>> StopsByIndex;          // this stores the stops
    std::unordered_map<TStopID, std::shared_ptr<SStop>> Stops; // stores the stops by ID
};

// constructor for the bus system
CCSVBusSystem::CCSVBusSystem(std::shared_ptr<CDSVReader> stopsrc, std::shared_ptr<CDSVReader> routesrc)
{
    DImplementation = std::make_unique<SImplementation>();
    // check if data is valid first
    if (!stopsrc && !routesrc)
    {
        throw std::invalid_argument("Both stopsrc and routesrc are null");
    }
    std::vector<std::string> row;
    if (routesrc)
    {
        std::unordered_map<std::string, std::shared_ptr<SRoute>> tempRoutes;
        while (routesrc->ReadRow(row))
        {
            if (row.size() >= 2)
            { // i have ot make sure there is enmnough row s first
                try
                {
                    std::string routeName = row[0];      // first one should be routename
                    TStopID stopID = std::stoul(row[1]); // second one should be the stop id
                    auto &route = tempRoutes[routeName];
                    if (!route)
                    {
                        route = std::make_shared<SRoute>();
                        route->RouteName = routeName;
                    }
                    route->rStops.push_back(stopID);
                }
                catch (const std::exception &e)
                {
                    // handle error
                    std::cerr << "exception caught -  " << e.what() << "\n";
                }
            }
        }

        for (const auto &pair : tempRoutes)
        {
            DImplementation->Routes[pair.first] = pair.second;
            DImplementation->RoutesByIndex.push_back(pair.second);
        }
    }
    if (stopsrc)
    {
        while (stopsrc->ReadRow(row))//go through every oline
        {
            // ensure sufficient columns exists
            if (row.size() >= 2)//make sure enough cokumn are there
            {
                try
                {
                    auto stop = std::make_shared<SStop>();
                    stop->StopID = std::stoul(row[0]);
                    stop->NodeIDValue = std::stoul(row[1]);
                    DImplementation->Stops[stop->StopID] = stop;
                    DImplementation->StopsByIndex.push_back(stop);
                }
                catch (const std::exception &e)
                {
                    std::cerr << "exception caught -  " << e.what() << "\n";
                }
            }
        }
    }
}

CCSVBusSystem::~CCSVBusSystem() = default;

std::size_t CCSVBusSystem::StopCount() const noexcept
{
    return DImplementation->StopsByIndex.size();
}

// return the total number of routes
std::size_t CCSVBusSystem::RouteCount() const noexcept
{
    return DImplementation->RoutesByIndex.size();
}

// return a stop by its ID
std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByID(TStopID id) const noexcept
{
    auto temp = DImplementation->Stops.find(id);
    if (temp != DImplementation->Stops.end())
    {
        return temp->second;
    }
    return nullptr;
}

// return a route by index
std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByIndex(std::size_t index) const noexcept
{
    if (index < DImplementation->RoutesByIndex.size())
    {
        return DImplementation->RoutesByIndex[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SStop> CCSVBusSystem::StopByIndex(std::size_t index) const noexcept
{
    if (index < DImplementation->StopsByIndex.size())
    {
        return DImplementation->StopsByIndex[index];
    }
    return nullptr;
}

std::shared_ptr<CBusSystem::SRoute> CCSVBusSystem::RouteByName(const std::string &name) const noexcept
{
    auto it = DImplementation->Routes.find(name);
    if (it != DImplementation->Routes.end())
    {
        return it->second;
    }
    return nullptr;
}

std::ostream &operator<<(std::ostream &os, const CCSVBusSystem &bussystem)
{
    os << "StopCount: " << bussystem.StopCount() << "\n"
       << "RouteCount: " << bussystem.RouteCount() << "\n";

    for (size_t i = 0; i < bussystem.StopCount(); ++i)
    {
        if (auto stop = bussystem.StopByIndex(i))
        {
            os << "Index " << i << " ID: " << stop->ID()
               << " NodeID: " << stop->NodeID() << "\n";
        }
    }

    for (size_t i = 0; i < bussystem.RouteCount(); ++i)
    {
        if (auto route = bussystem.RouteByIndex(i))
        {
            os << "Route Index " << i << " Name: " << route->Name()
               << " StopCount: " << route->StopCount() << "\n";
        }
    }

    return os;
}

