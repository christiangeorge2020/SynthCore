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

	// --- calculate modulated filter fc
	// --- for freq shifting as semitones
	// --- this is the total range; though we may only use half in the calc
	//     This is because of how the intensity can invert an EG, and a LFO will modulation in both directions
	double freqModSemitoneRange = semitonesBetweenFrequencies(freqModLow, freqModHigh) / 2.0;

	// --- using bipolar mod input; could be EG or could be LFO
	double freqModSemitones_fc1 = 0.5*freqModSemitoneRange*modulators->modulationInputs[kBipolarMod]; // k_BipolarMod -> Fc1

	// --- perform modulation by multiplying Fc by the offset in semitones
	double fc1 = parameters->fc * pitchShiftTableLookup(freqModSemitones_fc1);
	boundValue(fc1, freqModLow, freqModHigh);

	// --- updte the subfilters
	ZVAFilterParameters zvaParams = subFilter[0].getParameters();
	zvaParams.fc = fc1;// parameters->fc;
	zvaParams.filterAlgorithm = vaFilterAlgorithm::kLPF1;

	for (int i = 0; i < NUM_SUBFILTERS; i++)
	{
		if (i > 1 && parameters->filterAlgorithm == moogFilterAlgorithm::kLPF2)
			zvaParams.filterAlgorithm = vaFilterAlgorithm::kAPF1;

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
	audioData->outputs[0] = processAudioSample(audioData->inputs[0]);

	// --- done
	return true;
}