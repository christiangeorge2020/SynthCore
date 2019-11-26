// --- Synth Core v1.0
//
#include "vafilters.h"

/**
\brief Perform note-on operations for the component
\return true if handled, false if not handled
*/
bool MoogFilter::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	// --- store MIDI info for key tracking, etc...

	keyTrackPitch = midiPitch;

	// --- can also store a note-on flag if needed

	return true;
}

/**
\brief Perform note-off operations for the component
\return true if handled, false if not handled
*/
bool MoogFilter::doNoteOff(double midiPitch, uint32_t midiNoteNumber, uint32_t midiNoteVelocity)
{
	// --- nothing to do (yet?)
	return true;
}

/**
\brief Recalculate the component's internal variables based on GUI modifiers, modulators, and MIDI data
\return true if handled, false if not handled
*/
bool MoogFilter::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 

	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	// --- update limiter; NOTE: this does not use much CPU at all; dB are subtracted
	//     and no raw conversions are done - cheap
	limiters[MONO_CHANNEL].setThreshold_dB(parameters->truToneThreshold_dB);

	// --- calculate modulated filter fc
	// --- for freq shifting as semitones
	// --- this is the total range; though we may only use half in the calc
	//     This is because of how the intensity can invert an EG, and a LFO will modulation in both directions
	double freqModSemitoneRange = semitonesBetweenFrequencies(freqModLow, freqModHigh) / 2.0;

	// --- using bipolar mod input; could be EG or could be LFO
	double freqModSemitones_fc1 = 0.5*freqModSemitoneRange*modulators->modulationInputs[kBipolarMod]; // k_BipolarMod -> Fc1

	// --- perform modulation by multiplying Fc by the offset in semitones
	double fc1 = parameters->enableKeyTrack ? (keyTrackPitch * parameters->keyTrackRatio) : parameters->fc;
	fc1 *= pitchShiftTableLookup(freqModSemitones_fc1);

	boundValue(fc1, freqModLow, freqModHigh);

	// --- updte the subfilters
	ZVAFilterParameters zvaParams = subFilter[0].getParameters();
	zvaParams.fc = fc1;// parameters->fc;
	zvaParams.filterAlgorithm = vaFilterAlgorithm::kLPF1;

	// --- do the update -- NOTE: the subfilters will only recalculate the coeffients
	//							  if the cutoff frequency has changed so this is not as 
	//							  CPU intensive as it looks but you could improve it a bit
	//							  by performing the fc test here (you will need a member
	//							  variable for per-run persistence)
	for (int i = 0; i < NUM_SUBFILTERS; i++)
	{
		subFilter[i].setParameters(zvaParams);
	}

	// --- changing fc on sub-filters requires recalc of Betas
	calculateFilterCoeffs();

	return true; // handled
}

// --- process function
bool MoogFilter::processSynthAudio(SynthProcessorData* audioData)
{
	// --- make sure we have input and outputs
	if (audioData->numInputChannels == 0 || audioData->numOutputChannels == 0)
		return false;

	// --- this is a mono object, so it only has one input and one output channel
	//     other channels will be ignored
	double xn = audioData->inputs[MONO_CHANNEL];

	// --- 4th order MOOG:
	double sigma = 0.0;

	// --- this is the sum of the scaled feedback paths from the S-ports of the subfilters
	sigma = beta[0] * subFilter[0].getS0Port() +
		beta[1] * subFilter[1].getS0Port() +
		beta[2] * subFilter[2].getS0Port() +
		beta[3] * subFilter[3].getS0Port();

	// --- gain comp is a simple on/off switch LPF ONLY!!!!
	if (parameters->enableGainComp)
		xn *= 1.0 + 0.5*K; // --- increase 0.5 for MORE bass

	// --- now figure out u(n) = alpha0*[x(n) - K*sigma]
	double u = alpha0*(xn - K*sigma);

	// --- send u -> LPF1 and then cascade the outputs to form y(n)
	//     NOTE: verbose version; you can nest these function calls
	double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
	double y1 = subFilter[1].processAudioSample(y0);
	double y2 = subFilter[2].processAudioSample(y1);
	double yn = subFilter[3].processAudioSample(y2);

	// -- do the limiter for self oscillation; could also nest with above if you want
	audioData->outputs[MONO_CHANNEL] = limiters[MONO_CHANNEL].processAudio(yn);

	// --- done
	return true;
}