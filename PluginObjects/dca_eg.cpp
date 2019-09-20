#include "dca_eg.h"

/**
	\brief Perform note-on operations for the component
	\return true if handled, false if not handled
*/
bool DCA::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	// --- store our MIDI velocity
	midiVelocityGain = mmaMIDItoAtten(midiNoteVelocity);

	// --- set flag (needed?)
	noteOn = true;

	return true;
}

/**
	\brief Perform note-off operations for the component
	\return true if handled, false if not handled
*/
bool DCA::doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{

	return true;
}

/**
	\brief Recalculate the component's internal variables based on GUI modifiers, modulators, and MIDI data
	\return true if handled, false if not handled
*/
bool DCA::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 


	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	// --- apply Max Down modulator
	double ampMod = doUnipolarModulationFromMax(modulators->modulationInputs[kMaxDownAmpMod], 0.0, 1.0);

	// --- support for MIDI Volume CC
	double midiVolumeGain = mmaMIDItoAtten(midiInputData->ccMIDIData[VOLUME_CC07]);

	// --- calculate the final raw gain value
	//     multiply the various gains together: MIDI Velocity * EG Mod * Amp Mod * gain_dB (from GUI, next code line)
	gainRaw = midiVelocityGain * modulators->modulationInputs[kEGMod] * ampMod;

	// --- apply final output gain
	if (parameters->gain_dB > kMinAbsoluteGain_dB) 
		gainRaw *= pow(10.0, parameters->gain_dB / 20.0);
	else
		gainRaw = 0.0; // OFF

	// --- is mute ON? 0 = OFF, 1 = ON
	if (parameters->mute) gainRaw = 0.0;

	// --- now process pan modifiers
	double panTotal = panValue + modulators->modulationInputs[kBipolarMod];

	// --- limit in case pan control is biased
	boundValue(panTotal, -1.0, 1.0);

	// --- equal power calculation in synthfunction.h
	calculatePanValues(panTotal, panLeftGain, panRightGain);

	return true; // handled
}

/**
\brief Perform note-on operations for the component
\return true if handled, false if not handled
*/
bool EnvelopeGenerator::doNoteOn(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{
	restart();

	// --- apply MIDI based modulators - note that this only needs to happen once
	//     and did not belong in old modulation matrix
	if (parameters->velocityToAttackScaling)
	{
		// --- normalize MIDI velocity and invert
		//     MIDI Velocity = 0   --> no change to attack, scalar = 1.0
		//     MIDI Velocity = 127 --> attack time is now 0mSec, scalar = 0.0
		double scalar = 1.0 - (double)midiNoteVelocity / 127.0;
		calculateAttackTime(parameters->attackTime_mSec, scalar);
	}
	if (parameters->noteNumberToDecayScaling)
	{
		// --- normalize MIDI velocity and invert
		//     MIDI Note 0   --> no change to decay, scalar = 1.0
		//     MIDI Note 127 --> decay time is now 0mSec, scalar = 0.0
		double scalar = 1.0 - (double)midiNoteNumber / 127.0;
		calculateDecayTime(parameters->decayTime_mSec, scalar);
	}

	return true;
}

///**
//\brief externally called reset function
//\return true if handled, false if not handled
//*/
bool EnvelopeGenerator::restart()
{
	// --- reset the state
	if (parameters->resetToZero)
	{
		// --- reset
		envelopeOutput = 0.0;
	}
	else
	{
		// --- leave FSM in current state, unless it is off
		//     which means the note went all the way through the release state and turned off
		//     in that case reset to attack
		//if (state == egState::kOff)
		//	// --- go to the attack state
		//	state = egState::kAttack;
	}

	// --- go to the attack state
	state = egState::kAttack;

	//  ---delay is one-time, does not repeat
	if (delayTime_mSec != 0.0)
		state = egState::kDelay;

	holdTimer.resetTimer();
	offTimer.resetTimer();
	delayTimer.resetTimer();

	return true;
}

/**
\brief Perform note-off operations for the component
\return true if handled, false if not handled
*/
bool EnvelopeGenerator::doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{
	//if (holdTime_mSec > 0.0)
	//	return true;

	if (sustainOverride)
	{
		// --- set releasePending flag
		releasePending = true;
		return true;
	}

	// --- go directly to release state
	if (envelopeOutput > 0)
		state = egState::kRelease;
	else // sustain was already at zero
		state = egState::kOff;

	return true; // handled
}

/**
\brief Recalculate the component's internal variables based on GUI modifiers, modulators, and MIDI data
\return true if handled, false if not handled
*/
bool EnvelopeGenerator::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 


	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	// --- sustain level first: other calculations depend on it
	bool sustainUpdate = false;
	if (sustainLevel != parameters->sustainLevel)
		sustainUpdate = true;

	holdTime_mSec = parameters->holdTime_mSec;
	holdTimer.setTargetValueInSamples((holdTime_mSec*sampleRate) / 1000.0);
	
	offTime_mSec = parameters->offTime_mSec;
	offTimer.setTargetValueInSamples((offTime_mSec*sampleRate) / 1000.0);

	delayTime_mSec = parameters->delayTime_mSec;
	delayTimer.setTargetValueInSamples((delayTime_mSec*sampleRate) / 1000.0);

	// --- for new note-on events
	if(delayTime_mSec == 0.0 && state == egState::kDelay)
		state = egState::kAttack;
	
	sustainLevel = parameters->sustainLevel;
	resetToZero = parameters->resetToZero;

	if (attackTime_mSec != parameters->attackTime_mSec)
		calculateAttackTime(parameters->attackTime_mSec);

	if (sustainUpdate || (decayTime_mSec != parameters->decayTime_mSec))
		calculateDecayTime(parameters->decayTime_mSec);

	if (releaseTime_mSec != parameters->releaseTime_mSec)
		calculateReleaseTime(parameters->releaseTime_mSec);

	// --- update MIDI stuff
	setSustainOverride(midiInputData->ccMIDIData[SUSTAIN_PEDAL] > 63);

	return true;
}

///**
//\brief Perform operations for voice-stealing operation
//\return true if handled, false if not handled
//*/
bool EnvelopeGenerator::shutdown()
{
	// --- legato mode - ignore
	//if (parameters->legatoMode)
	//	return false;

	// --- calculate the linear inc values based on current outputs
	incShutdown = -(1000.0*envelopeOutput) / shutdownTime_mSec / sampleRate;

	// --- set state and reset counter
	state = egState::kShutdown;

	// --- for sustain pedal
	sustainOverride = false;
	releasePending = false;

	return true;
}

/**
\brief Calculate the attack time variables including the coefficient and offsets

\param attackTime: the attack time in mSec
\param attackTimeScalar: a scalar value on the range [0, +1] for using the velocity to attack time modulation
*/
void EnvelopeGenerator::calculateAttackTime(double attackTime, double attackTimeScalar)
{
	// --- store for comparing so don't need to waste cycles on updates
	attackTime_mSec = attackTime;

	// --- samples for the exponential rate
	double samples = sampleRate*((attackTime_mSec*attackTimeScalar) / 1000.0);

	// --- coeff and base for iterative exponential calculation
	attackCoeff = exp(-log((1.0 + attackTCO) / attackTCO) / samples);
	attackOffset = (1.0 + attackTCO)*(1.0 - attackCoeff);
}

/**
\brief Calculate the decay time variables including the coefficient and offsets

\param decayTime: the decay time in mSec
\param decayTimeScalar: a scalar value on the range [0, +1] for using the note number to decay time modulation
*/
void EnvelopeGenerator::calculateDecayTime(double decayTime, double decayTimeScalar)
{
	// --- store for comparing so don't need to waste cycles on updates
	decayTime_mSec = decayTime;

	// --- samples for the exponential rate
	double samples = sampleRate*((decayTime_mSec*decayTimeScalar) / 1000.0);

	// --- coeff and base for iterative exponential calculation
	decayCoeff = exp(-log((1.0 + decayTCO) / decayTCO) / samples);
	decayOffset = (sustainLevel - decayTCO)*(1.0 - decayCoeff);
}

/**
\brief Calculate the reease time variables including the coefficient and offsets

\param releaseTime: the reease time in mSec
\param releaseTimeScalar: a scalar value on the range [0, +1], currently not used but could be used for a MIDI modulation
*/
void EnvelopeGenerator::calculateReleaseTime(double releaseTime, double releaseTimeScalar)
{
	// --- store for comparing so don't need to waste cycles on updates
	releaseTime_mSec = releaseTime;

	// --- samples for the exponential rate
	double samples = sampleRate*((releaseTime_mSec*releaseTimeScalar) / 1000.0);

	// --- coeff and base for iterative exponential calculation
	releaseCoeff = exp(-log((1.0 + releaseTCO) / releaseTCO) / samples);
	releaseOffset = -releaseTCO*(1.0 - releaseCoeff);
}

/**
\brief Run the EG through one cycle of the finite state machine.
\return true if handled, false if not handled
*/
const ModOutputData EnvelopeGenerator::renderModulatorOutput()
{
	ModOutputData egOutput;

	// --- decode the state
	switch (state)
	{
		case egState::kOff:
		{
			if (offTime_mSec != 0.0)
			{
				state = egState::kAttack;
				offTimer.resetTimer();
				break;
			}

			// --- output is OFF
			if (!outputEG || parameters->resetToZero)
				envelopeOutput = 0.0;
			break;
		}
		case egState::kDelay:
		{
			// --- check expired
			if (delayTime_mSec == 0.0 || delayTimer.timerExpired())
			{
				// go to next state
				state = egState::kAttack;	

				delayTimer.resetTimer();
				break;
			}

			// --- increment counter
			delayTimer.advanceTimer();
			break;
		}

		case egState::kAttack:
		{
			// --- render value
			envelopeOutput = attackOffset + envelopeOutput*attackCoeff;

			// --- check go to next state
			if (envelopeOutput >= 1.0 || attackTime_mSec <= 0.0)
			{
				envelopeOutput = 1.0;
				if(parameters->egContourType == egType::kADSR)
					state = egState::kDecay;	// go to kDecay
				else if (parameters->egContourType == egType::kAHR)
					state = egState::kHoldOn;	// go to kHoldOn
				else
					state = egState::kDecay;	// go to kDecay

				break;
			}
			break;
		}
	
		case egState::kDecay:
		{
			// --- render value
			envelopeOutput = decayOffset + envelopeOutput*decayCoeff;

			// --- check go to next state
			if (envelopeOutput <= sustainLevel || decayTime_mSec <= 0.0)
			{
				envelopeOutput = sustainLevel;
			//	if (parameters->egContourType == egType::kADSR)
					state = egState::kSustain;		// go to next state
				break;
			}
			break;
		}
		case egState::kHoldOn:
		{
			// --- check expired
			if (holdTime_mSec == 0.0 || holdTimer.timerExpired())
			{
				if (parameters->egContourType == egType::kAHR)
					state = egState::kRelease;	// go to kAR_Release
				else
					state = egState::kDecay;	// go to next state
				
				holdTimer.resetTimer();
				break;
			}

			// --- increment counter
			holdTimer.advanceTimer();
			break;
		}
		case egState::kSustain:
		{
			// --- render value
			envelopeOutput = sustainLevel;
			break;
		}
	
		case egState::kRelease:
		{
			// --- if sustain pedal is down, override and return
			if (sustainOverride)
			{
				// --- leave envelopeOutput at whatever value is currently has
				//     this is in case the user hits the sustain pedal after the note is released
				break;
			}
			else
				// --- render value
				envelopeOutput = releaseOffset + envelopeOutput*releaseCoeff;

			// --- check go to next state
			if (envelopeOutput <= 0.0 || releaseTime_mSec <= 0.0)
			{
				envelopeOutput = 0.0;

				if(offTime_mSec <= 0.0)
					state = egState::kOff;			// go to OFF state
				else
					state = egState::kHoldOff;		// go to OFF state

				break;
			}
			break;
		}
		case egState::kShutdown:
		{
		/*		if (resetToZero)
			{
		*/			// --- the shutdown state is just a linear taper since it is so short
				envelopeOutput += incShutdown;

				// --- check go to next state
				if (envelopeOutput <= 0)
				{
					state = egState::kOff;		// go to next state
					envelopeOutput = 0.0;		// reset envelope
					break;
				}
		//	}
			break; // this is needed!!
		}
		case egState::kHoldOff:
		{
			// --- check expired
			if (offTime_mSec == 0.0 || offTimer.timerExpired())
			{
				state = egState::kAttack;
				offTimer.resetTimer();
				break;
			}

			// --- increment counter
			offTimer.advanceTimer();
			break;
		}
	}

	// --- load up the outut struct
	egOutput.modOutputCount = 2;
	egOutput.modulationOutputs[kEGNormalOutput] = envelopeOutput;
	egOutput.modulationOutputs[kEGBiasedOutput] = envelopeOutput - sustainLevel;

	return egOutput;
}

/**
\brief Recalculate the time constant offsets (TCOs) when the mode changes; this effectively sets the curvature of the eg segments
*/
void EnvelopeGenerator::setEGMode(egTCMode mode)
{
	// --- save it
	egMode = mode;

	// --- analog - use e^-1.5x, e^-4.95x
	if (mode == egTCMode::kAnalog)
	{
		attackTCO = exp(-1.5);  // fast attack
		decayTCO = exp(-4.95);
		releaseTCO = decayTCO;
	}
	else
	{
		// digital is linear-in-dB so use
		attackTCO = 0.99999;
		decayTCO = decayTCO = exp(-11.05);
		releaseTCO = decayTCO;
	}

	// --- recalc these
	calculateAttackTime(attackTime_mSec);
	calculateDecayTime(decayTime_mSec);
	calculateReleaseTime(releaseTime_mSec);
}

