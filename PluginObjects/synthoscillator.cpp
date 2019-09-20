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

	// --- for noise generation
	srand(time(NULL));

	return true;
}

bool SynthOsc::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	wavetableOscillator->doNoteOn(midiPitch, _midiNoteNumber, midiNoteVelocity);
	
	return true;
}

bool SynthOsc::doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	wavetableOscillator->doNoteOff(midiPitch, _midiNoteNumber, midiNoteVelocity);

	return true;
}

bool SynthOsc::update(bool updateAllModRoutings)
{
	if (parameters->oscillatorWaveform == SynthOscWaveform::kSin ||
		parameters->oscillatorWaveform == SynthOscWaveform::kAnalogSaw_WT ||
		parameters->oscillatorWaveform == SynthOscWaveform::kParabola)
		wavetableOscillator->update(updateAllModRoutings);
	// --- add more else statements here


	return true;
}

const OscillatorOutputData SynthOsc::renderAudioOutput()
{
	// --- prep output buffer
	oscillatorAudioData.outputs[0] = 0.0;
	oscillatorAudioData.outputs[1] = 0.0;

	if(parameters->oscillatorWaveform == SynthOscWaveform::kSin ||
		parameters->oscillatorWaveform == SynthOscWaveform::kAnalogSaw_WT ||
		parameters->oscillatorWaveform == SynthOscWaveform::kParabola)
		oscillatorAudioData = wavetableOscillator->renderAudioOutput();
	
	// --- add more else statements here to render other oscillators


	return oscillatorAudioData;
}

