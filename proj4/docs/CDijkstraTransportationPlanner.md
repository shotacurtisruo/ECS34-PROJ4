MarkDown on CDijkstraTransporationPlanner:
    The DijkstraTransportationPlanner.cpp file is part of the transporation planning system that incorporates our favorite : dijkstras algorithm to find which ways/ path is fastest between the nodes. We took into account three modes of transportation which waas wlaking, biking and busing and also took into account how like we cant bike after we bus since the buses cannot take on bikes,  and also we cant bike then bus etc with the assumptions given on the pdf file.

Methods: 
    For the Struct that encapsulates implementation of transportationplanner, it holds all the config data , th sorteed nodes, the distance / time for the path routers, and the mappings 
    between the node ids and vertexes. this basically sets up the routing grpahs while processesing out street map an
    d bus system.

    InitializeNodes - we used this private function to organize the nodes from streetmap and sorted them by their ids. we then mapped the node ids to their indices in the srted lists/vectors. basically to set up faster and efficenet access to nodes

    createroutervertices - initialized routing grpahs both dist and itme and adds vertices for every node in the vector. made sure we establisehd bidirectional mappings between node ids and vertexids.

    proccessbussyetm - pretty self explanatory  - created mapping for bus stop ids to nodeids. also tracks first bust stop for every node and lowest id

    processalways - process all ways ofin street map 

    parsespeedlimit - this was a helper function which parsed the speed limit strings and then convert to numerical values. makde sure it handled cases where speeds was maybe shown in diff formats

    addbusedges - this function goes through all the bus routes adn added egdes to time router for bus travel and calculated the time of travel between tehe bus stops. made sure for the travel time we added the stop ime as well and recorded route info

    formatlocation - this formated the coordinates we got from the given geographic utils file into readable strings with degrees , min and secs

    getdirectionstring - converted the bearing angle into directions so like NE, N , E etc

    the rest of functions after constructor are explained on the directions and through my comments. nothign too complex.

    Findfastestpath - this one foudn the fastest path between two nodes using time router  made sure it incorporated the different travel methods like walk, bus bike, and combined consec steps with the same travel method.

    didnt get the getpathdescripttion to work :(