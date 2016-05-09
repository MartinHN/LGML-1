/*
 ==============================================================================

 JavaScriptController.h
 Created: 5 May 2016 10:15:48am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTCONTROLLER_H_INCLUDED
#define JAVASCRIPTCONTROLLER_H_INCLUDED


#include "OSCDirectController.h"
#include "JavascriptEnvironment.h"

class JavascriptController : public OSCDirectController ,public JavascriptEnvironment{
public:
    JavascriptController();
    ~JavascriptController();
    Result processMessage(const OSCMessage &m)override;
    Result callForMessage(const OSCMessage & msg);
    void callonAnyMsg(const OSCMessage & msg);

    void buildLocalNamespace() override;
    static var sendOSCFromJS(const JavascriptEnvironment::NativeFunctionArgs& a);
    void onContainerParameterChanged(Parameter * p) override;


    void newJsFileLoaded()override;
    ControllerUI * createUI() override;


private:
    String getJavaScriptFunctionName(const String &);
    StringArray nonValidMessages;
    bool hasAnyMsgMethod;

    DynamicObject * createOSCJsObject();

};


#endif  // JAVASCRIPTCONTROLLER_H_INCLUDED