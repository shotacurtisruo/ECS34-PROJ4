//implement this class
#include "DijkstraPathRouter.h"
#include <iostream> 
#include <algorithm>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory> //used for dynamic memory mangaement for shared and unique pointers
#include <limits>
#include <string> //enables use of hnadling string data

//the cdijkstra path router class will implement the cpathrouter abstract interface - 
//thecdijkstra path router class will find the shortest path between source and destination vertices if one exists. 
//the vertex IDs do not have to match node or stop IDS used by the other classes. 

struct CDijkstraPathRouter::SImplementation{
    
    struct ThisVertex{
        TVertexID ID; //this is the unique id of the vertex
        std::any Tag; //this is the tag that could be string int or any type
        std::unordered_map<TVertexID, double> edges; //thsi stores weight of
        //edges from this node to other nodes
        std::vector<TVertexID> path; //this is a list of adjacent vertices
        
        ~ThisVertex() = default;

        TVertexID GetVertexID() const noexcept{
            return ID;
        }

        std::any GetTag() const noexcept{
            return Tag;
        }

        //this will return the list of connected neighbors
        std::vector<TVertexID> GetNeighbors() const noexcept{
            return path;
        }

        //this will get weight of an edge -> to  anieghbor
        double GetWeight(TVertexID to) const noexcept{
            auto it = edges.find(to);
            if(it != edges.end()){
                return it->second;
            }
            return std::numeric_limits<double>::infinity();
        }

    };

    std::vector<std::shared_ptr<ThisVertex>> vertices; //this is a vector of shared pointers to the vertices
    TVertexID nextID; //this is the next id to be assigned to a vertex

    std::size_t VertexCount() const noexcept{//this funcitno returns number of vertices in graph 
        return vertices.size();
    }
    
    //now must create a a new vertex to a grpah 
    TVertexID AddVertex(std::any tag) noexcept{
        auto newVertex = std::make_shared<ThisVertex>();
        newVertex->ID = nextID;
        newVertex->Tag = tag;
        vertices.push_back(newVertex);
        nextID++;
        return newVertex->ID;
    }

    //if the tag exists it returns the tag of a vertex
    std::any GetVertexTag(TVertexID id) const noexcept {
        if (id < vertices.size()){
            return vertices[id]->GetTag();
        }
        return std::any();
    }

    //this adds weighted edge between the src and dest given
    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir = false) const noexcept{
        if(src >= vertices.size() || dest >= vertices.size() || weight < 0){
            return false;
        }

        vertices[src]->edges[dest] = weight; // add weight from src to dest 
        vertices[src]->path.push_back(dest); // add directed edge to path vector

        if(bidir){ // if the path can be traversed both ways, add weight from dest to src
            vertices[dest]->edges[src] = weight;
            vertices[dest]->path.push_back(src); // add directed edge to path vector
        }

        return true;
    }

    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept{//no precomp is needed for this way 
        return true;
    }

    //now implement dijkstra to find shortest path // the std::vector will store the shortest path 
    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
        // returns path distance of path from src to dest, and fills out path with vertices.
        //if no path, NoPathExists is returned.

        //clear any previous path 
        path.clear();

        //if src or dest are invalid return no path
        if(src >= vertices.size() || dest >= vertices.size()){
            return NoPathExists;
        }

        //now define a priority queue to store the vertices
        //where elements is a pair of distance and vertex id

        //we will use priority queues because this should be more faster than a simple linear search of paths and our TC would
        //be O(V^2) 
        std::priority_queue<std::pair<TVertexID, double>, std::vector<std::pair<TVertexID, double>>, std::greater<std::pair<TVertexID, double>>> priorityq;

        //now we will define a distance vector to store the distance from src to each vertex 
        //initializes distances to NoPathExists (infinite)
        std::vector<double> dist(VertexCount(), NoPathExists);
        //now we will define a previous vector to store the previous vertex in the path
        std::vector<TVertexID> previous(VertexCount(), InvalidVertexID);

        //now initialize src vertex distance to 0 and push it into priority queue 
        priorityq.push(std::make_pair(0, src));
        dist[src] = 0;

        //now we will implement the dijkstra algorithm
        while(!priorityq.empty()){
            //get the vertex with the smallest distance
            TVertexID v = priorityq.top().second;
            double distance = priorityq.top().first;

            priorityq.pop();

            //if we reach dest vertex you stop early 
            if(v == dest){
                break;
            }

            //now if current distance is bigger than the known one skip it
            if(distance > dist[v]){
                continue;
            }

            //now we will iterate over the neighbors of u
            for(TVertexID neighbor : vertices[v]->GetNeighbors()){
                double weight = vertices[v]->GetWeight(neighbor);

                // now if new calcullation is better than the known one update it
                if(dist[neighbor] > dist[v] + weight) {
                    dist[neighbor] = dist[v] + weight;
                    previous[neighbor] = v;
                    priorityq.push(std::make_pair(dist[neighbor], neighbor));
                }
            }
        }

        //nmwo you have to reconstruct the path
        TVertexID at = dest;
        while (at != src) {
            path.push_back(at);
            if (previous[at] == InvalidVertexID) {
                return NoPathExists;
            }
            at = previous[at];
        }

        path.push_back(src); // finish off reconstruction by adding src cuz we stopped right before in loop

        //now u reverese the path to make sure u have correct order from source to destination
        std::reverse(path.begin(), path.end());

        //this is the goaal now return total shortest path from dist to src to dest
        //if dest is still notpathexists no path is returned
        if (dist[dest] == NoPathExists){
            return NoPathExists;
        } else { 
            return dist[dest];
        }
    }

};
//now we will construct the dijkstra path router class
CDijkstraPathRouter::CDijkstraPathRouter(){
    DImplementation = std::make_unique<SImplementation>();
}

CDijkstraPathRouter::~CDijkstraPathRouter(){

}
std::size_t CDijkstraPathRouter::VertexCount() const noexcept{
    return DImplementation->VertexCount();
}

CPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept{
    return DImplementation->AddVertex(tag);
}

std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept{
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept{
    return DImplementation->AddEdge(src,dest,weight,bidir);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept{
    return DImplementation->Precompute(deadline);
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept{
    return DImplementation->FindShortestPath(src,dest,path);
}