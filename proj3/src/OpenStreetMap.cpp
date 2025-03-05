#include "OpenStreetMap.h"
#include "XMLReader.h"
#include <memory> //need for different porinters
#include <vector>
#include <string>
#include <unordered_map>
//implementation of details 
struct COpenStreetMap::SImplementation {
    class SNodeData;
    class SWayData;
    std::vector<std::shared_ptr<SNodeData>> NodeList;
    std::vector<std::shared_ptr<SWayData>> WayList;
};
//this reps a single ind node in the data
class COpenStreetMap::SImplementation::SNodeData : public CStreetMap::SNode {
public:
    TNodeID Identifier; // this idfnitifies a unique node
    TLocation Coordinates;// coords of the node so lat and long
    std::unordered_map<std::string, std::string> Properties;
//the atteriburtes of th enode
    TNodeID ID() const noexcept override {
        return Identifier;//returns unique id
    }

    TLocation Location() const noexcept override {
        return Coordinates;//obv returns the coords
    }

    std::size_t AttributeCount() const noexcept override {
        return Properties.size();//returns the number of attributes
    }

    std::string GetAttributeKey(std::size_t idx) const noexcept override {
        if (idx >= Properties.size()) {
            return "";
        }
        auto iter = std::next(Properties.begin(), idx);
        return iter->first;
    }

    bool HasAttribute(const std::string &key) const noexcept override {
        return Properties.count(key) > 0;
    }

    std::string GetAttribute(const std::string &key) const noexcept override {
        //gets the value of attribtue by the key 
        auto iter = Properties.find(key);
        return (iter != Properties.end()) ? iter->second : "";
    }
};

class COpenStreetMap::SImplementation::SWayData : public CStreetMap::SWay {
public:
    TWayID Identifier;
    std::vector<TNodeID> NodeReferences;
    std::unordered_map<std::string, std::string> Properties;

    TWayID ID() const noexcept override {
        return Identifier;
    }

    std::size_t NodeCount() const noexcept override {
        return NodeReferences.size();
    }

    TNodeID GetNodeID(std::size_t idx) const noexcept override {
        //this gets a node by index
        if (idx < NodeReferences.size()) {
            return NodeReferences[idx];
        }
        return CStreetMap::InvalidNodeID;
    }
//count the num of attributes the way has
    std::size_t AttributeCount() const noexcept override {
        return Properties.size();
    }
//get attribtue key by index
    std::string GetAttributeKey(std::size_t idx) const noexcept override {
        if (idx >= Properties.size()) {
            return "";
        }
        auto iter = std::next(Properties.begin(), idx);
        return iter->first;
    }

    bool HasAttribute(const std::string &key) const noexcept override {
        return Properties.count(key) > 0;
    }

    std::string GetAttribute(const std::string &key) const noexcept override {
        auto iter = Properties.find(key);
        return (iter != Properties.end()) ? iter->second : "";
    }
};

COpenStreetMap::COpenStreetMap(std::shared_ptr<CXMLReader> source) {
    DImplementation = std::make_unique<SImplementation>();
    
    SXMLEntity xmlEntity;
    std::shared_ptr<SImplementation::SNodeData> activeNode = nullptr;
    std::shared_ptr<SImplementation::SWayData> activeWay = nullptr;
    
    while (source->ReadEntity(xmlEntity)) {
        if (xmlEntity.DType == SXMLEntity::EType::StartElement) {
            if (xmlEntity.DNameData == "node") {
                activeNode = std::make_shared<SImplementation::SNodeData>();
                activeWay = nullptr;
                
                for (const auto& attribute : xmlEntity.DAttributes) {
                    if (attribute.first == "id") {
                        activeNode->Identifier = std::stoull(attribute.second);
                    } else if (attribute.first == "lat") {
                        activeNode->Coordinates.first = std::stod(attribute.second);
                    } else if (attribute.first == "lon") {
                        activeNode->Coordinates.second = std::stod(attribute.second);
                    } else {
                        activeNode->Properties[attribute.first] = attribute.second;
                    }
                }
            } else if (xmlEntity.DNameData == "way") { //uif it finds a new way
                activeWay = std::make_shared<SImplementation::SWayData>();
                activeNode = nullptr;
                
                for (const auto& attribute : xmlEntity.DAttributes) {
                    if (attribute.first == "id") {
                        activeWay->Identifier = std::stoull(attribute.second);
                    } else {
                        activeWay->Properties[attribute.first] = attribute.second;
                    }
                }
                //way referes to node
            } else if (xmlEntity.DNameData == "nd" && activeWay) {
                for (const auto& attribute : xmlEntity.DAttributes) {
                    if (attribute.first == "ref") {
                        activeWay->NodeReferences.push_back(std::stoull(attribute.second));
                    }
                }
                //this is attirbute for the node or way
            } else if (xmlEntity.DNameData == "tag") {
                std::string key, value;
                for (const auto& attribute : xmlEntity.DAttributes) {
                    if (attribute.first == "k") {
                        key = attribute.second;
                    } else if (attribute.first == "v") {
                        value = attribute.second;
                    }
                }
                if (!key.empty()) {
                    if (activeNode) {
                        activeNode->Properties[key] = value;
                    } else if (activeWay) {
                        activeWay->Properties[key] = value;
                    }
                }
            }
        } else if (xmlEntity.DType == SXMLEntity::EType::EndElement) {
            if (xmlEntity.DNameData == "node" && activeNode) {
                DImplementation->NodeList.push_back(activeNode);
                activeNode = nullptr;
            } else if (xmlEntity.DNameData == "way" && activeWay) {
                DImplementation->WayList.push_back(activeWay);
                activeWay = nullptr;
            }
        }
    }
}

COpenStreetMap::~COpenStreetMap() = default;

std::size_t COpenStreetMap::NodeCount() const noexcept {
    return DImplementation->NodeList.size();
}

std::size_t COpenStreetMap::WayCount() const noexcept {
    return DImplementation->WayList.size();
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByIndex(std::size_t index) const noexcept {
    return (index < DImplementation->NodeList.size()) ? DImplementation->NodeList[index] : nullptr;
}

std::shared_ptr<CStreetMap::SNode> COpenStreetMap::NodeByID(TNodeID id) const noexcept {
    for (const auto& node : DImplementation->NodeList) {
        if (node->ID() == id) {
            return node;
        }
    }
    return nullptr;
    //must return null
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByIndex(std::size_t index) const noexcept {
    return (index < DImplementation->WayList.size()) ? DImplementation->WayList[index] : nullptr;
}

std::shared_ptr<CStreetMap::SWay> COpenStreetMap::WayByID(TWayID id) const noexcept {
    for (const auto& way : DImplementation->WayList) {
        if (way->ID() == id) {
            return way;
        }
    }
    return nullptr;
    //must return unull
}