/*
 ==============================================================================

 NodeBaseHeaderUI.cpp
 Created: 8 Mar 2016 5:53:52pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeBaseHeaderUI.h"

NodeBaseHeaderUI::NodeBaseHeaderUI() : removeBT("X")
{
    node = nullptr;
    nodeUI = nullptr;

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
						0.7f, Colours::transparentBlack,
						removeImage, 1.0f, Colours::transparentBlack,
						removeImage, 1.0f, Colours::pink.withAlpha(0.8f),
						0.5f);
    setSize(20, 30);
}

NodeBaseHeaderUI::~NodeBaseHeaderUI()
{
    if (node != nullptr && node->hasAudioOutputs) {
        node->audioProcessor->removeRMSListener(&vuMeter);
    }

}

void NodeBaseHeaderUI::setNodeAndNodeUI(NodeBase * _node, NodeBaseUI * _nodeUI)
{
    this->node = _node;
    this->nodeUI = _nodeUI;

    if (node != nullptr && node->hasAudioOutputs) {
        node->audioProcessor->addRMSListener(&vuMeter);
        addAndMakeVisible(vuMeter);
    }

    titleUI = node->nameParam->createStringParameterUI();
    titleUI->setNameLabelVisible(false);
    titleUI->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(titleUI);

    enabledUI = node->enabledParam->createToggle();
    addAndMakeVisible(enabledUI);

	addAndMakeVisible(grabber);
	addAndMakeVisible(removeBT);

    init();

}
void NodeBaseHeaderUI::mouseDoubleClick(const MouseEvent &){
    if(titleUI){titleUI->valueLabel.showEditor();}
}

void NodeBaseHeaderUI::init()
{
    //to override

}

void NodeBaseHeaderUI::resized()
{
	if (!node) return;

	int vuMeterWidth = 14;
	int removeBTWidth = 15;
	int grabberWidth = 40;

	Rectangle<int> r = getLocalBounds();

	if (node->hasAudioOutputs) {
		Rectangle<int> vuMeterRect = r.removeFromRight(vuMeterWidth).reduced(4);
		vuMeter.setBounds(vuMeterRect);
	}

	r.reduce(5, 2);
    r.removeFromLeft(enabledUI->getWidth());
        
	removeBT.setBounds(r.removeFromRight(removeBTWidth));
	grabber.setBounds(r.removeFromRight(grabberWidth));
    titleUI->setBounds(r);

	enabledUI->setTopLeftPosition(5, 5);

}