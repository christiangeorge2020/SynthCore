#pragma once

#include "synthdefs.h"
#include "guiconstants.h"

enum class WindowType { kHann, kHamming };
enum class WindowEGState { kOff, kWindowing, kHoldOff };
// --- outputs[] indexes for this component
enum {
	kWindowEGNormalOutput,
	kNumWindowEGOutputs
}; 

/**
\struct WindowEGParameters
\ingroup SynthStructures
\brief

*/
struct WindowEGParameters
{
	WindowEGParameters() {}
	WindowEGParameters& operator=(const WindowEGParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		// --- bulk memory block copy (fast)
		windowType = params.windowType;
		offTime_mSec = params.offTime_mSec;
		windowWidth_mSec = params.windowWidth_mSec;
		windowWidth_Samples = params.windowWidth_Samples;
		widthInSamples = params.widthInSamples;

		return *this;
	}

	WindowType windowType = WindowType::kHann;
	double offTime_mSec = 0.0;
	double windowWidth_mSec = 0.0;
	uint32_t windowWidth_Samples = 0.0;
	bool widthInSamples = true;
};

/**
\class WindowEG
\ingroup SynthClasses
\brief Encapsulates a virtual analog ADSR envelope generator with adjustable segment curvature

Outputs: contains 2 outputs
- Normal Output
- Biased Output (for using the EG to control pitch -- this way the note will hit its MIDI pitch during the sustain segment

*/
class WindowEG : public ISynthModulator
{
public:
	WindowEG(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<WindowEGParameters> _parameters) :
		midiInputData(_midiInputData)
	, parameters(_parameters)
	{
		if (!parameters)
			parameters = std::make_shared<WindowEGParameters>();
	}
	virtual ~WindowEG() {}

	// --- ISynthModulator
	virtual bool reset(double _sampleRate)
	{
		// --- retain sample rate for update calculations
		sampleRate = _sampleRate;

		// --- reset the state
		envelopeOutput = 0.0;
		state = WindowEGState::kOff;
		holdTimer.resetTimer();
		offTimer.resetTimer();
		windowTimer.resetTimer();

		return true;
	}

	virtual bool update(bool updateAllModRoutings = true);

	// --- note event handlers
	virtual bool doNoteOn(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity);

	virtual const ModOutputData renderModulatorOutput();

	void restart();

	// --- get/set for modulators
	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}
	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

	Timer getWindowTimer() { return windowTimer; }
protected:
	// --- MIDI Data Interface
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- modulators
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<WindowEGParameters> parameters = nullptr;

	Timer windowTimer;
	double windowWidth_mSec = 0.0;
	uint32_t windowWidth_Samples = 0.0;

	Timer holdTimer;
	double holdTime_mSec = 0.0;

	Timer delayTimer;
	double delayTime_mSec = 0.0;

	Timer offTimer;
	double offTime_mSec = 0.0;

	// --- sample rate for time calculations
	double sampleRate = 0.0;			///< sample rate

	// --- the current output of the EG
	double envelopeOutput = 0.0;		///< the current envelope output sample

	// --- stage variable
	WindowEGState state = WindowEGState::kOff;		///< EG state variable

	// --- RUN/STOP flag
	bool noteOn = false;

};
