/*
  ==============================================================================

    Engine.cpp
    Created: 2 Apr 2016 11:03:21am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "Engine.h"

#include "NodeConnectionEditor.h"

const char* const filenameSuffix = ".lgml";
const char* const filenameWildcard = "*.lgml";

Engine::Engine():FileBasedDocument (filenameSuffix,
                                    filenameWildcard,
                                    "Load a filter graph",
                                    "Save a filter graph"){



        initAudio();
}


Engine::~Engine(){
    stopAudio();
    TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
    ControllerManager::deleteInstance();
    NodeManager::deleteInstance();
    VSTManager::deleteInstance();

}




void Engine::initAudio(){

    graphPlayer.setProcessor(&NodeManager::getInstance()->audioGraph);
    ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()->getXmlValue ("audioDeviceState"));
    getAudioDeviceManager().initialise (256, 256, savedAudioState, true);

    getAudioDeviceManager().addAudioCallback (&graphPlayer);
    getAudioDeviceManager().addAudioCallback(TimeManager::getInstance());
    DBG("init audio");

}


void Engine::stopAudio(){
    getAudioDeviceManager().removeAudioCallback (&graphPlayer);
    getAudioDeviceManager().removeAudioCallback(TimeManager::getInstance());
    getAudioDeviceManager().closeAudioDevice();
}


void Engine::clear(){
    //    do we need to stop audio?
     //stopAudio();


    TimeManager::getInstance()->stop();
    ControllerManager::getInstance()->clear();
    NodeManager::getInstance()->clear();

    changed();    //fileDocument
}

void Engine::createNewGraph(){
    clear();

    NodeBase * node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioIn);
    node->xPosition->setValue(150);
    node->yPosition->setValue(50);
    node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioOut);
    node->xPosition->setValue(450);
    node->yPosition->setValue(50);
    changed();
}