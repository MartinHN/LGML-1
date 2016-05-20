/*
 ==============================================================================

 This file was auto-generated by the Introjucer!

 It contains the basic startup code for a Juce application.

 ==============================================================================
 */


#include "MainComponent.h"

MainContentComponent* createMainContentComponent(Engine* e);

//==============================================================================
class LGMLApplication : public JUCEApplication
{
public:
    //==============================================================================
    LGMLApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

#ifdef LGML_UNIT_TESTS
        UnitTestRunner tstRunner;
        tstRunner.runAllTests();

#else
        PropertiesFile::Options options;
        options.applicationName     = "LGML";
        options.filenameSuffix      = "settings";
        options.osxLibrarySubFolder = "Preferences";

        appProperties = new ApplicationProperties();
        appProperties->setStorageParameters (options);
        Process::setPriority (Process::HighPriority);

        engine = new Engine();

        mainWindow = new MainWindow (getApplicationName(),engine);

        engine->parseCommandline(commandLine);

#endif

    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        mainWindow = nullptr; // (deletes our window)
        engine = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& /*commandLine*/) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.


    }

    //==============================================================================
    /*
     This class implements the desktop window that contains an instance of
     our MainContentComponent class.
     */
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name,Engine * e)  : DocumentWindow (name,
                                                               Colours::lightgrey,
                                                               DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            MainContentComponent * mainComponent = createMainContentComponent(e);
            setContentOwned (mainComponent, true);
            setResizable (true, true);

            centreWithSize (getWidth(), getHeight());
            setVisible (true);

#if JUCE_WINDOWS
            setMenuBar(mainComponent);
#endif
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
         class uses a lot of them, so by overriding you might break its functionality.
         It's best to do all your work in your content component instead, but if
         you really have to override any DocumentWindow methods, make sure your
         subclass also calls the superclass's method.
         */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    ApplicationCommandManager commandManager;
    ScopedPointer<ApplicationProperties> appProperties;
    AudioDeviceManager deviceManager;
    UndoManager undoManager;

    ScopedPointer<Engine> engine;
private:
    ScopedPointer<MainWindow> mainWindow;

};


static LGMLApplication& getApp()                 { return *dynamic_cast<LGMLApplication*>(JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager()      { return getApp().commandManager; }
ApplicationProperties& getAppProperties()           { return *getApp().appProperties; }
AudioDeviceManager & getAudioDeviceManager()        { return getApp().deviceManager;}
UndoManager & getAppUndoManager()                      { return getApp().undoManager;}
//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (LGMLApplication)
