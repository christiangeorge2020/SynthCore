#pragma once

#include "synthdefs.h"
#include "guiconstants.h"


/**
\enum LFOWaveform
\ingroup SynthStructures
\brief Enumeration for selecting LFO waveform 
 - Triangle
 - Sin
 - Saw
 - Random Sample and Hold
 - Quasi-Random Sample and Hold
 - Noise
 - Quasi-Random Noise
*/
enum class LFOWaveform { kTriangle, kSin, kSaw, kRSH, kQRSH, kNoise, kQRNoise };

/**
\enum LFOMode
\ingroup SynthStructures
\brief Enumeration for selecting the LFO mode
- kSync: LFO restarts with each new note on
- kOneShot: LFO runs once
- kFreeRun: LFO begins running upon initialization, does not reset with a new note on
*/
enum class LFOMode { kSync, kOneShot, kFreeRun };

/**
\enum LFOOutput
\ingroup SynthDefs
\brief Enumerations determing the output swing
*/
enum {
	kLFONormalOutput,			// 0
	kLFONormalOutputInverted,	// 1 etc...
	kLFOQuadPhaseOutput,
	kLFOQuadPhaseOutputInverted,
	kUnipolarOutputFromMax,		/* this mimics an INVERTED EG going from MAX -> MAX */
	kUnipolarOutputFromMin		/* this mimics an EG going from 0.0 -> MAX */
};

/**
\struct SynthLFOParameters
\ingroup SynthStructures
\brief LFO Parameter Structure for Recieving GUI and User Data
*/
struct SynthLFOParameters
{
	SynthLFOParameters() {}
	SynthLFOParameters& operator=(const SynthLFOParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		waveform = params.waveform;
		mode = params.mode;

		frequency_Hz = params.frequency_Hz;
		outputAmplitude = params.outputAmplitude;

		lfoDelay_mSec = params.lfoDelay_mSec;
		lfoRamp_mSec = params.lfoRamp_mSec;

		lfoShape = params.lfoShape;
		shapeSplitpoint = params.shapeSplitpoint;

		return *this;
	}

	// --- individual parameters
	LFOWaveform waveform = LFOWaveform::kTriangle;
	LFOMode mode = LFOMode::kSync;

	double frequency_Hz = 0.0;
	double outputAmplitude = 1.0;
	double lfoDelay_mSec = 0.0;
	double lfoRamp_mSec = 0.0;
	double lfoShape = 0.5;
	double shapeSplitpoint = 0.5;
};


/**
\class SynthLFO
\ingroup SynthClasses
\brief Synth LFO
*/
class SynthLFO : public ISynthModulator//ISynthOscillator
{
public:
	SynthLFO(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<SynthLFOParameters> _parameters)
		: midiInputData(_midiInputData) 
	, parameters(_parameters){
		srand(time(NULL)); // --- seed random number generator

		// --- randomize the PN register
		pnRegister = rand();

	}	/* C-TOR */
	virtual ~SynthLFO() {}				/* D-TOR */

	// --- ISynthOscillator
	virtual bool reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		phaseInc = parameters->frequency_Hz / sampleRate;

		// --- Delay and Ramp Reset
		delayTimer.resetTimer();
		rampTimer.resetTimer();


		// --- timebase variables
		modCounter = 0.0;			///< Modulo counter [0.0, +1.0]
		modCounterQP = 0.25;		///< Quad Phase modulo counter [0.0, +1.0]

		return true;
	}

	// --- ISynthModulator cont'd
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity) 
	{ 
		renderComplete = false;

		/// Delay and Ramp Timers activate upon NoteOn Message
		int delay_Samples = msecToSamples(sampleRate, parameters->lfoDelay_mSec); ///< Convert mseconds to sample
		delayTimer.resetTimer(); ///< Reset
		delayTimer.setTargetValueInSamples(delay_Samples); ///< Set target delay

		int ramp_Samples = msecToSamples(sampleRate, parameters->lfoRamp_mSec);
		rampTimer.resetTimer();
		rampTimer.setTargetValueInSamples(ramp_Samples);

		

		if (parameters->mode == LFOMode::kSync || parameters->mode == LFOMode::kOneShot)
		{
			modCounter = 0.0;			///< modulo counter [0.0, +1.0]
			modCounterQP = 0.25;		///< Quad Phase modulo counter [0.0, +1.0]
		}
	
		randomSHCounter = -1; // -1 = reset
		return true; 
	}

	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
	{
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
	std::shared_ptr<SynthLFOParameters> parameters = nullptr;

	// --- sample rate
	double sampleRate = 0.0;			///< sample rate

	Timer delayTimer;			///< timer object for setting lfo starting delay
	Timer rampTimer;			///< timer object for lfo ramp onset time
	double rampIncrement;

	double lfoShape_first = 0.5;
	double lfoShape_second = 0.5;
	
	// --- timebase variables
	double modCounter = 0.0;			///< modulo counter [0.0, +1.0]
	double phaseInc = 0.0;				///< phase inc = fo/fs
	double modCounterQP = 0.25;			///< Quad Phase modulo counter [0.0, +1.0]
	bool renderComplete = false;		///< flag for one-shot

	// --- 32-bit register for RS&H
	uint32_t pnRegister = 0;			///< 32 bit register for PN oscillator
	int randomSHCounter = -1;			///< random sample/hold counter;  -1 is reset condition
	double randomSHValue = 0.0;			///< current output, needed because we hold this output for some number of samples = (sampleRate / oscFrequency)

	/**
	\struct checkAndWrapModulo
	\brief Check a modulo counter and wrap it if necessary
	*/
	inline bool checkAndWrapModulo(double& moduloCounter, double phaseInc)
	{
		// --- for positive frequencies
		if (phaseInc > 0 && moduloCounter >= 1.0)
		{
			moduloCounter -= 1.0;
			return true;
		}

		// --- for negative frequencies
		if (phaseInc < 0 && moduloCounter <= 0.0)
		{
			moduloCounter += 1.0;
			return true;
		}

		return false;
	}

	/**
	\struct checkAndWrapModulo
	\brief Advance, and then check a modulo counter and wrap it if necessary
	*/
	inline bool advanceAndCheckWrapModulo(double& moduloCounter, double phaseInc)
	{
		// --- advance counter
		moduloCounter += phaseInc;

		// --- for positive frequencies
		if (phaseInc > 0 && moduloCounter >= 1.0)
		{
			moduloCounter -= 1.0;
			return true;
		}

		// --- for negative frequencies
		if (phaseInc < 0 && moduloCounter <= 0.0)
		{
			moduloCounter += 1.0;
			return true;
		}

		return false;
	}

	// --- increment the modulo counter
	inline void advanceModulo(double& moduloCounter, double phaseInc) { moduloCounter += phaseInc; }

	// --- sine approximation with parabolas
	const double B = 4.0 / kPi;
	const double C = -4.0 / (kPi* kPi);
	const double P = 0.225;
	// http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
	//
	// --- angle is -pi to +pi
	inline double parabolicSine(double angle)
	{
		double y = B * angle + C * angle * fabs(angle);
		y = P * (y * fabs(y) - y) + y;
		return y;
	}

	inline double msecToSamples(double sampleRate, double timeMSec)
	{
		return sampleRate * (timeMSec / 1000.0);;
	}
};

