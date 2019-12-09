#ifndef __synthOscillator_h__
#define __synthOscillator_h__

// --- we need these
#include "synthdefs.h"
#include "wavetableoscillator.h"
#include "resonator.h"
#include "dca_eg.h"

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
	
	// --- **7**
	virtual std::vector<std::string> getWaveformNames(uint32_t bankIndex);
	virtual std::vector<std::string> getBankNames();
	virtual void setBankSet(uint32_t _bankSet);
	virtual uint32_t getBankSet();

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

	// --- smart pointers to the oscillator object
	std::unique_ptr<WaveTableOsc> wavetableOscillator = nullptr;

	// --- resonator tube
	std::unique_ptr<Resonator> resonator;
	
	// --- for the noise EG
	std::shared_ptr<EGParameters> noiseEGParameters = std::make_shared<EGParameters>();

	double midiNotePitch = 0.0;

	// --- resonator tube
	std::unique_ptr<EnvelopeGenerator> noiseEG;

	// --- 32-bit register for RS&H
	uint32_t pnRegister = 0;			///< 32 bit register for PN oscillator
};









#endif /* defined(__synthOscillator_h__) */
