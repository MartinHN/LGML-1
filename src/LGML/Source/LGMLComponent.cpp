/*
  ==============================================================================

    LGMLComponent.cpp
    Created: 20 Jul 2017 8:58:32am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "LGMLComponent.h"

#include "ControllableContainer.h"
#include "LGMLDragger.h"
LGMLComponent::LGMLComponent(bool _isDraggable):isDraggable(_isDraggable){
  type = NONE;
  addMouseListener(this,true);
  isMapping = LGMLDragger::getInstance()->isMappingActive;
}

void LGMLComponent::setLGMLElement(Controllable * c){
  clearElement();
  controllable = c;
  setName( c->shortName);
  type = CONTROLLABLE;
}
void LGMLComponent::setLGMLElement(ControllableContainer * c){
  clearElement();
  container = c;
  setName( c->shortName);
  type = CONTAINER;
}

void LGMLComponent::clearElement(){
controllable = nullptr;
  container = nullptr;
  type = NONE;
}
void LGMLComponent::setIsMapping(bool b){
  isMapping = b;
  repaint();
}
void LGMLComponent::paintOverChildren(Graphics &g) {
  Component::paintOverChildren(g);
  if(isMapping ){
  g.setColour(Colours::red.withAlpha(0.5f));
  g.fillAll();
  }
}
void LGMLComponent::mouseEnter(const MouseEvent &e){
  Component::mouseEnter(e);
  if(isMapping && isDraggable){
    LGMLDragger::getInstance()->registerForDrag(this);
  }
};
void LGMLComponent::mouseExit(const MouseEvent &e){
  Component::mouseExit(e);
if(isMapping && isDraggable && !contains(e.getEventRelativeTo(this).getPosition())){
  LGMLDragger::getInstance()->unRegisterForDrag(this);
  repaint();
}
};