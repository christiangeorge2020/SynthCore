#pragma once

#include "synthdefs.h"
#include "synthlfo.h"
#include "guiconstants.h"

/**
\struct RotorParameters
\ingroup SynthStructures
\brief LFO Parameter Structure for Recieving GUI and User Data
*/
struct RotorParameters
{
	RotorParameters() {}
	RotorParameters& operator=(const RotorParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		waveform_X = params.waveform_X;
		mode_X = params.mode_X;

		waveform_Y = params.waveform_Y;
		mode_Y = params.mode_Y;

		frequency_Hz_X = params.frequency_Hz_X;
		outputAmplitude_X = params.outputAmplitude_X;

		frequency_Hz_Y = params.frequency_Hz_Y;
		outputAmplitude_Y = params.outputAmplitude_Y;



		return *this;
	}

	// --- individual parameters
	LFOWaveform waveform_X = LFOWaveform::kTriangle;
	LFOMode mode_X = LFOMode::kSync;

	LFOWaveform waveform_Y = LFOWaveform::kTriangle;
	LFOMode mode_Y = LFOMode::kSync;

	double frequency_Hz_X = 0.0;
	double outputAmplitude_X = 1.0;

	double frequency_Hz_Y = 0.0;
	double outputAmplitude_Y = 1.0;

	std::shared_ptr<SynthLFOParameters> lfoXParameters = std::make_shared<SynthLFOParameters>();
	std::shared_ptr<SynthLFOParameters> lfoYParameters = std::make_shared<SynthLFOParameters>();

};

class Rotor : public ISynthModulator
{
	Rotor(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<RotorParameters> _parameters, std::unique_ptr<SynthLFO> _lfoX, std::unique_ptr<SynthLFO> _lfoY)
		: midiInputData(_midiInputData)
		, parameters(_parameters)
		{
	
		lfoX.reset(new SynthLFO(midiInputData, parameters->lfoXParameters));
		lfoY.reset(new SynthLFO(midiInputData, parameters->lfoYParameters));
	} /* C-TOR */
	
	virtual ~Rotor() {}				/* D-TOR */

	// --- ISynthOscillator
	virtual bool reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		
		lfoX->reset(sampleRate);
		lfoY->reset(sampleRate);


		return true;
	}

	virtual bool update(bool updateAllModRoutings = true);

	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
	{
		lfoX->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
		lfoY->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);

		return true;
	}

	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
	{
		lfoX->doNoteOff(midiPitch, _midiNoteNumber, midiNoteVelocity);
		lfoY->doNoteOff(midiPitch, _midiNoteNumber, midiNoteVelocity);
		return true;
	}

	// --- the oscillator function
	const ModOutputData renderModulatorOutput();

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
	std::shared_ptr<RotorParameters> parameters = nullptr;

	// --- sample rate
	double sampleRate = 0.0;			///< sample rate


	std::unique_ptr<SynthLFO> lfoX;
	std::unique_ptr<SynthLFO> lfoY;
};