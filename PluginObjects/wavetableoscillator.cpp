// --- Synth Core v1.0
//
#include "wavetableoscillator.h"

// --- datasource
bool WaveTableBank::createBandLimitedTables(uint32_t waveform, uint32_t tableInterval,
											double* multiTable128[MAX_WAVE_TABLES], 
											uint32_t tableLength, double sampleRate)
{
	unsigned int numTables = 0;
	double seedFreq = 0.0;
	uint32_t seedMIDINote = 0;

	if (tableInterval == wtTableInterval::highRes)
	{
		seedFreq = pow(2.0, (-69.0 / 12.0)) * 440.0; // MIDI note 0 = 8.175.....
		seedMIDINote = 0;
		numTables = calculateNumTables(seedMIDINote, 1);
	}
	else if (tableInterval == wtTableInterval::octave)
	{
		seedFreq = 27.5; // --- Note A0, bottom of piano = 27.5Hz
		seedMIDINote = 21;
		numTables = calculateNumTables(seedMIDINote, 12);
	}
	else if (tableInterval == wtTableInterval::min3rd)
	{
		seedFreq = 27.5; // --- Note A0, bottom of piano = 27.5Hz
		seedMIDINote = 21;
		numTables = calculateNumTables(seedMIDINote, 3);
	}

	// --- create the tables
	for (int j = 0; j < numTables; j++)
	{
		// --- create new buffer
		double* tableAccumulator = new double[tableLength];
		memset(tableAccumulator, 0, tableLength * sizeof(double));

		int numHarmonics = (int)((sampleRate / 2.0 / seedFreq) - 1.0);

		double maxTableValue = 0;
		int halfNumHarmonics = (int)((double)numHarmonics / 2.0);

		for (int i = 0; i < tableLength; i++)
		{
			// --- for bandlimited tables based on full harmonics
			for (int g = 0; g <= numHarmonics; g++)
			{
				// --- some equations are based on half the total harmonics, such as triangle here
				//     as it skips harmonics
				if (waveform == wtWaveFormIndex::TRIANGLE_WAVE && g <= halfNumHarmonics)
				{
					// --- triangle
					tableAccumulator[i] += generateTriangleHarmonic(i, double(g), tableLength);
				}
				if(waveform == wtWaveFormIndex::PARABOLIC_WAVE) // --- others are based on all harmonics in sequence
				{
					// --- Parabola Waveform
					tableAccumulator[i] += generateParabolaHarmonic(i, double(g), tableLength);
				}
			}

			// --- store the max values
			if (i == 0)
			{
				maxTableValue = tableAccumulator[i];
			}
			else
			{
				// --- test and store
				if (tableAccumulator[i] > maxTableValue)
					maxTableValue = tableAccumulator[i];
			}
		}

		// --- normalize
		for (int i = 0; i < tableLength; i++)
			tableAccumulator[i] /= maxTableValue;

		// --- store on parabolix table set in one of 128 slots
		multiTable128[seedMIDINote] = tableAccumulator;

		// --- next table is one octave up
		if (tableInterval == wtTableInterval::octave)
		{
			seedFreq *= 2.0;
			seedMIDINote += 12;
		}
		else if (tableInterval == wtTableInterval::min3rd)
		{
			seedFreq *= pow(2.0, (3.0 / 12.0));
			seedMIDINote += 3;
		}
		else if (tableInterval == wtTableInterval::highRes)
		{
			seedFreq *= pow(2.0, (1.0 / 12.0));
			seedMIDINote += 1;
		}
	}

	// --- for high-res, we are done!
	if (tableInterval == wtTableInterval::highRes)
		return true;

	// --- now replicate the table pointers
	int nLastIndex = -1;
	int numWaveTables = 128;

	double* pLastTable = NULL;
	for (int i = 0; i<128; i++)
	{
		if (multiTable128[i])
		{
			nLastIndex = i;
			pLastTable = multiTable128[i];
		}
	}

	if (!pLastTable)
		return false;// no tables : (

	// upper part first
	for (int i = 127; i >= nLastIndex; i--)
		multiTable128[i] = pLastTable;

	int index = nLastIndex - 1; // first index already has value in it
	bool bWorking = index >= 0 ? true : false;

	while (bWorking)
	{
		if (!multiTable128[index])
			multiTable128[index] = pLastTable;
		else
			pLastTable = multiTable128[index];

		index--;

		if (index < 0)
			bWorking = false;
	}

	return true;
}

// --- generate tables based on sample rate
bool WaveTableBank::resetWaveTables(double sampleRate)
{
	// --- example of parabolas on octave boundaries
	bool createdParabolas = createBandLimitedTables(PARABOLIC_WAVE, wtTableInterval::min3rd, 
													wavetables[PARABOLIC_WAVE]->pdMultiTable128, 
													wavetables[PARABOLIC_WAVE]->currentWaveTableLen,
													sampleRate);
	
	bool createdTriangles = createBandLimitedTables(TRIANGLE_WAVE, wtTableInterval::min3rd, 
													wavetables[TRIANGLE_WAVE]->pdMultiTable128,
													wavetables[TRIANGLE_WAVE]->currentWaveTableLen,
													sampleRate);

	// --- add more here?

	// --- true if all succeed
	return createdParabolas && createdTriangles;
}


// --- returns the length of the selected table, but not necessarily used
uint32_t WaveTableBank::selectTable(int oscillatorWaveformIndex, uint32_t midiNoteNumber)
{
	// --- for testing
	oscillatorWaveformIndex = 2; // for testing remove this.....later.....

	// --- tables are stored with oscillator waveform index
	selectedWT = wavetables[oscillatorWaveformIndex];
	
	// --- access Wavetable structure via vector container as array notation []
	selectedWT->selectTable(midiNoteNumber);
	
	return selectedWT->currentWaveTableLen;
}


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
	selectedWaveBank = waveTableData->getInterface(SIK_TABLES);
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

	return true;
}

std::vector<std::string> WaveTableOsc::getWaveformNames(uint32_t bankIndex)
{
	std::vector<std::string> emptyVector;

	// --- decode bank index
	IWaveBank* bank = waveTableData->getInterface(bankIndex);
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
		waveTableReadIndex = 0.0;
	}

	phaseInc = 0.0;

	return true;
}

bool WaveTableOsc::doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity)
{
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

	// Major Modes
	double Ionian[15] = { -12.0, -10.0, -8.0, -7.0, -5.0, -3.0, -1.0, 0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 12.0 };
	double diff[15] = { 0.0 };

	// Quantize
	for (int i = 0; i <= 15; i++) {
		diff[i] = abs(Ionian[i] - fmodInput);
	}
	
	double quantFMod; 
	double smallestDiff = diff[0];
	for (int i = 1; i <= 15; i++) {
		if (smallestDiff > diff[i]) {
			smallestDiff = diff[i];
			quantFMod = Ionian[i];
		}
	}


	// --- calculate combined tuning offsets by simply adding values in semitones
	double currentPitchModSemitones = glideMod + 
		quantFMod +
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
	oscillatorFrequency = midiNotePitch*pitchShift*parameters->fmRatio;
	oscillatorFrequencySlaveOsc = oscillatorFrequency*parameters->hardSyncRatio;

	// --- BOUND the value to our range - in theory, we would bound this to any NYQUIST
	boundValue(oscillatorFrequency, 0.0, sampleRate / 2.0);
	boundValue(oscillatorFrequencySlaveOsc, 0.0, sampleRate / 2.0);
	
	// --- find the midi note closest to the pitch to select the wavetable
	renderMidiNoteNumber = midiNoteNumberFromOscFrequency(oscillatorFrequency);

	// --- BANK is set here; can have any number of banks
	selectedWaveBank = waveTableData->getInterface(parameters->oscillatorBankIndex);

	// --- calculate phase inc; this uses FINAL oscFrequency variable above
	//
	//     NOTE: uses selected bank from line of code above; these must be in pairs.
	uint32_t tableLen = selectedWaveBank->selectTable(parameters->oscillatorWaveformIndex, renderMidiNoteNumber);
	
	// --- if table size changed, need to reset the current read location
	//     to be in the same relative location as before
	if (tableLen != currentTableLength)
	{
		// --- need to reset the read location to reflect the new table
		double position = waveTableReadIndex / (double)currentTableLength;
		waveTableReadIndex = position*tableLen;
		currentTableLength = tableLen;
	}

	// --- note that we neex the current table length for this calculation, and we save it
	phaseInc = calculateWaveTablePhaseInc(oscillatorFrequency, sampleRate, currentTableLength);

	return true;
}

const OscillatorOutputData WaveTableOsc::renderAudioOutput()
{
	// --- prep output buffer
	oscillatorAudioData.outputs[0] = 0.0;
	oscillatorAudioData.outputs[1] = 0.0;

	// --- render into left channel
	oscillatorAudioData.outputs[0] = readWaveTable(waveTableReadIndex, phaseInc);

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
		output = selectedWaveBank->readWaveTable(phaseModReadIndex);
	}

	// --- increment index
	readIndex += _phaseInc;
	checkAndWrapWaveTableIndex(readIndex, currentTableLength);

	// --- adjust with final volume (amplitude)
	return output;
}