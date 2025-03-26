Markdown on CCSVBusSystem:

    The CCSVBusSystem class inherits the abstract classes defined in CBusSystem. 
    
    It's meant to implement the virtual methods below. 

Methods: 

    StopCount(): Use this to return the number of stops in the bus system. 

    RouteCount(): Use this to return the number of routes in the bus system.

    StopByID(id): Use this to return a stop by its ID.  

    StopByIndex(index): Use this to return the stop at a given index. 

    RouteByIndex(index): Use this to return a route by its index.

    RouteByName(name): Use this to return a route by its name. 

Classes: 

    SStop: Class that contains a StopID and NodeID; Use ID() method to get StopID and NodeID() 
    method to get NodeID

    SRoute: Class that contains a RouteName, and vector of stops. Use the method Name() to get the route's
    name. StopCount() is pretty self-expanatory, and GetStopID(index) gives the stop ID at an index.