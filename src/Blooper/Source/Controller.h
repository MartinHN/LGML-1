/*
  ==============================================================================

    Controller.h
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "JuceHeader.h"
class ControllerUI;

class Controller
{
public:
	Controller();
	virtual ~Controller();

	ControllerUI * createUI();

	void remove(); //will dispatch askForRemoveController

	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}

		virtual void askForRemoveController(Controller *) = 0;
	};

	ListenerList<Listener> listeners;
	void addListener(Listener* newListener) { listeners.add(newListener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controller)
};



#endif  // CONTROLLER_H_INCLUDED
