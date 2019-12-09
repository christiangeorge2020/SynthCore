// --- Synth Core v1.0
//
#include "synthoscillator.h"



SynthOsc::SynthOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
					std::shared_ptr<SynthOscParameters> _parameters, 
					std::shared_ptr<WaveTableData> _waveTableData)
	: midiInputData(_midiInputData)
	, parameters(_parameters)
	, waveTableData(_waveTableData)
{
	// --- create subcomponents
	if (!parameters)
		parameters = std::make_shared<SynthOscParameters>();

	// --- create sub-components
	wavetableOscillator.reset(new WaveTableOsc(midiInputData, parameters, waveTableData));

	// --- **7**
	wavetableOscillator->setBankSet(BANK_SET_0);

	// --- randomize the PN register
	pnRegister = rand();

	noiseEGParameters->egContourType = egType::kAHR;
	noiseEGParameters->attackTime_mSec = parameters->noiseEGAttack;
	noiseEGParameters->holdTime_mSec = parameters->noiseEGHold;
	noiseEGParameters->releaseTime_mSec = parameters->noiseEGRelease;

	noiseEG.reset(new EnvelopeGenerator(midiInputData, noiseEGParameters));
	
	resonator.reset(new Resonator());
}

// --- **7**
void SynthOsc::setBankSet(uint32_t _bankSet)
{
	wavetableOscillator->setBankSet(_bankSet);
}
uint32_t SynthOsc::getBankSet()
{
	return wavetableOscillator->getBankSet();
}
// --- **7**
std::vector<std::string> SynthOsc::getWaveformNames(uint32_t bankIndex)
{
	return wavetableOscillator->getWaveformNames(bankIndex);
}
std::vector<std::string> SynthOsc::getBankNames()
{
	return wavetableOscillator->getBankNames();
}
SynthOsc::~SynthOsc()
{

}

// --- ISynthOscillator
bool SynthOsc::reset(double _sampleRate)
{
	// --- reset all subcomponents here
	//
	wavetableOscillator->reset(_sampleRate);
	wavetableOscillator->setModulators(modulators);

	noiseEG->reset(_sampleRate);
	resonator->reset(_sampleRate);

	//// --- for noise generation
	srand(time(NULL));

	return true;
}

bool SynthOsc::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	wavetableOscillator->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
	//midiNotePitch = midiPitch;

	// --- for noise resonator
	noiseEG->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
	resonator->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
	
	return true;
}

bool SynthOsc::doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	wavetableOscillator->doNoteOff(midiPitch, _midiNoteNumber, midiNoteVelocity);

	return true;
}




bool SynthOsc::update(bool updateAllModRoutings)
{
	wavetableOscillator->update(updateAllModRoutings);

	// --- EG update
	noiseEG->update(updateAllModRoutings);

	resonator->setDecay(parameters->resonatorDecay);
	//resonator->setResonatorTuning_Hz(midiNotePitch);

	noiseEGParameters->egContourType = egType::kAHR;
	noiseEGParameters->attackTime_mSec = parameters->noiseEGAttack;
	noiseEGParameters->holdTime_mSec = parameters->noiseEGHold;
	noiseEGParameters->releaseTime_mSec = parameters->noiseEGRelease;

	return true;
}

const OscillatorOutputData SynthOsc::renderAudioOutput()
{
	// --- prep output buffer
	oscillatorAudioData.outputs[0] = 0.0;
	oscillatorAudioData.outputs[1] = 0.0;

	// --- exciter generation
	double noise = 0.0;

	if (parameters->exciterInput == ExciterMode::kNone) {
		oscillatorAudioData = wavetableOscillator->renderAudioOutput();
		return oscillatorAudioData;
	}
	else if (parameters->exciterInput == ExciterMode::kNoise) {
		// --- noise-grain generation
		ModOutputData noiseEGOutput = noiseEG->renderModulatorOutput();
		double egOut = noiseEGOutput.modulationOutputs[kEGNormalOutput];
		noise = doWhiteNoise() * egOut;

		oscillatorAudioData.outputs[0] = resonator->processAudioSample(noise);
		oscillatorAudioData.outputs[1] = oscillatorAudioData.outputs[0];

		return oscillatorAudioData;
	}
	else if (parameters->exciterInput == ExciterMode::kWave) {
		oscillatorAudioData = wavetableOscillator->renderAudioOutput();

		ModOutputData noiseEGOutput = noiseEG->renderModulatorOutput();
		double egOut = noiseEGOutput.modulationOutputs[kEGNormalOutput];

		oscillatorAudioData = wavetableOscillator->renderAudioOutput();

		oscillatorAudioData.outputs[0] *= egOut;

		oscillatorAudioData.outputs[0] = resonator->processAudioSample(oscillatorAudioData.outputs[0]);
		oscillatorAudioData.outputs[1] = oscillatorAudioData.outputs[0];

		return oscillatorAudioData;

	}
	

	// ModOutputData windowEGOutput = windowEG->renderModulatorOutput();

	//// --- scale by amplitude
	//oscillatorAudioData.outputs[0] *= parameters->outputAmplitude;

	oscillatorAudioData = wavetableOscillator->renderAudioOutput();

	oscillatorAudioData.outputs[0] = resonator->processAudioSample(oscillatorAudioData.outputs[0]);
	oscillatorAudioData.outputs[1] = oscillatorAudioData.outputs[0];
	
	return oscillatorAudioData;
}

