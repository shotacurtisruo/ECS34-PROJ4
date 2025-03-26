Markdown on COpenStreetMap:

    COpenStreetMap overrides the abstract structs and virtual methods from 
    CStreetMap. The implementations of these are given below. 

Methods: 

    NodeCount(): Use this to return the node count of a street map. 

    WayCount(): Use this to return the way count of a street map. 

    NodeByIndex(index): Use this to return the node at a given index. 

    NodeByID(id): Use this to return a node by its ID. If there isn't a node, returns nullptr. 

    WayByIndex(index): Use this to return a way at a given index. 

    WayByID(id): Use this to return a way by its ID. If there isn't a way, return nullptr. 

Classes: 

    SNodeData (implementation of SNode):
        ID() -> returns the unique NodeID
        Location() -> returns the node ID's coordinates
        AttributeCount() -> returns the number of attributes the node has
        GetAttributeKey(index) -> returns the attribute's key at a given index (not value)
        HasAttribute(key) -> given a key, returns true or false based on if it has a value for the key
        GetAttribute(key) -> returns the value of the attribute using the given key

    SWayData (implementation of SWay):
        ID() -> returns the unique WayID 
        NodeCount() -> returns the number of nodes in the way
        GetNodeID() -> returns NodeID by a given index
        AttributeCount() -> returns the number of attributes the way has
        GetAttributeKey(index) -> returns the key of an attribute given an index
        HasAttribute(key) -> returns true or false based on if the way has a value for the key
        GetAttribute(key) -> returns the value of the attribute using the given key