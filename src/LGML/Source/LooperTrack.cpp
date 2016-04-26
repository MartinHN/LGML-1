/*
  ==============================================================================

    LooperTrack.cpp
    Created: 26 Apr 2016 4:11:41pm
    Author:  bkupe

  ==============================================================================
*/


#include "LooperTrack.h"
#include "TimeManager.h"

#include "Looper.h"

LooperTrack::LooperTrack(Looper * looper, int _trackIdx) :
	ControllableContainer("Track " + String(_trackIdx)),
	parentLooper(looper),
	quantizedRecordStart(0),
	quantizedRecordEnd(0),
	quantizedPlayStart(0),
	quantizedPlayEnd(0),
	recordNeedle(0),
	playNeedle(0),
	streamAudioBuffer(16384),// 16000 ~ 300ms and 256*64
	monoLoopSample(1, 44100 * MAX_LOOP_LENGTH_S),
	trackState(CLEARED),
	internalTrackState(BUFFER_STOPPED),
	lastInternalTrackState(BUFFER_STOPPED),
	trackIdx(_trackIdx)
{

	//setCustomShortName("track/" + String(_trackIdx)); //can't use "/" in shortName, will use ControllableIndexedContainer for that when ready.

	selectTrig = addTrigger("Select", "Select this track");
	recPlayTrig = addTrigger("Rec Or Play", "Tells the track to wait for the next bar and then start record or play");
	playTrig = addTrigger("Play", "Tells the track to wait for the next bar and then stop recording and start playing");
	stopTrig = addTrigger("Stop", "Tells the track to stop ");
	clearTrig = addTrigger("Clear", "Tells the track to clear it's content if got any");
	volume = addFloatParameter("Volume", "Set the volume of the track", 1, 0, 1);
	mute = addBoolParameter("Mute", "Sets the track muted (or not.)", false);
	solo = addBoolParameter("Solo", "Sets the track solo (or not.)", false);
	
	preDelayMs = addIntParameter("Pre Delay MS", "Pre process delay (in milliseconds)", 40, 0, 200);

	stateParameterString = addStringParameter("state", "track state", "");
	stateParameterStringSynchronizer = new AsyncTrackStateStringSynchroizer(stateParameterString);
	addTrackListener(stateParameterStringSynchronizer);
	stateParameterString->isControllableFeedbackOnly = true;
	preDelayMs->isControllableExposed = false;


	// post init
	volume->setValue(defaultVolumeValue);
}

void LooperTrack::processBlock(AudioBuffer<float>& buffer, MidiBuffer &) {


	updatePendingLooperTrackState(TimeManager::getInstance()->timeInSample, buffer.getNumSamples());


	// RECORDING
	if (internalTrackState == BUFFER_RECORDING)
	{
		if (recordNeedle + buffer.getNumSamples()> parentLooper->getSampleRate() * MAX_LOOP_LENGTH_S) {
			setTrackState(STOPPED);
		}
		else {
			for (int i = monoLoopSample.getNumChannels() - 1; i >= 0; --i) {
				monoLoopSample.copyFrom(i, recordNeedle, buffer, i, 0, buffer.getNumSamples());
			}
			recordNeedle += buffer.getNumSamples();
		}

	}

	else {
		streamAudioBuffer.writeBlock(buffer);
	}

	// PLAYING
	// allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
	// we may need it if we start to use a different clock  than looperState in OOServer that has a granularity of blockSize
	// or if we dynamicly change blockSize
	if (internalTrackState == BUFFER_PLAYING && recordNeedle>0 && monoLoopSample.getNumSamples())
	{
		if ((playNeedle + buffer.getNumSamples()) > recordNeedle)
		{

			//assert false for now see above
			//            jassert(false);
			int firstSegmentLength = recordNeedle - playNeedle;
			int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
			for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
				int maxChannelFromRecorded = jmin(monoLoopSample.getNumChannels() - 1, i);
				buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, playNeedle, firstSegmentLength);
				buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, 0, secondSegmentLength);
			}
			playNeedle = secondSegmentLength;

		}
		else {
			for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
				int maxChannelFromRecorded = jmin(monoLoopSample.getNumChannels() - 1, i);
				buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, playNeedle, buffer.getNumSamples());
			}
			playNeedle += buffer.getNumSamples();
			playNeedle %= recordNeedle;
		}

		float newVolume = mute->boolValue() ? 0 : volume->floatValue();
		for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
			buffer.applyGainRamp(i, 0, buffer.getNumSamples(), lastVolume, newVolume);
		}

		lastVolume = newVolume;


	}
	else {
		// silence output buffer
		for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
			buffer.applyGain(i, 0, buffer.getNumSamples(), 0);
		}
	}


}
void LooperTrack::updatePendingLooperTrackState(const uint64 curTime, int _blockSize) {


	//    process changed internalState
	if (internalTrackState != lastInternalTrackState) {

		if (internalTrackState == BUFFER_RECORDING) {
			if (askForBeingMasterTempoTrack()) {
				int samplesToGet = (int)(preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
				for (int i = monoLoopSample.getNumChannels() - 1; i >= 0; --i) {
					monoLoopSample.copyFrom(i, 0, streamAudioBuffer.getLastBlock(samplesToGet, i), samplesToGet);
				}
				recordNeedle = samplesToGet;
			}

			else {
				recordNeedle = 0;
			}
		}

		if (lastInternalTrackState == BUFFER_RECORDING) {
			if (askForBeingMasterTempoTrack()) {
				recordNeedle -= (int)(preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());

				const int fadeNumSamples = (int)(parentLooper->getSampleRate()*0.022f);
				if (fadeNumSamples>0 && recordNeedle>2 * fadeNumSamples) {
					monoLoopSample.applyGainRamp(0, 0, fadeNumSamples, 0, 1);
					monoLoopSample.applyGainRamp(0, recordNeedle - fadeNumSamples, fadeNumSamples, 1, 0);
				}
				TimeManager::getInstance()->setBPMForLoopLength(recordNeedle);


			}
			playNeedle = 0;
		}
	}

	// TODO subBlock precision ?
	// not sure -> triggers are updated at block size granularity

	// for now reduce block approximation noise when quantized
	const uint64 triggeringTime = curTime + _blockSize / 2;
	if (quantizedRecordStart>0) {
		if (triggeringTime >= quantizedRecordStart) {
			setTrackState(RECORDING);
		}

	}
	else if (quantizedRecordEnd>0) {
		if (triggeringTime >= quantizedRecordEnd) {
			setTrackState(PLAYING);

		}
	}



	if (quantizedPlayStart>0) {
		if (triggeringTime >= quantizedPlayStart) {
			setTrackState(PLAYING);
		}
	}
	else if (quantizedPlayEnd>0) {
		if (triggeringTime >= quantizedPlayEnd) {
			setTrackState(STOPPED);
		}
	}

	lastInternalTrackState = internalTrackState;
}


String LooperTrack::trackStateToString(const TrackState & ts) {

	switch (ts) {
	case SHOULD_CLEAR:
	case CLEARED:
		return "empty";
	case PLAYING:
		return "playing";
	case RECORDING:
		return "recording";
	case SHOULD_PLAY:
		return "willPlay";
	case SHOULD_RECORD:
		return "willRecord";
	case STOPPED:
		return "stopped";

	default:
		jassertfalse;
		break;
	}

	return "[noState]";
}

void LooperTrack::onContainerParameterChanged(Parameter * p)
{
	if (p == volume)
	{
		if (parentLooper->selectedTrack == this) parentLooper->volumeSelected->setValue(volume->floatValue());
	}
}

void LooperTrack::onContainerTriggerTriggered(Trigger * t) {
	if (t == selectTrig)
	{
		parentLooper->selectMe(this);

	}
	else if (t == recPlayTrig) {

		if (trackState == CLEARED) {
			setTrackState(SHOULD_RECORD);
		}
		else {
			setTrackState(SHOULD_PLAY);
		}

	}
	else if (t == playTrig) {
		setTrackState(SHOULD_PLAY);
	}
	else if (t == clearTrig) {
		setTrackState(SHOULD_CLEAR);
	}
	else if (t == stopTrig) {
		setTrackState(STOPPED);
	}
}

bool LooperTrack::askForBeingMasterTempoTrack() {
	return TimeManager::getInstance()->askForBeingMasterNode(parentLooper->looperNode)
		&& parentLooper->askForBeingMasterTrack(this);
}




void LooperTrack::setSelected(bool isSelected) {
	trackStateListeners.call(&LooperTrack::Listener::trackSelected, isSelected);
}


void LooperTrack::askForSelection(bool) {
	selectTrig->trigger();
}


void LooperTrack::setTrackState(TrackState newState) {



	// quantify
	if (newState == SHOULD_RECORD) {
		// are we able to set the tempo
		if (askForBeingMasterTempoTrack()) {
			TimeManager::getInstance()->stop();
			TimeManager::getInstance()->setPlayState(true, true);
			quantizedRecordStart = -1;
			setTrackState(RECORDING);
			return;
		}
		else if (!TimeManager::getInstance()->getIsSettingTempo()) {
			quantizedRecordStart = TimeManager::getInstance()->getNextQuantifiedTime();
		}
		//            RecordPer default if triggering other rec while we are current master and we record recording
		else if (NodeBase * originMasterNode = TimeManager::getInstance()->timeMasterNode) {
			if ((LooperNode*)originMasterNode == parentLooper->looperNode) {
				newState = RECORDING;
				TimeManager::getInstance()->setPlayState(true, false);
				parentLooper->lastMasterTempoTrack->setTrackState(LooperTrack::TrackState::PLAYING);
			}
		}
	}

	// on true start recording
	if (newState == RECORDING) {
		internalTrackState = BUFFER_RECORDING;
		quantizedRecordStart = -1;

	}
	// on true end recording
	else if ((trackState == RECORDING || trackState == STOPPED) && newState == SHOULD_PLAY) {
		{

			if ((askForBeingMasterTempoTrack() && trackState == RECORDING) ||
				(parentLooper->askForBeingAbleToPlayNow(this) && trackState == STOPPED)) {
				quantizedRecordEnd = -1;
				newState = PLAYING;
				TimeManager::getInstance()->stop();
				TimeManager::getInstance()->setPlayState(true, false);
			}
			else {
				quantizedRecordEnd = TimeManager::getInstance()->getNextQuantifiedTime();
			}
		}
	}

	// on ask for play
	if (newState == SHOULD_PLAY) {
		cleanAllQuantizeNeedles();
		quantizedPlayStart = TimeManager::getInstance()->getNextQuantifiedTime();
	}
	// on true start of play
	else if (newState == PLAYING) {
		internalTrackState = BUFFER_PLAYING;
		cleanAllQuantizeNeedles();
		playNeedle = 0;

	}
	// on true end of play
	else if (trackState == PLAYING && newState != PLAYING) {
		quantizedPlayEnd = -1;
	}
	// on should clear
	if (newState == SHOULD_CLEAR) {
		recordNeedle = 0;
		playNeedle = 0;
		cleanAllQuantizeNeedles();
		volume->setValue(defaultVolumeValue);
		newState = CLEARED;
		internalTrackState = BUFFER_STOPPED;


	}


	if (newState == STOPPED) {
		internalTrackState = BUFFER_STOPPED;
		cleanAllQuantizeNeedles();
		// force a track to stay in cleared state if stop triggered
		if (trackState == CLEARED) { newState = CLEARED; }
	}
	//DBG(newState <<","<<trackState );

	trackState = newState;

	parentLooper->checkIfNeedGlobalLooperStateUpdate();
	trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
};

void LooperTrack::cleanAllQuantizeNeedles() {
	quantizedPlayEnd = -1;
	quantizedPlayStart = -1;
	quantizedRecordEnd = -1;
	quantizedRecordStart = -1;
}


Component * LooperTrack::createControllableContainerEditor() {
	return nullptr;
}