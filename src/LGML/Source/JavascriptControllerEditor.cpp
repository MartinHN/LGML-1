/*
  ==============================================================================

    JavascriptControllerEditor.cpp
    Created: 10 May 2016 8:01:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "JavascriptControllerEditor.h"
#include "JsEnvironmentUI.h"


JavascriptControllerEditor::JavascriptControllerEditor(JavascriptControllerUI * jsControllerUI) :
	OSCControllerEditor(jsControllerUI),
	jsController(jsControllerUI->jsController)
{
	scriptUI = new JsEnvironmentUI(jsController);
	innerContainer.addAndMakeVisible(scriptUI);
}

JavascriptControllerEditor::~JavascriptControllerEditor()
  {
  }

void JavascriptControllerEditor::resized()
{
	OSCControllerEditor::resized();
	Rectangle<int> r = innerContainer.getLocalBounds();
	scriptUI->setBounds(r);

}




int JavascriptControllerEditor::getContentHeight(){
    return OSCControllerEditor::getContentHeight() + 30;

}