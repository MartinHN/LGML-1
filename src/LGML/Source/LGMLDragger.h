/*
 ==============================================================================

 ComponentDragger.h
 Created: 20 Jul 2017 8:47:29am
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "JuceHeader.h"

//DBGinclude
#include "Controllable.h"


class DraggedComponent;
class LGMLDragger : MouseListener{
public:

  LGMLDragger();
  ~LGMLDragger();
  juce_DeclareSingleton(LGMLDragger, true);

  void setMainComponent(Component * c,TooltipWindow * tip);

  Component * mainComp;
  TooltipWindow * tip;

  void registerDragCandidate(ControllableUI * c);
  void unRegisterDragCandidate(ControllableUI * c);


  void mouseEnter(const MouseEvent &e)override;
  void mouseUp(const MouseEvent &e)override;
  void mouseExit(const MouseEvent &e) override;
  
  
  void startDraggingComponent (Component* const componentToDrag, const MouseEvent& e);
  void dragComponent (Component* const componentToDrag, const MouseEvent& e,ComponentBoundsConstrainer* const constrainer);
  void endDraggingComponent(Component *  componentToDrag,const MouseEvent & e);



  ScopedPointer<DraggedComponent> dragCandidate;
  void setMappingActive(bool isActive);
  void toggleMappingMode();
  bool isMappingActive;

  void setSelected(ControllableUI *);

  Component*  dropCandidate;

  WeakReference<Component> selected;


  class Listener{
  public:
    virtual ~Listener(){};
    virtual void selectionChanged(Controllable *) = 0;
  };
  void addSelectionListener(Listener* l ){listeners.add(l);}
  void removeSelectionListener(Listener* l ){listeners.remove(l);}
private:

  Component * selectedSSContent;
  ListenerList<Listener> listeners;
  Point<int> mouseDownWithinTarget;


};

