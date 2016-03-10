/*
  ==============================================================================

    TriggerButtonUI.cpp
    Created: 8 Mar 2016 3:45:53pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TriggerButtonUI.h"

#include "Style.h"

//==============================================================================
TriggerButtonUI::TriggerButtonUI(Trigger *t):
	TriggerUI(t)
{
	setSize(20,15);
}

TriggerButtonUI::~TriggerButtonUI()
{
    trigger->removeTriggerListener(this);
}

void TriggerButtonUI::triggerTriggered(Trigger * p){
	repaint();
}

void TriggerButtonUI::paint (Graphics& g)
{
	setAlpha(trigger->enabled ? 1 : .5);

	Point<int> center = getBounds().getCentre();


	Colour c = isMouseOverOrDragging() ? (isMouseButtonDown() ? HIGHLIGHT_COLOR : NORMAL_COLOR.brighter()) : NORMAL_COLOR;

	g.setGradientFill(ColourGradient(c,center.x,center.y,c.darker(.2f),2,2,true));
	g.fillRoundedRectangle(getLocalBounds().toFloat(),4);
	g.setColour(c.darker());
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 4,2);
}

void TriggerButtonUI::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void TriggerButtonUI::mouseDown (const MouseEvent& event){
    trigger->trigger();
   
}

void TriggerButtonUI::mouseExit(const MouseEvent & event)
{
	repaint();
}

void TriggerButtonUI::mouseUp(const MouseEvent & event)
{
	repaint();
}

void TriggerButtonUI::mouseEnter(const MouseEvent & event)
{
	repaint();
}

