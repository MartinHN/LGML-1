/*
 ==============================================================================

 NodeConnection.cpp
 Created: 7 Mar 2016 12:39:02pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeConnection.h"
#include "NodeManager.h"

NodeConnection::NodeConnection(ConnectableNode * sourceNode, ConnectableNode * destNode, ConnectionType connectionType) :
sourceNode(sourceNode), 
destNode(destNode), 
connectionType(connectionType)
{

    // init with all possible Audio connections
    if(connectionType==AUDIO){
        int maxCommonAudioConnections = jmin(sourceNode->getAudioNode(true)->getProcessor()->getTotalNumOutputChannels() , destNode->getAudioNode(false)->getProcessor()->getTotalNumInputChannels());
        for( int i = 0 ; i < maxCommonAudioConnections ; i ++){
            addAudioGraphConnection(i, i);
        }
    }
}

NodeConnection::~NodeConnection()
{
    if(connectionType==AUDIO){removeAllAudioGraphConnections();}
    dataConnections.clear();
    sourceNode = nullptr;
    destNode = nullptr;
}

void NodeConnection::addAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    AudioConnection ac = AudioConnection(sourceChannel, destChannel);
    audioConnections.add(ac);
    NodeManager::getInstance()->audioGraph.addConnection(sourceNode->getAudioNode(false)->nodeId, sourceChannel, destNode->getAudioNode(true)->nodeId, destChannel);
    listeners.call(&Listener::connectionAudioLinkAdded, ac);
}

void NodeConnection::removeAudioGraphConnection(uint32 sourceChannel, uint32 destChannel)
{
    AudioConnection ac = AudioConnection(sourceChannel, destChannel);
	if(NodeManager::getInstanceWithoutCreating() != nullptr) NodeManager::getInstance()->audioGraph.removeConnection(sourceNode->getAudioNode(false)->nodeId, sourceChannel, destNode->getAudioNode(true)->nodeId, destChannel);
    audioConnections.removeAllInstancesOf(ac);
    listeners.call(&Listener::connectionAudioLinkRemoved, ac);


}
void NodeConnection::removeAllAudioGraphConnections()
{
    for(auto c:audioConnections){
        removeAudioGraphConnection(c.first,c.second);
    }

    audioConnections.clear();

}
void NodeConnection::addDataGraphConnection(Data * sourceData, Data * destData)
{
    DataProcessorGraph::Connection * c = NodeManager::getInstance()->dataGraph.addConnection(sourceData, destData);
    dataConnections.add(c);
    listeners.call(&Listener::connectionDataLinkAdded, c);
}

void NodeConnection::removeDataGraphConnection(Data * sourceData, Data * destData)
{
    DataProcessorGraph::Connection * c = NodeManager::getInstance()->dataGraph.getConnectionBetween(sourceData, destData);
    dataConnections.removeAllInstancesOf(c);
	NodeManager::getInstance()->dataGraph.removeConnection(c);
    listeners.call(&Listener::connectionDataLinkRemoved, c);
}

void NodeConnection::removeAllDataGraphConnections()
{
    for (auto &c : dataConnections) {
        removeDataGraphConnection(c->sourceData,c->destData);
    }
    dataConnections.clear();
}


void NodeConnection::remove()
{
    listeners.call(&NodeConnection::Listener::askForRemoveConnection,this);
}

void audioInputAdded(NodeBase * n, int channel) 
{
	//TODO HANDLE THAT !!!!
}

void audioInputRemoved(NodeBase * n, int channel)
{
	//TODO HANDLE THAT !!!!
}

void dataInputAdded(NodeBase * n, Data *) 
{
	//TODO HANDLE THAT !!!!
}

void dataInputRemoved(NodeBase * n, Data *) 
{
	//TODO HANDLE THAT !!!!
}

var NodeConnection::getJSONData()
{
    var data(new DynamicObject());
	
	ConnectableNode * tSource = (sourceNode->type == ContainerOutType) ? ((ContainerOutNode *)sourceNode)->getParentNodeContainer():sourceNode;
	ConnectableNode * tDest = (destNode->type == ContainerInType) ? ((ContainerInNode *)destNode)->getParentNodeContainer() : destNode;

    data.getDynamicObject()->setProperty("srcNode", tSource->shortName);
    data.getDynamicObject()->setProperty("dstNode", tDest->shortName);
    data.getDynamicObject()->setProperty("connectionType", (int)connectionType);

    var links;
    if (isAudio())
    {
        for (auto &c : audioConnections)
        {
            var cObject(new DynamicObject());
            cObject.getDynamicObject()->setProperty("sourceChannel", c.first);
            cObject.getDynamicObject()->setProperty("destChannel", c.second);
            links.append(cObject);
        }
    }
    else
    {
        for (auto &c : dataConnections)
        {
            var cObject(new DynamicObject());
            cObject.getDynamicObject()->setProperty("sourceData", c->sourceData->name);
            cObject.getDynamicObject()->setProperty("destData", c->destData->name);
            links.append(cObject);
        }
    }

    data.getDynamicObject()->setProperty("links", links);

    return data;
}

void NodeConnection::loadJSONData(var data)
{
    //srcNodeId, destNodeId & connectionType set at creation, not in this load

    DBG("Load JSON Data Node COnnection !");
    const Array<var> * links = data.getProperty("links",var()).getArray();

    if (links != nullptr)
    {
        if (isAudio())
        {
            removeAllAudioGraphConnections();
            for (var &linkVar : *links)
            {
                int sourceChannel = linkVar.getProperty("sourceChannel", var());
                int destChannel = linkVar.getProperty("destChannel", var());
                DBG("Add from JSON, " << sourceChannel << " > " <<destChannel);

                addAudioGraphConnection(sourceChannel, destChannel);
            }
        }
        else
        {
            removeAllDataGraphConnections();
            for (auto &linkVar : *links)
            {
                String sourceName = linkVar.getProperty("sourceData", var());
                String destName = linkVar.getProperty("destData", var());
                addDataGraphConnection(sourceNode->getOutputDataByName(sourceName), destNode->getInputDataByName(destName));
            }
        }
    }
}
