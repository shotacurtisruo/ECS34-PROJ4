#include "BusSystemIndexer.h"
#include "BusSystem.h"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <string>
#include <memory>

struct CBusSystemIndexer::SImplementation
{
    std::shared_ptr<CBusSystem> bussystem;
    SImplementation(std::shared_ptr<CBusSystem> bussystem) : bussystem(bussystem)
    {
    }

    std::size_t StopCount() const noexcept
    {
        return bussystem->StopCount();
    }

    std::size_t RouteCount() const noexcept
    {
        return bussystem->RouteCount();
    }

    std::shared_ptr<SStop> SortedStopByIndex(std::size_t index) const noexcept
    {
        if (index >= bussystem->StopCount())
        { // return nullptr if index >= StopCount()
            return nullptr;
        }

        std::vector<int> StopID;
        for (std::size_t i = 0; i < bussystem->StopCount(); i++)
        {
            auto busstop = bussystem->StopByIndex(i)->ID(); // get the stop by id
            StopID.push_back(busstop);                      // add to the vector
        }

        std::sort(StopID.begin(), StopID.end());   // sort the stops by id
        return bussystem->StopByID(StopID[index]); // gives the stop specified by the indexed ID
    }

    std::shared_ptr<SRoute> SortedRouteByIndex(std::size_t index) const noexcept
    {
        if (index >= bussystem->RouteCount())
        { // return nullptr if RouteCount() < index
            return nullptr;
        }

        std::vector<std::string> RouteID;
        for (std::size_t i = 0; i < bussystem->RouteCount(); i++)
        {
            auto busroute = bussystem->RouteByIndex(i)->Name(); // get the route by name
            RouteID.push_back(busroute);                        // add to the vector
        }

        std::sort(RouteID.begin(), RouteID.end());     // sort the routes by name
        return bussystem->RouteByName(RouteID[index]); // gives the route specified by the indexed name
    }

    std::shared_ptr<SStop> StopByNodeID(TNodeID id) const noexcept
    {
        for (std::size_t i = 0; i < bussystem->StopCount(); i++)
        {
            if (bussystem->StopByIndex(i)->NodeID() == id)
            { // if id matches nodeid return the stop associated
                return bussystem->StopByIndex(i);
            }
        }

        return nullptr; // else return nullptr
    }

    bool RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routes) const noexcept
    {

        if (!StopByNodeID(src) || !StopByNodeID(dest))
        {
            return false;
        }

        CBusSystem::TStopID srcstopID = StopByNodeID(src)->ID(); // get the stop ids
        CBusSystem::TStopID deststopID = StopByNodeID(dest)->ID();

        for (std::size_t i = 0; i < bussystem->RouteCount(); i++)
        { // iterate through the routes
            auto route = bussystem->RouteByIndex(i);

            bool foundsrc = false;
            bool founddest = false;

            for (std::size_t j = 0; j < route->StopCount(); j++)
            {                                                   // iterate through the route's stops
                CBusSystem::TStopID stop = route->GetStopID(j); // get a stop
                if (stop == srcstopID)
                { // if matches source, we found the source
                    foundsrc = true;
                }
                if (stop == deststopID)
                { // if matches destination, we found the destination
                    founddest = true;
                }
                if (founddest == true && foundsrc == true)
                { // if the route is found, add it to routes
                    routes.insert(route);
                    break; // break so that we don't keep iterating
                }
            }
        }
        if (!routes.empty())
        { // if routes isn't empty, return true
            return true;
        }
        else
        {
            return false; // else return false
        }
    }

    bool RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept
    {
        if (!StopByNodeID(src) || !StopByNodeID(dest))
        {
            return false;
        }

        CBusSystem::TStopID srcstopID = StopByNodeID(src)->ID(); // get the stop ids
        CBusSystem::TStopID deststopID = StopByNodeID(dest)->ID();

        for (std::size_t i = 0; i < bussystem->RouteCount(); i++)
        { // iterate through the routes
            auto route = bussystem->RouteByIndex(i);

            bool foundsrc = false;
            bool founddest = false;

            for (std::size_t j = 0; j < route->StopCount(); j++)
            {                                                   // iterate through the route's stops
                CBusSystem::TStopID stop = route->GetStopID(j); // get a stop
                if (stop == srcstopID)
                { // if matches source, we found the source
                    foundsrc = true;
                }
                if (stop == deststopID)
                { // if matches destination, we found the destination
                    founddest = true;
                }
                if (founddest == true && foundsrc == true)
                { // if a route is found, return true
                    return true;
                }
            }
        }
        return false;
    }
};

// constructor
CBusSystemIndexer::CBusSystemIndexer(std::shared_ptr<CBusSystem> bussystem) : DImplementation(std::make_unique<SImplementation>(bussystem))
{
}
// destructor
CBusSystemIndexer::~CBusSystemIndexer() = default;

std::size_t CBusSystemIndexer::StopCount() const noexcept
{
    return DImplementation->StopCount();
}

std::size_t CBusSystemIndexer::RouteCount() const noexcept
{
    return DImplementation->RouteCount();
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::SortedStopByIndex(std::size_t index) const noexcept
{
    return DImplementation->SortedStopByIndex(index);
}

std::shared_ptr<CBusSystem::SRoute> CBusSystemIndexer::SortedRouteByIndex(std::size_t index) const noexcept
{
    if (index >= DImplementation->RouteCount())
    {
        return nullptr;
    }
    else
    {
        return DImplementation->SortedRouteByIndex(index);
    }
}

std::shared_ptr<CBusSystem::SStop> CBusSystemIndexer::StopByNodeID(TNodeID id) const noexcept
{
    return DImplementation->StopByNodeID(id);
}

bool CBusSystemIndexer::RoutesByNodeIDs(TNodeID src, TNodeID dest, std::unordered_set<std::shared_ptr<SRoute>> &routes) const noexcept
{
    return DImplementation->RoutesByNodeIDs(src, dest, routes);
}

bool CBusSystemIndexer::RouteBetweenNodeIDs(TNodeID src, TNodeID dest) const noexcept
{
    return DImplementation->RouteBetweenNodeIDs(src, dest);
}