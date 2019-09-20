#pragma once

#include "synthdefs.h"
#include "guiconstants.h"

// --- parameters come mostly fro GUI controls (modulators come from other objects)
struct DCAParameters
{
	DCAParameters() {}
	DCAParameters& operator=(const DCAParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		gain_dB = params.gain_dB;
		mute = params.mute; 
		panValue = params.panValue;

		return *this;
	}

	// --- individual parameters
	double gain_dB = 0.0;		// --- for per-voice gain control (not same as master MIDI volume)
	bool mute = false;
	double panValue = 0.0;		// --- [-1, +1] --> [left -> right]
};

/**
	\class DCA
	\ingroup SynthClasses
	\brief Encapsulates a digitally controlled amplifier
	\author Will Pirkle
	\version Revision : 1.0
	\date Date : 2017 / 09 / 24
*/
class DCA : public ISynthProcessor
{
public:
	// --- Dependency Injection forces relationship
	DCA(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<DCAParameters> _parameters) :
		midiInputData(_midiInputData)
		, parameters(_parameters)
	{
		if(!parameters)
			parameters = std::make_shared<DCAParameters>();
	}

	virtual ~DCA() {}

	// --- ISynthProcessor
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);

	// --- set sample rate, then update coeffs
	virtual bool reset(double _sampleRate)
	{
		gainRaw = 1.0;			// --- unity
		panLeftGain = 0.707;	// --- center
		panRightGain = 0.707;	// --- center

		return true;
	}

	// --- we can do multi-channel but need a different manner for passing data
	virtual bool canProcessAudioFrame() { return false; }

	// --- process audio: run the filter
	virtual double processAudioSample(double xn) { return xn; }

	// --- note pass by pointer to avoid copying data
	bool processSynthAudio(SynthProcessorData* audioData)
	{
		// --- make sure we have input and outputs
		if (audioData->numInputChannels == 0 || audioData->numOutputChannels == 0)
			return false;

		// --- if MONO, no panning is applied to left channel
		if (audioData->numInputChannels == 1 && audioData->numOutputChannels == 1)
			audioData->outputs[0] = audioData->inputs[0] * gainRaw;
		else if (audioData->numOutputChannels > 1)
			// --- stereo, add left pan value
			audioData->outputs[0] = audioData->inputs[0] * gainRaw * panLeftGain;

		// --- now process right channel
		// --- monot -> stereo: copy left channel to right and apply panning gain
		if (audioData->numInputChannels == 1 && audioData->numOutputChannels == 2)
			audioData->outputs[1] = audioData->inputs[0] * gainRaw  * panRightGain;

		// --- stereo to stereo
		else if (audioData->numInputChannels == 2 && audioData->numOutputChannels == 2)
			audioData->outputs[1] = audioData->inputs[1] * gainRaw  * panRightGain;

		return true;
	}

	// --- access to modulators
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
	const const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- we share Parameters with other voice's same-components
	std::shared_ptr<DCAParameters> parameters = nullptr;

	// --- set of double[MAX_MODULATION_CHANNELS]
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	double gainRaw = 1.0;			///< the final raw gain value
	double panLeftGain = 0.707;		///< left channel gain
	double panRightGain = 0.707;	///< right channel gain
	double midiVelocityGain = 0.0;

	// --- pan value is set internally by voice, or via MIDI/MIDI Channel
	double panValue = 0.0;			///< pan value is set internally by voice, or via MIDI/MIDI Channel

	// --- note on flag
	bool noteOn = false;
};

// ----------------- ENVELOPE GENERATOR ----------------------------------- //
// --- outputs[] indexes for this component
enum {
	kEGNormalOutput,
	kEGBiasedOutput,
	kNumEGOutputs
}; // normal, biased, number of outputs (last)


// --- strongly typed enum for trivial oscillator type & mode
enum class egTCMode { kAnalog, kDigital };
enum class egState { kOff, kDelay, kAttack, kDecay, kSustain, kRelease, kShutdown, kHoldOn, kHoldOff };
enum class egType { kADSR, kAHDSR, kAHR };

/**
\struct EGParameters
\ingroup SynthStructures
\brief 

*/
struct EGParameters
{
	EGParameters() {}
	EGParameters& operator=(const EGParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		// --- bulk memory block copy (fast)
		egMode = params.egMode;
		egContourType = params.egContourType;

		resetToZero = params.resetToZero;
		legatoMode = params.legatoMode;
		velocityToAttackScaling = params.velocityToAttackScaling;
		noteNumberToDecayScaling = params.noteNumberToDecayScaling;

		attackTime_mSec = params.attackTime_mSec;
		decayTime_mSec = params.decayTime_mSec;
		releaseTime_mSec = params.releaseTime_mSec;
		sustainLevel = params.sustainLevel;
		holdTime_mSec = params.holdTime_mSec;
		offTime_mSec = params.offTime_mSec;
		delayTime_mSec = params.delayTime_mSec;

		return *this;
	}

	egTCMode egMode = egTCMode::kAnalog;
	egType egContourType = egType::kADSR;

	bool resetToZero = false;
	bool legatoMode = false;
	bool velocityToAttackScaling = false;
	bool noteNumberToDecayScaling = false;

	//--- DADSR times from user
	double attackTime_mSec = 1.0;	// att: is a time duration
	double decayTime_mSec = 1.0;	// dcy: is a time to decay 1->0
	double releaseTime_mSec = 1.0;	// rel: is a time to decay 1->0
	double sustainLevel = 1.0;
	double holdTime_mSec = 0.0;
	double offTime_mSec = 0.0;
	double delayTime_mSec = 0.0;
};

/**
\class EnvelopeGenerator
\ingroup SynthClasses
\brief Encapsulates a virtual analog ADSR envelope generator with adjustable segment curvature

Outputs: contains 2 outputs
- Normal Output
- Biased Output (for using the EG to control pitch -- this way the note will hit its MIDI pitch during the sustain segment

*/
class EnvelopeGenerator : public ISynthModulator
{
public:
	EnvelopeGenerator(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<EGParameters> _parameters) :
		midiInputData(_midiInputData)
	, parameters(_parameters)
	{
		if (!parameters)
			parameters = std::make_shared<EGParameters>();

		setEGMode(egTCMode::kAnalog); // default operation - this is what I always use
	}
	virtual ~EnvelopeGenerator() {}

	// --- ISynthModulator
	virtual bool reset(double _sampleRate)
	{
		// --- wave tables are fs based
		bool bNewSR = _sampleRate != sampleRate ? true : false;

		// --- retain sample rate for update calculations
		sampleRate = _sampleRate;

		// --- recrate the tables only if sample rate has changed
		if (bNewSR)
		{
			// --- recalc these, SR dependent
			calculateAttackTime(parameters->attackTime_mSec);
			calculateDecayTime(parameters->decayTime_mSec);
			calculateReleaseTime(parameters->releaseTime_mSec);
		}

		// --- reset the state
		envelopeOutput = 0.0;
		state = egState::kOff;
		holdTimer.resetTimer();
		offTimer.resetTimer();

		return true;
	}

	virtual bool update(bool updateAllModRoutings = true);

	// --- note event handlers
	virtual bool doNoteOn(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity);

	virtual const ModOutputData renderModulatorOutput();

	// --- accessors - allow owner to get our state
	egState getState() { return state; }			///< returns current state of the EG finite state machine

	// --- output EG identifer access
	bool isOutputEG() { return outputEG; }			///< returns true if this EG is connected to the output DCA
	void setIsOutputEG(bool b) { outputEG = b; }	///< set/clear the output DCA flag
	bool isRTZ_EG() { return resetToZero; }
	void setEGMode(egTCMode mode);

	// --- function to shutdown the EG
	bool shutdown();
	bool restart();

	// --- get/set for modulators
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
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<EGParameters> parameters = nullptr;

	// --- these are local copies
	//     changes to the sustain level affect the calculations of decay and release (expensive)
	double sustainLevel = 1.0;
	double attackTime_mSec = 1.0;	///< att: is a time duration
	double decayTime_mSec = 1.0;	///< dcy: is a time to decay from max output to 0.0
	double releaseTime_mSec = 1.0;	///< rel: is a time to decay from max output to 0.0
	
	Timer holdTimer;
	double holdTime_mSec = 0.0;

	Timer delayTimer;
	double delayTime_mSec = 0.0;

	Timer offTimer;
	double offTime_mSec = 0.0;

	// --- calculate time params
	void calculateAttackTime(double attackTime, double attackTimeScalar = 1.0);
	void calculateDecayTime(double decayTime, double decayTimeScalar = 1.0);
	void calculateReleaseTime(double releaseTime, double releaseTimeScalar = 1.0);

	/// set the sustain pedal override to keep the EG stuck in the sustain state until the pedal is released
	void setSustainOverride(bool b)
	{
		sustainOverride = b;

		if (releasePending && !sustainOverride)
		{
			releasePending = false;
			doNoteOff(0.0, 0, 0);
		}
	}

	// --- sample rate for time calculations
	double sampleRate = 0.0;			///< sample rate

	// --- the current output of the EG
	double envelopeOutput = 0.0;		///< the current envelope output sample

	/** true if this EG is connected to the output DCA, or more generally
	//     if the note-off state of the voice depends on this EG expiring */
	bool outputEG = false;

	//--- Coefficient, offset and TCO values
	//    for each state
	double attackCoeff = 0.0;
	double attackOffset = 0.0;
	double attackTCO = 0.0;

	double decayCoeff = 0.0;
	double decayOffset = 0.0;
	double decayTCO = 0.0;

	double releaseCoeff = 0.0;
	double releaseOffset = 0.0;
	double releaseTCO = 0.0;

	// --- this is set internally; user normally not allowed to adjust
	double shutdownTime_mSec = 10.0;	///< short shutdown time when stealing a voice

	// --- for sustain pedal input; these two bools keep track of the override, 
	//     and eventual release when the sus pedal is released after user released key
	bool sustainOverride = false;		///< if true, places the EG into sustain mode
	bool releasePending = false;		///< a flag set when a note off event occurs while the sustain pedal is held, telling the EG to go to the release state once the pedal is released
	bool resetToZero = false;			///< notes the EG is in reset-to-zero mode

	// --- inc value for shutdown
	double incShutdown = 0.0;			///< shutdown linear incrementer

	// --- stage variable
	egState state = egState::kOff;		///< EG state variable

	// --- analog/digital mode
	egTCMode egMode = egTCMode::kAnalog; ///< analog or digital (linear in dB)

	// --- RUN/STOP flag
	bool noteOn = false;

};
