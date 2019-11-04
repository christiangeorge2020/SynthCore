#include "rotor.h"

bool Rotor::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 

	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

	return true;
}

const ModOutputData Rotor::renderModulatorOutput()
{

	ModOutputData lfoXOutput;
	ModOutputData lfoYOutput;

	lfoX->renderModulatorOutput();
	lfoY->renderModulatorOutput();

	return lfoXOutput;
};