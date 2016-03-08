/*
  ==============================================================================

    ControllerContentUI.h
    Created: 8 Mar 2016 10:48:41pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERCONTENTUI_H_INCLUDED
#define CONTROLLERCONTENTUI_H_INCLUDED

#include "ControllerUI.h"
#include "UIHelpers.h"

class ControllerContentUI : public ContourComponent
{
public:
	ControllerContentUI();

	Controller * controller;
	ControllerUI * cui;

	void setControllerAndUI(Controller * controller, ControllerUI * cui);
	virtual void init(); //override this function for proper init with controller and UI


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerContentUI)
};



#endif  // CONTROLLERCONTENTUI_H_INCLUDED
