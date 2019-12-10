// --- Synth Core v1.0
//
#include "wavetableoscillator.h"

// --- oscillator
WaveTableOsc::WaveTableOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
							std::shared_ptr<SynthOscParameters> _parameters, 
							std::shared_ptr<WaveTableData> _waveTableData)
	: midiInputData(_midiInputData)
	, parameters(_parameters)
	, waveTableData(_waveTableData)
{
	if (!parameters)
		parameters = std::make_shared<SynthOscParameters>();
	
	if (!waveTableData)
		waveTableData = std::make_shared<WaveTableData>();

	// --- SIK_TABLES are the default
	selectedWaveBank = waveTableData->getInterface(getBankIndex(bankSet, parameters->oscillatorBankIndex));
}

WaveTableOsc::~WaveTableOsc()
{
}

// --- ISynthOscillator
bool WaveTableOsc::reset(double _sampleRate)
{
	// --- save sample rate
	sampleRate = _sampleRate;

	// --- reset the VA counter; remove this for free-run operation
	modCounter = 0.0;
	phaseInc = 0.0;
	waveTableReadIndex = 0.0;

	phaseIncDetune = 0.0;
	modCounterDetune = 0.0;

	return true;
}

std::vector<std::string> WaveTableOsc::getWaveformNames(uint32_t bankIndex)
{
	std::vector<std::string> emptyVector;

	// --- decode bank index
	IWaveBank* bank = waveTableData->getInterface(getBankIndex(bankSet, bankIndex));

	if (bank)
		return bank->getWaveformNames();

	emptyVector.clear();

	return emptyVector;
}

bool WaveTableOsc::doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	// --- save pitch
	midiNotePitch = midiPitch;
	midiNoteNumber = _midiNoteNumber;
	renderMidiNoteNumber = midiNoteNumber; // this will get changed later after all modulations
	
	if (!parameters->enableFreeRunMode)
	{
		modCounter = 0.0;
		modCounterDetune = 0.0;
		waveTableReadIndex = 0.0;
	}

	phaseInc = 0.0;
	phaseIncDetune = 0.0;

	return true;
}

bool WaveTableOsc::doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
	return true;
}

bool WaveTableOsc::setUnison(double unisonDetune)
{
	unisonDetune_cents = unisonDetune;
	return true;
}

bool WaveTableOsc::update(bool updateAllModRoutings)
{
	// --- Run priority modulators 
	// --- note that this is a dummy call, just to keep the glide modulator synced
	glideModulator.getNextGlideModSemitones();

	// --- End Priority modulators
	if (!updateAllModRoutings)
		return true;

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

	/*int temperamentOffsetIndex = (midiNoteNumber % 12) - keyNoteNumber;
	double temperamentOffsetCents = temperamentOffsetIndex * 100;
	switch (temperamentChoice)
	{
		case 0: //E.T, aka no change.
		{
			//temperamentOffsetCents -= temperamentCents_Equal[temperamentOffsetIndex]; //will be 0 each time.
			//aka temperamentOffsetCents = 100 - temperamentCentsEqual[1] (100) = 0.
			temperamentOffsetCents = 0.0;
		}
		case 1:
		{
			temperamentOffsetCents -= temperamentCents_Bach_Lehman[temperamentOffsetIndex];
		}
		case 2:
		{
			temperamentOffsetCents -= temperamentCents_1_6_PythagoreanComma[temperamentOffsetIndex];
		}
		case 3:
		{
			temperamentOffsetCents -= temperamentCents_Sorge1758[temperamentOffsetIndex];
		}
		case 4:
		{
			temperamentOffsetCents -= temperamentCents_Vallotti[temperamentOffsetIndex];
		}
		case 5:
		{
			temperamentOffsetCents -= temperamentCents_Barnes[temperamentOffsetIndex];
		}
		case 6:
		{
			temperamentOffsetCents -= temperamentCents_Kellner[temperamentOffsetIndex];
		}
		case 7:
		{
			temperamentOffsetCents -= temperamentCents_Young2[temperamentOffsetIndex];
		}
		case 8:
		{
			temperamentOffsetCents -= temperamentCents_Werckmeister3[temperamentOffsetIndex];
		}
		case 9:
		{
			temperamentOffsetCents -= temperamentCents_Kirnberger3[temperamentOffsetIndex];
		}
		case 10:
		{
			temperamentOffsetCents -= temperamentCents_1_4_SyntonicCommaMeantone[temperamentOffsetIndex];
		}
		case 11:
		{
			temperamentOffsetCents -= temperamentCents_Neidhardt21_BigCity[temperamentOffsetIndex];
		}
		case 12:
		{
			temperamentOffsetCents -= temperamentCents_Neidhardt21_SmallCity[temperamentOffsetIndex];
		}
		case 13:
		{
			temperamentOffsetCents -= temperamentCents_Neidhardt21_Village[temperamentOffsetIndex];
		}
		case 14:
		{
			temperamentOffsetCents -= temperamentCents_Neidhardt21_3rdCircle4[temperamentOffsetIndex];
		}
		case 15:
		{
			temperamentOffsetCents -= temperamentCents_Neidhardt21_5thCircle11[temperamentOffsetIndex];
		}
	}
	*/
  // Major Modes
	double Ionian[15] = { -12.0, -10.0, -8.0, -7.0, -5.0, -3.0, -1.0, 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 12.0 };
	double Dorian[15] = { -12.0, -10.0, -9.0, -7.0, -5.0, -3.0, -2.0, 0.0, 2.0, 3.0, 5.0, 7.0, 9.0, 10.0, 12.0 };
	double Phrygian[15] = { -12.0, -11.0, -9.0, -7.0, -5.0, -3.0, -2.0, 0.0, 1.0, 3.0, 5.0, 7.0, 9.0, 10.0, 12.0 };
	double Lydian[15] = { -12.0, -10.0, -8.0, -6.0, -5.0, -3.0, -1.0, 0.0, 2.0, 4.0, 6.0, 7.0, 9.0, 11.0, 12.0 };
	double Mixolydian[15] = { -12.0, -10.0, -8.0, -7.0, -5.0, -3.0, -2.0, 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 10.0, 12.0 };
	double Aeolian[15] = { -12.0, -10.0, -9.0, -7.0, -5.0, -4.0, -2.0, 0.0, 2.0, 3.0, 5.0, 7.0, 8.0, 10.0, 12.0 };
	double Locrian[15] = { -12.0, -11.0, -9.0, -7.0, -6.0, -4.0, -2.0, 0.0, 1.0, 3.0, 5.0, 6.0, 8.0, 10.0, 12.0 };
	double diff[15] = { 0.0 };
	double scale[15] = { 0.0 };
	double quantFMod, smallestDiff = 0.0;
	if (parameters->scaleSelect != scaleMode::kNone)
	{
		if (parameters->scaleSelect == scaleMode::kIonian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Ionian[i];
		}
		else if (parameters->scaleSelect == scaleMode::kDorian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Dorian[i];
		}
		else if (parameters->scaleSelect == scaleMode::kPhrygian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Phrygian[i];
		}
		else if (parameters->scaleSelect == scaleMode::kLydian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Lydian[i];
		}
		else if (parameters->scaleSelect == scaleMode::kMixolydian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Mixolydian[15];
		}
		else if (parameters->scaleSelect == scaleMode::kAeolian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Aeolian[15];
		}
		else if (parameters->scaleSelect == scaleMode::kLocrian)
		{
			for (int i = 0; i <= 15; i++)
				scale[i] = Locrian[15];
		}

		// Quantize
		for (int i = 0; i <= 15; i++)
		{
			diff[i] = abs(scale[i] - fmodInput);
		}


		double smallestDiff = diff[0];

		for (int i = 1; i <= 15; i++)
		{
			if (smallestDiff > diff[i])
			{
				smallestDiff = diff[i];
				quantFMod = scale[i];
			}
		}
	}

	if (parameters->scaleSelect == scaleMode::kNone)
		quantFMod = fmodInput;

	// --- calculate combined tuning offsets by simply adding values in semitones
	double currentPitchModSemitones = glideMod + 
		(unisonDetune_cents / 100) +
		quantFMod +
		midiPitchBend +
		masterTuning +
		(parameters->detuneOctaves* 12) +						/* octave*12 = semitones */
		(parameters->detuneSemitones) +							/* semitones */
		(parameters->unisonDetuneCents / 100.0);				/* cents/100 = semitones */

	double detunePitchModSemitones = currentPitchModSemitones + (parameters->detuneCents / 100.0); /* cents/100 = semitones */

	// --- lookup the pitch shift modifier (fraction)
	//double pitchShift = pitchShiftTableLookup(currentPitchModSemitones);

	// --- direct calculation version 2^(n/12) - note that this is equal temperatment
	double pitchShift = pow(2.0, currentPitchModSemitones / 12.0);
	double detunePitchShift = pow(2.0, detunePitchModSemitones / 12.0);

	// --- calculate the moduated pitch value
	oscillatorFrequency = midiNotePitch*pitchShift*parameters->fmRatio;
	oscillatorFrequencySlaveOsc = oscillatorFrequency*parameters->hardSyncRatio;

	oscillatorFrequencyDetuned = midiNotePitch * detunePitchShift*parameters->fmRatio;

	// --- BOUND the value to our range - in theory, we would bound this to any NYQUIST
	boundValue(oscillatorFrequency, 0.0, sampleRate / 2.0);
	boundValue(oscillatorFrequencySlaveOsc, 0.0, sampleRate / 2.0);
	
	boundValue(oscillatorFrequencyDetuned, 0.0, sampleRate / 2.0);

	// --- find the midi note closest to the pitch to select the wavetable
	renderMidiNoteNumber = midiNoteNumberFromOscFrequency(oscillatorFrequency);
	renderMidiNoteNumberDetune = midiNoteNumberFromOscFrequency(oscillatorFrequencyDetuned);

	// --- BANK is set here; can have any number of banks
	selectedWaveBank = waveTableData->getInterface(getBankIndex(bankSet, parameters->oscillatorBankIndex));

	// --- calculate phase inc; this uses FINAL oscFrequency variable above
	//
	//     NOTE: uses selected bank from line of code above; these must be in pairs.

	uint32_t tableLen = kDefaultWaveTableLength;
	uint32_t tableLenDetune = kDefaultWaveTableLength;
	selectedWaveTable = selectedWaveBank->selectTable(parameters->oscillatorWaveformIndex, renderMidiNoteNumber, tableLen);
	selectedWaveTableDetuned = selectedWaveBank->selectTable(parameters->oscillatorWaveformIndex, renderMidiNoteNumberDetune, tableLenDetune);
	
	// --- if table size changed, need to reset the current read location
	//     to be in the same relative location as before
	if (tableLen != currentTableLength)
	{
		// --- need to reset the read location to reflect the new table
		double position = waveTableReadIndex / (double)currentTableLength;
		waveTableReadIndex = position*tableLen;
		currentTableLength = tableLen;
	}

	//pTableLen = &tableLen;

	/*
	// SHAPE MODULATION --- from LFO code
	double shapeModulation = modulators->modulationInputs[kShapeModBipolar];
	double shapeRange = (0.98 - 0.01) / 2.0;
	shapeModulation *= shapeRange;

	double cookedShape = parameters->oscillatorShape;
	//if (parameters->modRoute == ModRouting::LFO1_Shape || parameters->modRoute == ModRouting::Both)
	cookedShape += shapeModulation;

	boundValue(cookedShape, 0.01, 0.98);

	// Full period frequency of fundamental lfo frequency
	oscillatorShape_first = 1 / (2 * cookedShape);
	oscillatorShape_second = 1 / (2 * (1 - cookedShape));

	if (modCounter <= parameters->oscillatorShapeSplitPoint)
		oscillatorFrequency *= oscillatorShape_first;
	else
		oscillatorFrequency *= oscillatorShape_second;

	*/


	// --- note that we neex the current table length for this calculation, and we save it
	phaseInc = calculateWaveTablePhaseInc(oscillatorFrequency, sampleRate, currentTableLength);
	phaseIncDetune = calculateWaveTablePhaseInc(oscillatorFrequencyDetuned, sampleRate, currentTableLength);

	return true;
}

const OscillatorOutputData WaveTableOsc::renderAudioOutput()
{
	// --- prep output buffer
	oscillatorAudioData.outputs[0] = 0.0;
	oscillatorAudioData.outputs[1] = 0.0;

	//oscillatorAudioData.outputs[0] = readWaveTable(waveTableReadIndex, phaseInc);

	// --- render into left channel
	oscillatorAudioData.outputs[0] = 0.5 * readWaveTable(waveTableReadIndex, phaseInc);

	// Change selected wavetable for detuned oscillator to avoid aliasing
	//selectedWaveTable = selectedWaveBank->selectTable(parameters->oscillatorWaveformIndex, renderMidiNoteNumberDetune, *pTableLen);
	oscillatorAudioData.outputs[0] += 0.5 * readWaveTable(tableReadIndexDetune, phaseIncDetune);
	

	// --- scale by output amplitude
	oscillatorAudioData.outputs[0] *= (parameters->outputAmplitude * modulators->modulationInputs[kAmpMod]);

	// --- copy to right channel
	oscillatorAudioData.outputs[1] = oscillatorAudioData.outputs[0];

	return oscillatorAudioData;
}

// --- read a table and do linear interpolation
double WaveTableOsc::readWaveTable(double& readIndex, double _phaseInc)
{
	// --- read wave table
	double output = 0.0;
	double phaseModulator = modulators->modulationInputs[kPhaseMod];

	// --- NOTE: when using hard-sync, phase distortion (oscillator shape) is turned off
	if (parameters->enableHardSync)
	{
		// --- add your hard sync code here!

	}
	else
	{
		// --- apply phase modulation, if any -- HINT: you can sum phase modulations
		double phaseModReadIndex = readIndex + phaseModulator * currentTableLength;

		// --- check for multi-wrapping on new read index
		checkAndWrapWaveTableIndex(phaseModReadIndex, currentTableLength);

		// --- do the table read operation
		output = selectedWaveBank->readWaveTable(selectedWaveTable, phaseModReadIndex);
	}

	// --- increment index
	readIndex += _phaseInc;
	checkAndWrapWaveTableIndex(readIndex, currentTableLength);

	// --- adjust with final volume (amplitude)
	return output;
}