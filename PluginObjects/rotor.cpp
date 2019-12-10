#include "rotor.h"

bool Rotor::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 


	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	parameters->lfoXParameters->waveform = parameters->waveform_X;
	parameters->lfoXParameters->mode = parameters->mode_X;
	parameters->lfoXParameters->frequency_Hz = parameters->frequency_Hz_X;
	parameters->lfoXParameters->outputAmplitude = parameters->outputAmplitude_X;

	parameters->lfoYParameters->waveform = parameters->waveform_Y;
	parameters->lfoYParameters->mode = parameters->mode_Y;
	parameters->lfoYParameters->frequency_Hz = parameters->frequency_Hz_Y;
	parameters->lfoYParameters->outputAmplitude = parameters->outputAmplitude_Y;


	lfoX->update(updateAllModRoutings);
	lfoY->update(updateAllModRoutings);

	return true;
}

const ModOutputData Rotor::renderModulatorOutput()
{

	ModOutputData lfoXOutput;
	ModOutputData lfoYOutput;

	lfoX->renderModulatorOutput();
	lfoY->renderModulatorOutput();
	
	// ROTOR MOD MATRIX CONNECTION
	lfoXOutput.modulationOutputs[kRotorXOutput] = lfoXOutput.modulationOutputs[kLFONormalOutput];
	lfoYOutput.modulationOutputs[kRotorYOutput] = lfoYOutput.modulationOutputs[kLFONormalOutput];

	return lfoXOutput;
};