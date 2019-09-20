#include "window_eg.h"

/**
\brief Perform note-on operations for the component
\return true if handled, false if not handled
*/
bool WindowEG::doNoteOn(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{
	noteOn = true;
	restart();
	return true;
}

void WindowEG::restart()
{
	offTimer.resetTimer();
	windowTimer.resetTimer();
	state = WindowEGState::kWindowing;
}

/**
\brief Perform note-off operations for the component
\return true if handled, false if not handled
*/
bool WindowEG::doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{
	noteOn = false;
	state = WindowEGState::kOff;

	return true; // handled
}

/**
\brief Recalculate the component's internal variables based on GUI modifiers, modulators, and MIDI data
\return true if handled, false if not handled
*/
bool WindowEG::update(bool updateAllModRoutings)
{
	// --- Run priority modulators


	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	windowWidth_mSec = parameters->windowWidth_mSec;
	windowWidth_Samples = parameters->windowWidth_Samples;

	if (!parameters->widthInSamples)
		windowWidth_Samples = (windowWidth_mSec*sampleRate) / 1000.0;

	if (windowWidth_Samples < 8)
		windowWidth_Samples = 8;

	windowTimer.setTargetValueInSamples(windowWidth_Samples); 

	/*holdTime_mSec = parameters->holdTime_mSec;
	holdTimer.setTargetValueInSamples((holdTime_mSec*sampleRate) / 1000.0);*/

	offTime_mSec = parameters->offTime_mSec;
	offTimer.setTargetValueInSamples((offTime_mSec*sampleRate) / 1000.0);

	//delayTime_mSec = parameters->delayTime_mSec;
	//delayTimer.setTargetValueInSamples((delayTime_mSec*sampleRate) / 1000.0);

	return true;
}

/**
\brief Run the EG through one cycle of the finite state machine.
\return true if handled, false if not handled
*/
const ModOutputData WindowEG::renderModulatorOutput()
{
	ModOutputData egOutput;

	// --- decode the state
	switch (state)
	{
		case WindowEGState::kOff:
		{
			if (offTime_mSec != 0.0)
			{
				state = WindowEGState::kWindowing;
				offTimer.resetTimer();
				break;
			}

			envelopeOutput = 0.0;
			break;
		}

		case WindowEGState::kWindowing:
		{
			// --- render value
			uint32_t tick = windowTimer.getTick();

			envelopeOutput = 0.5 * (1 - cos((tick*2.0*kPi) / (windowWidth_Samples)));
	
			// --- check expired
			if (windowTimer.timerExpired())
			{
				state = WindowEGState::kHoldOff;
				windowTimer.resetTimer();
				break;
			}

			// --- increment counter
			windowTimer.advanceTimer();
			break;
		}

		case WindowEGState::kHoldOff:
		{
			// --- check expired
			if (offTime_mSec == 0.0 || offTimer.timerExpired())
			{
				if (offTime_mSec == 0.0)
					state = WindowEGState::kOff;
				else
					state = WindowEGState::kWindowing;
				offTimer.resetTimer();
				break;
			}

			// --- increment counter
			offTimer.advanceTimer();
			break;
		}
	}

	// --- load up the outut struct
	egOutput.modOutputCount = 1;
	egOutput.modulationOutputs[kWindowEGNormalOutput] = envelopeOutput;

	return egOutput;
}
