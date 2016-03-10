/*
  ==============================================================================

    TimeManagerUI.h
    Created: 8 Mar 2016 11:06:41pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef TIMEMANAGERUI_H_INCLUDED
#define TIMEMANAGERUI_H_INCLUDED

#include "TimeManager.h"


class TimeManagerUI : public Component{
    
    public :
    TimeManagerUI(){
        addAndMakeVisible(timeBar);
        
    }
    
    
    void resized()override{
        Rectangle<int> area = getLocalBounds();
        timeBar.setBounds(area.removeFromRight(area.getWidth()/2));
    }
    
    
    class TimeBar : public Component,public TimeManager::Listener,public Timer{
        public :
        TimeBar(){
             TimeManager::getInstance()->addTimeManagerListener(this);
        }
        
        void  async_play()override{
            startTimerHz(30);
        }
        void async_stop()override{
            repaint();
            stopTimer();
        }
        void timerCallback()override{
            repaint();
        }
        void paint(Graphics & g) override{
            Rectangle<int> area = getLocalBounds();
            int beatPerBar =TimeManager::getInstance()->beatPerBar;
            int beatWidth = area.getWidth()/beatPerBar;
            int lastBeat =TimeManager::getInstance()->getBeat()%beatPerBar;
            float widthCurrentPart = TimeManager::getInstance()->getBeatPercent();
            g.setColour(Colours::green);
            g.fillRect(area.withWidth(beatWidth*lastBeat));
            g.setColour(Colours::orange);
            g.fillRect(area.withLeft(beatWidth*lastBeat).withWidth(widthCurrentPart*beatWidth));
            g.setColour(Colours::grey);
            int beatBarWidth  =4;
            for(int i = 0 ; i < beatPerBar+1  ; i++){
                g.fillRect(area.withLeft(i*beatWidth - beatBarWidth/2).withWidth(beatBarWidth));
            }
        }
        
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeBar)
        
    };
    
    TimeBar timeBar;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManagerUI)
};


#endif  // TIMEMANAGERUI_H_INCLUDED