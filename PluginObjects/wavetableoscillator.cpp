// --- Synth Core v1.0
//
#include "wavetableoscillator.h"

// --- other constants
const unsigned int kNumWaveTables = 9;
const unsigned int kMaxNumWaveTables = 128;
const unsigned int kDefaultWaveTableLength = 256;// 512;

bool WaveTableData::resetWaveTables(double sampleRate)
{
	// --- create the tables
	// EXAMPLE: Here I create 128 wavetables, one per MIDI note number, then store the pointers in an array
	//          There will be one unique table per MIDI note
	//
	// --- octaveTables: set to true for octave-based tables (there will be 9 of them, with the last being the sin table)
	bool octaveTables = false;

	unsigned int numTables = octaveTables ? kNumWaveTables : kMaxNumWaveTables;

	// --- Note A0, bottom of piano = 27.5Hz
	//     MIDI note 0 = 8.175.....
	double seedFreq = octaveTables ? 27.5 : pow(2.0, (-69.0 / 12.0)) * 440.0;

	// 21 = A0
	// 0 = 0
	uint32_t seedMIDINote = octaveTables ? 21 : 0; 
	
	// --- create the tables
	for (int j = 0; j < numTables; j++)
	{
		// --- create new buffer
		double* tableAccumulator = new double[kDefaultWaveTableLength];
		memset(tableAccumulator, 0, kDefaultWaveTableLength * sizeof(double));

		int numHarmonics = (int)((sampleRate / 2.0 / seedFreq) - 1.0);

		double maxTableValue = 0;

		for (int i = 0; i < kDefaultWaveTableLength; i++)
		{
			for (int g = 1; g <= numHarmonics; g++)
			{
				// --- Parabola Waveform
				double n = double(g);
				tableAccumulator[i] += (1.0 / (n*n))*cos(2.0*kPi*i*n / kDefaultWaveTableLength);
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
		for (int i = 0; i < kDefaultWaveTableLength; i++)
			tableAccumulator[i] /= maxTableValue;

		// --- store
		parabolaTables[seedMIDINote] = tableAccumulator;

		// --- next table is one octave up
		if (octaveTables)
		{
			seedFreq *= 2.0;
			seedMIDINote += 12;
		}
		else
		{
			seedFreq *= pow(2.0, (1.0 / 12.0));
			seedMIDINote += 1;
		}
	}

	if(!octaveTables)
		return true;

	// --- now replicate the table pointers
	// now comb the array and replicate pointers
	int nLastIndex = -1;
	int numWaveTables = 128;

	double* pLastTable = NULL;
	for (int i = 0; i<128; i++)
	{
		if (parabolaTables[i])
		{
			nLastIndex = i;
			pLastTable = parabolaTables[i];
		}
	}

	if (!pLastTable)
		return false;// no tables : (

	// upper part first
	for (int i = 127; i >= nLastIndex; i--)
		parabolaTables[i] = pLastTable;

	int index = nLastIndex - 1; // first index already has value in it
	bool bWorking = index >= 0 ? true : false;

	while (bWorking)
	{
		if (!parabolaTables[index])
			parabolaTables[index] = pLastTable;
		else
			pLastTable = parabolaTables[index];

		index--;

		if (index < 0)
			bWorking = false;
	}

	return true;
}

void WaveTableData::destroyWaveTables()
{
	double* pTable = nullptr;
	double* pDeletedTable = nullptr;
	for (int i = 0; i<128; i++)
	{
		if (parabolaTables[i])
		{
			pTable = parabolaTables[i];
			if (pTable != pDeletedTable)
			{
				pDeletedTable = pTable;
				delete pTable;
			}
		}
	}

	for (int i = 0; i < 128; i++)
		parabolaTables[i] = nullptr;
}

int WaveTableData::getTableIndex(double oscillatorFrequency, SynthOscWaveform oscillatorWaveform)
{
	if (oscillatorWaveform == SynthOscWaveform::kSin)
		return -1;

	double seedFreq = 27.5; // Note A0, bottom of piano
	for (int j = 0; j < kNumWaveTables; j++)
	{
		if (oscillatorFrequency <= seedFreq)
			return j;

		// --- next octave
		seedFreq *= 2.0;
	}

	// --- this means the frequency was outside the limit and is a flag to use the sine table
	return -1;
}

const double* WaveTableData::selectTable(int oscillatorWaveform, /* selector: note it is *int* so can use with strongly typed enums */
											uint32_t midiNoteNumber,
											double sampleRate, 
											uint32_t& tableLength)
{
	SynthOscWaveform waveform = convertIntToEnum(oscillatorWaveform, SynthOscWaveform);
	const double* currentWaveTable = nullptr;
	double outputComp = 1.0;
	tableLength = getTableLength(oscillatorWaveform, midiNoteNumber);

	if (waveform == SynthOscWaveform::kAnalogSaw_WT)
		currentWaveTable = analogsaw[midiNoteNumber];
	else if (waveform == SynthOscWaveform::kParabola)
		currentWaveTable = parabolaTables[midiNoteNumber];
	else if (waveform == SynthOscWaveform::kSin)
		currentWaveTable = &sin_n1024[0];
	
	return currentWaveTable;
}

uint32_t WaveTableData::getTableLength(int oscillatorWaveform, uint32_t midiNoteNumber)
{
	uint32_t length = kDefaultWaveTableLength;
	SynthOscWaveform waveform = convertIntToEnum(oscillatorWaveform, SynthOscWaveform);

	if (waveform == SynthOscWaveform::kSin)
		length = 1024;
	else
		length = kDefaultWaveTableLength;

	return length;
}

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
	oscillatorFrequency = midiNotePitch*pitchShift*parameters->fmRatio;
	oscillatorFrequencySlaveOsc = oscillatorFrequency*parameters->hardSyncRatio;

	// --- BOUND the value to our range - in theory, we would bound this to any NYQUIST
	boundValue(oscillatorFrequency, 0.0, sampleRate / 2.0);
	boundValue(oscillatorFrequencySlaveOsc, 0.0, sampleRate / 2.0);
	
	// --- find the midi note closest to the pitch to select the wavetable
	renderMidiNoteNumber = midiNoteNumberFromOscFrequency(oscillatorFrequency);

	// --- calculate phase inc; this uses FINAL oscFrequency variable above
	unsigned int tableLength = waveTableData->getTableLength(enumToInt(parameters->oscillatorWaveform), renderMidiNoteNumber);
	phaseInc = calculateWaveTablePhaseInc(oscillatorFrequency, sampleRate, tableLength);

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
	uint32_t tableLength = 256;
	const double* currentWaveTable = waveTableData->selectTable(enumToInt(parameters->oscillatorWaveform), 
																renderMidiNoteNumber, sampleRate, tableLength);
	double outputComp = 1.0;
	if (parameters->oscillatorWaveform == SynthOscWaveform::kAnalogSaw_WT)
		outputComp = -1.9;

	// --- critical failure
	if (!currentWaveTable) return 0.0;

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
		double phaseModReadIndex = readIndex + phaseModulator * tableLength;

		// --- check for multi-wrapping on new read index
		checkAndWrapWaveTableIndex(phaseModReadIndex, tableLength);

		// --- do the table read operation
		output = readWaveTable(phaseModReadIndex, currentWaveTable, tableLength);
	}

	// --- increment index
	readIndex += _phaseInc;

	// --- adjust with final volume (amplitude)
	return outputComp * output;
}