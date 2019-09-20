#ifndef __synthOscillator_h__
#define __synthOscillator_h__

// --- we need these
#include "synthdefs.h"
#include "wavetableoscillator.h"

/**
\class SynthOsc
\ingroup SynthClasses
\brief Encapsulates one synth oscillator

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/

class SynthOsc : public ISynthOscillator
{
public:
	SynthOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
			 std::shared_ptr<SynthOscParameters> _parameters, 
			 std::shared_ptr<WaveTableData> _waveTableData);
	virtual ~SynthOsc();

	// --- ISynthOscillator
	virtual bool reset(double _sampleRate);
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);

	// --- could also place glide mod on this object and share it?
	virtual bool setGlideModulation(uint32_t _startMIDINote, uint32_t _endMIDINote, double glideTime_mSec)
	{
		wavetableOscillator->setGlideModulation(_startMIDINote, _endMIDINote, glideTime_mSec);

		return true;
	}

	// --- our render function
	const OscillatorOutputData renderAudioOutput();
	
	// --- get our modulators
	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}

	// --- not used here;
	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

protected:
	// --- MIDI Data Interface
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- modulators
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<SynthOscParameters> parameters = std::make_shared<SynthOscParameters>();

	// --- parameters
	std::shared_ptr<WaveTableData> waveTableData = nullptr;

	// --- our output
	OscillatorOutputData oscillatorAudioData;

	// --- smart pointers to the oscillator objects
	std::unique_ptr<WaveTableOsc> wavetableOscillator = nullptr;

	// --- add more oscillators here

};









#endif /* defined(__synthOscillator_h__) */
