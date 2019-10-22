#ifndef __wavetableOscillator_h__
#define __wavetableOscillator_h__

// --- includes
#include "synthdefs.h"

// --- wavetable objects and structs
#include "wavetablebank.h"
#include "wavetabledata.h"

/**
\class WaveTableOsc
\ingroup SynthClasses
\brief High_Accuracy wavetable oscillator with three waveforms: AK, FM and VS

\author Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 24
*/
class WaveTableOsc : public ISynthOscillator
{
public:
	WaveTableOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
				 std::shared_ptr<SynthOscParameters> _parameters, 
				 std::shared_ptr<WaveTableData> _waveTableData);

	virtual ~WaveTableOsc();

	// --- ISynthOscillator
	virtual std::vector<std::string> getWaveformNames(uint32_t bankIndex);
	virtual bool reset(double _sampleRate);
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);

	virtual std::vector<std::string> getBankNames() { return waveTableData->getWaveBankNames(bankSet); }
	virtual void setBankSet(uint32_t _bankSet) { bankSet = _bankSet; }
	virtual uint32_t getBankSet() { return bankSet; }

	virtual bool setGlideModulation(uint32_t _startMIDINote, uint32_t _endMIDINote, double glideTime_mSec)
	{
		// --- start up the glide modulator
		return glideModulator.start(_startMIDINote, _endMIDINote, glideTime_mSec, sampleRate);
	}

	// --- main render function
	const OscillatorOutputData renderAudioOutput();

	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}

	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

protected:
	// --- MIDI Data Interface
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- modulators
	GlideModulator glideModulator;
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<SynthOscParameters> parameters = nullptr;

	// --- shared tables, in case they are huge or need a long creation time
	std::shared_ptr<WaveTableData> waveTableData = nullptr;

	// --- currently selected wavetable
	IWaveBank* selectedWaveBank = nullptr;

	IWaveTable* selectedWaveTable = nullptr;

	// --- for anything
	double readWaveTable(double& readIndex, double _phaseInc); 

	// --- the FINAL frequncy after all modulations
	double oscillatorFrequency = 440.0;
	double oscillatorFrequencySlaveOsc = 440.0;

	// --- the note number of the playing note
	uint32_t midiNoteNumber = 0;

	// --- midi note closest to FINAL pitch, but not over
	uint32_t renderMidiNoteNumber = 0;

	// --- the midi pitch, will need to save for portamento
	double midiNotePitch = 0.0;				///<the midi pitch, will need to save for portamento

	// --- our output
	OscillatorOutputData oscillatorAudioData;

	// --- timebase variables
	double modCounter = 0.0;						///<  VA modulo counter 0 to 1.0
	double phaseInc = 0.0;							///<  phase inc = fo/fs
	double sampleRate = 0.0;						///<  fs
	
	// --- WaveRable oscillator variables
	double waveTableReadIndex = 0.0;		///< wavetable read location
	uint32_t currentTableLength = kDefaultWaveTableLength;

	// --- flag indicating state (running or not)
	bool noteOn = false;

	uint32_t bankSet = BANK_SET_0;

};


#endif /* defined(__wavetableOscillator_h__) */
