/*
  ==============================================================================

    Engine.h
    Created: 2 Apr 2016 11:03:21am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

//#include "JuceHeader.h"


#include "ControlManager.h"
#include "TimeManager.h"
#include "NodeManager.h"
#include "VSTManager.h"

class Engine:public FileBasedDocument{
public:
    Engine();
    ~Engine();


    // Audio

    AudioProcessorPlayer graphPlayer;

    void createNewGraph();
    void clear();
    void initAudio();
    void stopAudio();

	void parseCommandline(const String & );

    //==============================================================================
    // see EngineFileDocument.cpp

    //  inherited from FileBasedDocument
    String getDocumentTitle()override ;
    Result loadDocument (const File& file)override;
    Result saveDocument (const File& file)override;
    File getLastDocumentOpened() override;
    void setLastDocumentOpened (const File& file) override;

    //    #if JUCE_MODAL_LOOPS_PERMITTED
    //     File getSuggestedSaveAsFile (const File& defaultFile)override;
    //    #endif

    // our Saving methods
    var getJSONData();
    void loadJSONData(var data, bool clearManagers = true);

};


#endif  // ENGINE_H_INCLUDED
