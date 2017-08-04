/*
 ==============================================================================

 ConnectableNodeUI.cpp
 Created: 18 May 2016 11:34:23pm
 Author:  bkupe

 ==============================================================================
 */

#include "ConnectableNodeUI.h"
#include "ConnectableNodeHeaderUI.h"
#include "NodeContainer.h"
#include "ContainerInNode.h"
#include "ContainerOutNode.h"


// needed because main component need to know it for its scopedPointer
#include "FloatSliderUI.h"


#include "NodeManagerUI.h"

ConnectableNodeUI::ConnectableNodeUI(ConnectableNode * cn, ConnectableNodeContentUI * contentUI, ConnectableNodeHeaderUI * headerUI) :
InspectableComponent(cn, "node"),
connectableNode(cn),
inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT),
mainComponentContainer(this, contentUI, headerUI),
dragIsLocked(false),
bMiniMode(false),
resizer(this,&constrainer),
isDraggingFromUI(false)
{
  connectorWidth = 10;

  constrainer.setMinimumWidth(50);
  constrainer.setMinimumHeight(50);

  addAndMakeVisible(mainComponentContainer);

	 if (connectableNode->userCanAccessInputs)
   {
     inputContainer.setConnectorsFromNode(connectableNode);
     addAndMakeVisible(inputContainer);
   }

	 if (connectableNode->userCanAccessOutputs)
   {
     outputContainer.setConnectorsFromNode(connectableNode);
     addAndMakeVisible(outputContainer);
   }

	 getHeaderContainer()->addMouseListener(this, true);// (true, true);
  getContentContainer()->addMouseListener(this, false);

	 mainComponentContainer.setNodeAndNodeUI(connectableNode, this);

	 connectableNode->addConnectableNodeListener(this);
	 connectableNode->position->hideInEditor = true;

	 connectableNode->nodeWidth->hideInEditor = true;
	 connectableNode->nodeHeight->hideInEditor = true;


	 addAndMakeVisible(&resizer);

	 //connectableNode->miniMode->hideInEditor = true;

	 setMiniMode(connectableNode->miniMode->boolValue());



}

ConnectableNodeUI::~ConnectableNodeUI()
{

  connectableNode->removeConnectableNodeListener(this);
}



void ConnectableNodeUI::moved()
{
  isDraggingFromUI = true;
  connectableNode->position->setPoint(getPosition());
  isDraggingFromUI = false;
}


void ConnectableNodeUI::setMiniMode(bool value)
{
  if (bMiniMode == value) return;

  bMiniMode = value;

  mainComponentContainer.setMiniMode(bMiniMode);
  setSize(getMiniModeWidth(bMiniMode),getMiniModeHeight(bMiniMode));
}

int ConnectableNodeUI::getMiniModeWidth(bool forMiniMode)
{
  return forMiniMode ? 180 : (getContentContainer()->getWidth() + inputContainer.getWidth()+outputContainer.getWidth() + (mainComponentContainer.audioCtlUIContainer?mainComponentContainer.audioCtlUIContainer->getWidth()+mainComponentContainer.audioCtlContainerPadRight:0));
}

int ConnectableNodeUI::getMiniModeHeight(bool forMiniMode)
{
  return getHeaderContainer()->getBottom() + (forMiniMode?10:getContentContainer()->getHeight());
}

void ConnectableNodeUI::paint(Graphics&)
{

}

void ConnectableNodeUI::resized()
{
  //	if (!connectableNode->miniMode->boolValue())
  //	{





  Rectangle<int> r = getLocalBounds();
  Rectangle<int> inputBounds = r.removeFromLeft(connectorWidth);
  Rectangle<int> outputBounds = r.removeFromRight(connectorWidth);

  if (connectableNode->userCanAccessInputs)
  {
    inputContainer.setBounds(inputBounds);
  }

  if (connectableNode->userCanAccessOutputs)
  {
    outputContainer.setBounds(outputBounds);
  }

  mainComponentContainer.setBounds(r);
  resizer.setBounds(r.removeFromRight(10).removeFromBottom(10));

}

void ConnectableNodeUI::nodeParameterChanged(ConnectableNode *, Parameter * p)
{
  if (p == connectableNode->position )
  {
    if(!isDraggingFromUI)postCommandMessage(posChangedId);
  }
  else if( p == connectableNode->nodeHeight || p == connectableNode->nodeWidth) {
    if(!isDraggingFromUI)postCommandMessage(sizeChangedId);
  }

  else if (p == connectableNode->enabledParam)
  {
    postCommandMessage(repaintId);
  } else if (p == connectableNode->miniMode)
  {
    postCommandMessage(setMiniModeId);
  }
}

void ConnectableNodeUI::handleCommandMessage(int commandId){
  switch(commandId){
    case repaintId:
      repaint();
      break;
    case setMiniModeId:
      setMiniMode(connectableNode->miniMode->boolValue());
      break;
    case posChangedId:
      setTopLeftPosition(connectableNode->position->getPoint());
      break;
    case sizeChangedId:
      getContentContainer()->setSize(connectableNode->nodeWidth->intValue(), connectableNode->nodeHeight->intValue());
      resized();
      break;

    default:
      break;
  }
}

// allow to react to custom MainComponentContainer.contentContainer
void ConnectableNodeUI::childBoundsChanged(Component* c) {
  // if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
  if (c == &mainComponentContainer) {
    int destWidth = mainComponentContainer.getWidth() + 2 * connectorWidth;
    int destHeight = mainComponentContainer.getHeight();
    if (getWidth() != destWidth ||
        destHeight != getHeight()) {
      setSize(destWidth, destHeight);
    }
  }
}


void ConnectableNodeUI::mouseDown(const juce::MouseEvent &/*e*/)
{
  selectThis();
  //	if (e.eventComponent != &mainComponentContainer.headerContainer->grabber) return;
  //  if (e.eventComponent->getParentComponent() != mainComponentContainer.headerContainer) return;
  isDraggingFromUI = true;
  nodeInitPos = getBounds().getPosition();
}



void ConnectableNodeUI::mouseUp(const juce::MouseEvent &) {
  
  isDraggingFromUI = false;
}

void ConnectableNodeUI::mouseDrag(const MouseEvent & e)
{
  //	if (e.eventComponent->getParentComponent() != mainComponentContainer.headerContainer) return;
  //if(dragIsLocked) return;

  isDraggingFromUI = true;
  Point<int> diff = Point<int>(e.getPosition() - e.getMouseDownPosition());
  Point <int> newPos = nodeInitPos + diff;

  connectableNode->position->setPoint(newPos);
  setTopLeftPosition(newPos);

}

bool ConnectableNodeUI::keyPressed(const KeyPress & key)
{
  if (!isSelected) return false;
  if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
  {
    if (connectableNode->canBeRemovedByUser)
    {
      connectableNode->remove();
    }

    return true;
  }
  else if(key.getModifiers().isCommandDown() && key.getKeyCode()==KeyPress::downKey){
    if(NodeContainer * c = dynamic_cast<NodeContainer * >(connectableNode.get())){
      if(NodeManagerUI * manager = findParentComponentOfClass<NodeManagerUI>()){
        manager->setCurrentViewedContainer(c);
        return true;
      }
    }
  }

  return false;
}

////////////    MAIN CONTAINER


ConnectableNodeUI::MainComponentContainer::MainComponentContainer(ConnectableNodeUI * _nodeUI, ConnectableNodeContentUI * content, ConnectableNodeHeaderUI * header) :
connectableNodeUI(_nodeUI),
headerContainer(header),
contentContainer(content),
audioCtlUIContainer(nullptr),
miniMode(false)
{

  if (headerContainer == nullptr) headerContainer = new ConnectableNodeHeaderUI();
  if (contentContainer == nullptr) contentContainer = new ConnectableNodeContentUI();


  addAndMakeVisible(headerContainer);
  addAndMakeVisible(contentContainer);

}

void ConnectableNodeUI::MainComponentContainer::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
  if (_node->hasAudioOutputs() && _node->hasMainAudioControl) {
    jassert(audioCtlUIContainer == nullptr);
    audioCtlUIContainer = new ConnectableNodeAudioCtlUI();
    addAndMakeVisible(audioCtlUIContainer);
    audioCtlUIContainer->setNodeAndNodeUI(_node, _nodeUI);
  }

  headerContainer->setNodeAndNodeUI(_node, _nodeUI);
  contentContainer->setNodeAndNodeUI(_node, _nodeUI);

  resized();
}

void ConnectableNodeUI::MainComponentContainer::paint(Graphics & g)
{
  if(!connectableNodeUI->connectableNode.get())return;
  g.setColour(connectableNodeUI->connectableNode->enabledParam->boolValue() ? PANEL_COLOR : PANEL_COLOR.darker(.7f));
  g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);

  g.setColour(connectableNodeUI->isSelected ? HIGHLIGHT_COLOR : LIGHTCONTOUR_COLOR);
  g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1), 4.f, connectableNodeUI->isSelected ? 2.f : .5f);
}


void ConnectableNodeUI::MainComponentContainer::resized()
{

  // if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
  Rectangle<int> r = getLocalBounds();
  if (r.getWidth() == 0 || r.getHeight() == 0)return;

  Rectangle<int> headerBounds = r.removeFromTop(headerContainer->getHeight());
  headerContainer->setBounds(headerBounds);

  if (!miniMode)
  {

    if (audioCtlUIContainer) {
      r.removeFromRight(audioCtlContainerPadRight);
      audioCtlUIContainer->setBounds(r.removeFromRight(audioCtlContainerWidth).reduced(0, 4));
    }
    connectableNodeUI->connectableNode->nodeWidth->setValue(r.getWidth(),true);
    connectableNodeUI->connectableNode->nodeHeight->setValue(r.getHeight(), true);
    contentContainer->setBounds(r);
  }
}

void ConnectableNodeUI::MainComponentContainer::setMiniMode(bool value)
{
  if (miniMode == value) return;
  miniMode = value;

  if (miniMode)
  {
    removeChildComponent(contentContainer);
    if(audioCtlUIContainer) removeChildComponent(audioCtlUIContainer);
  }else
  {
    addChildComponent(contentContainer);
    if (audioCtlUIContainer) addChildComponent(audioCtlUIContainer);
  }

  headerContainer->setMiniMode(miniMode);

}

void ConnectableNodeUI::MainComponentContainer::childBoundsChanged(Component* c) {
  if (c == contentContainer || c == audioCtlUIContainer) {
    int destWidth = contentContainer->getWidth() +
      (audioCtlUIContainer ? (audioCtlContainerWidth+ audioCtlContainerPadRight ): 0);
    int destHeight = contentContainer->getHeight() + headerContainer->getHeight();
    if (getWidth() != destWidth ||
        getHeight() != destHeight) {
      setSize(destWidth, destHeight);
    }
  }
}







// ======= CONNECTOR CONTAINER AND CONNECTOR COMPONENT ===================
ConnectableNodeUI::ConnectorContainer::ConnectorContainer(ConnectorComponent::ConnectorIOType type) : type(type), displayLevel(ConnectorComponent::MINIMAL)
{
  setInterceptsMouseClicks(false, true);

}

void ConnectableNodeUI::ConnectorContainer::setConnectorsFromNode(ConnectableNode * _node)
{
  connectors.clear();

  ConnectableNode * targetNode = _node;

  if (targetNode == nullptr)
  {
    DBG("Target Node nullptr !");
    return;
  }


  addConnector(type, NodeConnection::ConnectionType::AUDIO, targetNode);
  addConnector(type, NodeConnection::ConnectionType::DATA, targetNode);

  resized();

}

void ConnectableNodeUI::ConnectorContainer::addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode * _node)
{
  ConnectorComponent * c = new ConnectorComponent(ioType, dataType, _node);
  connectors.add(c);
  addChildComponent(c);

  c->addConnectorListener(this);
}

void ConnectableNodeUI::ConnectorContainer::connectorVisibilityChanged(ConnectorComponent *)
{
  resized();
}


void ConnectableNodeUI::ConnectorContainer::resized()
{
  Rectangle<int> r = getLocalBounds();

  r.removeFromTop(10);
  for (auto &c : connectors)
  {
    if (!c->isVisible()) continue;

    c->setBounds(r.removeFromTop(r.getWidth()));
    r.removeFromTop(15);

  }
}

ConnectorComponent * ConnectableNodeUI::ConnectorContainer::getFirstConnector(NodeConnection::ConnectionType dataType)
{
  for (int i = 0; i < connectors.size(); i++)
  {
    if (connectors.getUnchecked(i)->dataType == dataType) return connectors.getUnchecked(i);
  }

  return nullptr;
}



Array<ConnectorComponent*> ConnectableNodeUI::getComplementaryConnectors(ConnectorComponent * baseConnector)
{
  Array<ConnectorComponent *> result;


  ConnectorContainer * checkSameCont = baseConnector->ioType == ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;
  if (checkSameCont->getIndexOfChildComponent(baseConnector) != -1) return result;

  ConnectorContainer * complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;
  for (int i = 0; i < complCont->connectors.size(); i++)
  {
    ConnectorComponent *c = (ConnectorComponent *)complCont->getChildComponent(i);
    if (c->dataType == baseConnector->dataType)
    {
      result.add(c);
    }
  }

  return result;
}

//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes

ConnectorComponent * ConnectableNodeUI::getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType)
{
  if (ioType == ConnectorComponent::INPUT)
  {
    return inputContainer.getFirstConnector(connectionType);
  }
  else
  {
    return outputContainer.getFirstConnector(connectionType);
  }
}
