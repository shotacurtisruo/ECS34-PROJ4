Markdown on CDijkstraPathRouter: 

    This class inherits the virtual methods from CPathRouter. It implements them as shown below. 

Methods: 

    AddVertex(tag): Use this to add a vertex to the graph with the given tag
    GetVertexTag(id): Use this to get the tag of a vertex. 
    AddEdge(src, dest, weight, bidir): Use this to add an edge to the graph with its direction from src to dest, 
        unless bidir is set to true. Also adds the weight to the edge. 
    FindShortestPath(src, dest, path): Use this to return the shortest path from a src to dest. It used dijkstra 
        to try and be more efficient. 

Classes: 

    Includes the class ThisVertex that is used by the methods we implemented. 