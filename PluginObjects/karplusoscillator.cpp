// --- Synth Core v1.0
//
#include "karplusoscillator.h"

KarplusOsc::KarplusOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
						std::shared_ptr<SynthOscParameters> _parameters)
	: midiInputData(_midiInputData)
	, parameters(_parameters)
{
	if (!parameters)
		parameters = std::make_shared<KarplusOscParameters>();
	
	srand(time(NULL)); // --- seed random number generator

	// --- randomize the PN register
	pnRegister = rand();

	noiseEG.reset(new EnvelopeGenerator(midiInputData, parameters->noiseEGParameters));
	parameters->noiseEGParameters->egContourType = egType::kAHR;
	resonator.reset(new Resonator());
}

KarplusOsc::~KarplusOsc()
{
}

// --- ISynthOscillator
bool KarplusOsc::reset(double _sampleRate)
{
	// --- save sample rate
	sampleRate = _sampleRate;

	noiseEG->reset(_sampleRate);
	resonator->reset(_sampleRate);

	return true;
}

bool KarplusOsc::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	// --- save pitch
	midiNotePitch = midiPitch;
	midiNoteNumber = _midiNoteNumber;

	// --- for noise resonator
	noiseEG->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
	resonator->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);

	return true;
}

bool KarplusOsc::doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	return true;
}

bool KarplusOsc::update(bool updateAllModRoutings)
{
	// --- Run priority modulators
	// --- note that this is a dummy call, just to keep the glide modulator synced
	glideModulator.getNextGlideModSemitones();

	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	// --- EG update
	noiseEG->update(updateAllModRoutings);

	// --- calculate MIDI pitch bend range
	double midiPitchBendRange = midiInputData->globalMIDIData[kMIDIMasterPBSensCoarse] +
		(midiInputData->globalMIDIData[kMIDIMasterPBSensFine] / 100.0);

	// --- calculate MIDI pitch bend (USER)
	double midiPitchBend = midiPitchBendRange * midiPitchBendToBipolar(midiInputData->globalMIDIData[kMIDIPitchBendDataLSB],
		midiInputData->globalMIDIData[kMIDIPitchBendDataMSB]);

	// --- coarse (semitones): -64 to +63 maps-> 0, 127 (7-bit)
	int mtCoarse = midiInputData->globalMIDIData[kMIDIMasterTuneCoarseMSB];
	mapIntValue(mtCoarse, 0, 127, -64, +63, false); // false = no rounding

	// --- fine (cents): -100 to +100 as MIDI 14-bit
	double mtFine = midi14_bitToDouble(midiInputData->globalMIDIData[kMIDIMasterTuneFineLSB], midiInputData->globalMIDIData[kMIDIMasterTuneFineMSB], -100.0, 100.0);

	// --- this gives proper int.fraction value
	double masterTuning = (double)mtCoarse + ((double)mtFine / 100.0);

	// --- calculate combined tuning offsets by simply adding values in semitones
	double fmodInput = modulators->modulationInputs[kBipolarMod] * kOscBipolarModRangeSemitones;

	// --- do the portamento
	double glideMod = glideModulator.getNextGlideModSemitones();

	// --- calculate combined tuning offsets by simply adding values in semitones
	double currentPitchModSemitones = glideMod +
		fmodInput +
		midiPitchBend +
		masterTuning +
		(parameters->detuneOctaves * 12) +						/* octave*12 = semitones */
		(parameters->detuneSemitones) +							/* semitones */
		(parameters->detuneCents / 100.0) +						/* cents/100 = semitones */
		(parameters->unisonDetuneCents / 100.0);					/* cents/100 = semitones */

	// --- lookup the pitch shift modifier (fraction)
	//double pitchShift = pitchShiftTableLookup(currentPitchModSemitones);

	// --- direct calculation version 2^(n/12) - note that this is equal temperatment
	double pitchShift = pow(2.0, currentPitchModSemitones / 12.0);

	// --- calculate the moduated pitch value
	oscillatorFrequency = midiNotePitch*pitchShift;

	// --- BOUND the value to our range - in theory, we would bound this to any NYQUIST
	boundValue(oscillatorFrequency, 0.0, sampleRate / 2.0);

	// --- resonator data
	resonator->setResonatorTuning_Hz(oscillatorFrequency);
	resonator->setDecay(parameters->resonatorDecay);

	return true;
}

const OscillatorOutputData KarplusOsc::renderAudioOutput()
{
	// --- prep output buffer
	oscillatorAudioData.outputs[0] = 0.0;
	oscillatorAudioData.outputs[1] = 0.0;

	// --- exciter generation
	double noise = 0.0;

	// --- noise-grain generation
	ModOutputData noiseEGOutput = noiseEG->renderModulatorOutput();
	//ModOutputData windowEGOutput = windowEG->renderModulatorOutput();
	
	double egOut = noiseEGOutput.modulationOutputs[kEGNormalOutput];
	
	if (parameters->noiseType == NoiseType::kWhite)
		noise = doWhiteNoise() * egOut;
	else
		noise = doPNSequence(pnRegister) * egOut;
	
	// --- process exciter through resonator
	oscillatorAudioData.outputs[0] = resonator->processAudioSample(noise);

	// --- scale by amplitude
	oscillatorAudioData.outputs[0] *= parameters->outputAmplitude;

	// --- copy to right channel
	oscillatorAudioData.outputs[1] = oscillatorAudioData.outputs[0];

	return oscillatorAudioData;
}

