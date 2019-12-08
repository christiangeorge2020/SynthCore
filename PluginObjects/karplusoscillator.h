#ifndef __karplusOscillator_h__
#define __karplusOscillator_h__

// --- includes
#include "synthdefs.h"
#include "resonator.h"
#include "dca_eg.h"

// --- ALL oscillator waveforms
enum class NoiseType { kWhite, kPN };

/**
\struct SynthOscParameters
\ingroup SynthStructures
\brief Contains voice-level modifiers and component modifer pointers that are shared across all voices. A "modifier" is any variable that *may* be connected to a
GUI control, however modifiers are not required to be connected to anything and their default values are set in the structure.
*A nodifier may also be used by an outer container object to modify parameters externally!*
*/
struct KarplusOscParameters
{
	KarplusOscParameters() {}

	KarplusOscParameters& operator=(const KarplusOscParameters& params)
	{
		if (this == &params)
			return *this;

		resonatorPitchShiftSemitones = params.resonatorPitchShiftSemitones;
		resonatorDecay = params.resonatorDecay;

		detuneOctaves = params.detuneOctaves;
		detuneSemitones = params.detuneSemitones;
		detuneCents = params.detuneCents;
		unisonDetuneCents = params.unisonDetuneCents;
		outputAmplitude = params.outputAmplitude;

		noiseType = params.noiseType;
		noiseEGParameters = params.noiseEGParameters;

		return *this;
	}

	// --- resonator specific
	double resonatorPitchShiftSemitones = 0.0;
	double resonatorDecay = 1.0;

	// ---  detuning values
	double detuneOctaves = 0.0;			// 1 = up one octave, -1 = down one octave
	double detuneSemitones = 0.0;		// 1 = up one half-step, -1 = down one half-step

	// --- this is most common for "fat" sounds; it can also be forced during "unison" mode
	double detuneCents = 0.0;			// 1 = up one cent, -1 = down one cent

	// --- this is a root-detuner for unison mode (could combine with detuneCents but would become tangled)
	double unisonDetuneCents = 0.0;		// 1 = up one cent, -1 = down one cent
	double outputAmplitude = 1.0;		// raw value, NOT dB

	// --- exciter components
	NoiseType noiseType = NoiseType::kWhite;

	// --- for the noise EG
	std::shared_ptr<EGParameters> noiseEGParameters = std::make_shared<EGParameters>();
};

/**
\class KarplusOsc
\ingroup SynthClasses
\brief Encapsulates one wavetable oscillator.

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
class KarplusOsc : public ISynthOscillator
{
public:
	KarplusOsc(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<SynthOscParameters> _parameters);
	virtual ~KarplusOsc();

	// --- ISynthOscillator
	virtual bool reset(double _sampleRate);
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool setGlideModulation(uint32_t _startMIDINote, uint32_t _endMIDINote, double glideTime_mSec)
	{
		// --- start up the glide modulator
		return glideModulator.start(_startMIDINote, _endMIDINote, glideTime_mSec, sampleRate);
	}

	virtual std::vector<std::string> getWaveformNames(uint32_t bankIndex)
	{
		std::vector<std::string> emptyVector;
		emptyVector.clear();
		return emptyVector;
	}

	virtual std::vector<std::string> getBankNames()
	{
		std::vector<std::string> emptyVector;
		emptyVector.clear();
		return emptyVector;
	}

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
	std::shared_ptr<KarplusOscParameters> parameters = nullptr;

	// --- the FINAL frequncy after all modulations
	double oscillatorFrequency = 440.0;

	// --- the note number of the playing note
	uint32_t midiNoteNumber = 0;

	// --- the midi pitch, will need to save for portamento
	double midiNotePitch = 0.0;				///<the midi pitch, will need to save for portamento

	// --- our output
	OscillatorOutputData oscillatorAudioData;

	// --- timebase variables
	double sampleRate = 0.0;				///<  fs

	// --- flag indicating state (running or not)
	bool noteOn = false;

	// --- resonator tube
	std::unique_ptr<Resonator> resonator;

	// --- resonator tube
	std::unique_ptr<EnvelopeGenerator> noiseEG;

	// --- 32-bit register for RS&H
	uint32_t pnRegister = 0;			///< 32 bit register for PN oscillator
};


#endif /* defined(__synthOscillator_h__) */
