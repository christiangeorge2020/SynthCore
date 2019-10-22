#ifndef __wavetableBank_h__
#define __wavetableBank_h__

// --- includes
#include "synthdefs.h"

// --- wavetable objects and structs
#include "wavetable.h"

// --- max banks per plugin
// const uint32_t MAX_BANKS_PER_PLUGIN = 8;// 128;
									
// --- max banks per oscillator
const uint32_t MAX_BANKS_PER_OSCILLATOR = 8; // --- 4 factory + 4 user

// --- factory 
const uint32_t FACTORY_BANK_START = 0;
const uint32_t NUM_FACTORY_BANKS = 4;
const uint32_t FACTORY_BANK_END = FACTORY_BANK_START + NUM_FACTORY_BANKS - 1;

// --- user
const uint32_t USER_BANK_START = NUM_FACTORY_BANKS;
const uint32_t NUM_USER_BANKS = 4;
const uint32_t USER_BANK_END = USER_BANK_START + NUM_USER_BANKS - 1;

// --- not sure if needed
enum bankSet {
	BANK_SET_0, BANK_SET_1
};

// --- max tables per bank
const uint32_t MAX_TABLES_PER_BANK = 32;

// --- 32 sets-of-4
// const uint32_t MAX_NUM_BANK_SETS = MAX_BANKS_PER_PLUGIN / MAX_BANKS_PER_OSCILLATOR;

// --- bank sets 0 - 31
//     Each Bank Set is a group of 4 bank slots
//     BANK_SET_0 = 0, 1, 2, 3
//     BANK_SET_1 = 4, 5, 6, 7
//	   etc...


//enum bankSet {
//	BANK_SET_0, BANK_SET_1, BANK_SET_2, BANK_SET_3, BANK_SET_4, BANK_SET_5, BANK_SET_6, BANK_SET_7, BANK_SET_8, BANK_SET_9,
//	BANK_SET_20, BANK_SET_21, BANK_SET_22, BANK_SET_23, BANK_SET_24, BANK_SET_25, BANK_SET_26, BANK_SET_27, BANK_SET_28, BANK_SET_29,
//	BANK_SET_30, BANK_SET_31
//};

// --- for burnt in or on-the-fly calculated tables
enum wtWaveFormIndex { SINE_WAVE, PARABOLIC_WAVE, TRIANGLE_WAVE };

// --- helper to make x4 bank sets
inline uint32_t getBankIndex(uint32_t bankSet, uint32_t oscillatorBankIndex) { return (bankSet * MAX_BANKS_PER_OSCILLATOR) + oscillatorBankIndex; }

// --- object that holds the table data; for sharing tables
class WaveTableBank : public IWaveBank
{
public:
	WaveTableBank(){ }

	// --- add a new wavetable -NOTE: this must be done at the same (just after) time as construction
	inline void addWaveTable(Wavetable* wt){ wavetables.push_back(wt); }

	// --- from a .h File
	//     tableNames can be nullptr -- if so, the name is the default table name
	inline void initializeWithHiResWTBank(BankDescriptor bankDesc)
	{
		// --- add to container
		if(bankDesc.tablePtrsCount > MAX_TABLES_PER_BANK)
			bankDesc.tablePtrsCount = MAX_TABLES_PER_BANK;

		for (int i = 0; i < bankDesc.tablePtrsCount; i++)
		{
			Wavetable* wt = new Wavetable;
			wt->initWithHiResWTSet(bankDesc.tablePtrs[i]);
			
			// --- override the built-in table name
			//     NOTE: can be used for localization
			if (bankDesc.tableNames)
				wt->waveformName = bankDesc.tableNames[i];

			this->addWaveTable(wt);
		}

		// --- we are enabled!
		enabled = true;
	}

	// ---- have tables destroy themselves
	virtual ~WaveTableBank()
	{
		for (int i = 0; i < wavetables.size(); i++)
		{
			wavetables[i]->destroyWaveTables();
		}
	}

	// --- read a wavetable; this can vary considerably depending
	//     on how you implement the wavetable itself; here I just
	//     forward to the wavetable structure
	inline virtual double readWaveTable(IWaveTable* selectedWT, double readIndex)
	{
		if (selectedWT == nullptr) return 0.0;
		return selectedWT->readWaveTable(readIndex);
	}

	// --- IWaveTable
	virtual bool resetWaveTables(double sampleRate)
	{
		if (!enabled)
			return false;

		return true;
	}

	// --- select a new table based on midi note and waveform
	//     NOTE: the MIDI note number reflects the pitch-modulated oscillator value and always rounds
	//           in the direction of NO aliasing (GUARANTEED)
	virtual IWaveTable* selectTable(int oscillatorWaveformIndex, uint32_t midiNoteNumber, uint32_t& tableLen)
	{
		if (getNumWaveforms() <= 0)
			return 0;

		if (oscillatorWaveformIndex > getNumWaveforms() - 1)
			oscillatorWaveformIndex = getNumWaveforms() - 1;

		// --- tables are stored with oscillator waveform index
		IWaveTable* selectedWT = wavetables[oscillatorWaveformIndex];

		// --- access Wavetable structure via vector container as array notation []
		selectedWT->selectTable(midiNoteNumber);

		tableLen = selectedWT->getWaveTableLength();

		return selectedWT;
	}

	// --- get the number of INITIALIZED waves for this datasource,
	virtual uint32_t getNumWaveforms() { return wavetables.size(); }


	// --- get the list of 32 strings
	virtual std::vector<std::string> getWaveformNames()
	{
		int blankWavs = MAX_TABLES_PER_BANK - wavetables.size();
		// --- load up a vector of strings
		std::vector<std::string> stringList;
		for (int i = 0; i < wavetables.size(); i++) {
			stringList.push_back(wavetables[i]->waveformName);
		}
		for (int i = 0; i < blankWavs; i++) {
			stringList.push_back("-- empty --");
		}

		return stringList;
	}

	// --- this is where you get the bank name, maybe for GUI
	virtual std::string getWaveBankName(){ return bankName; }
	virtual void setWaveBankName(std::string _bankName){ bankName = _bankName; }

	// --- create tables
	bool createBandLimitedTables(uint32_t waveform, uint32_t tableInterval, double* multiTable128[MAX_WAVE_TABLES], uint32_t tableLength, double sampleRate)
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
					if (waveform == wtWaveFormIndex::PARABOLIC_WAVE) // --- others are based on all harmonics in sequence
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


	// --- generate a sample value based on the harmonic number; these will be accumulated
	//     in band limited tables.
	//     Parabola:
	double generateParabolaHarmonic(uint32_t sampleNum, double harmonicNum, uint32_t tableLength)
	{
		// --- prevent divide by 0; no DC component here
		if (harmonicNum == 0)
			return 0.0;

		return (1.0 / (harmonicNum*harmonicNum))*cos(2.0*kPi*sampleNum*harmonicNum / tableLength);
	}

	// --- generate a sample value based on the harmonic number; these will be accumulated
	//     in band limited tables.
	//     Triangle:
	double generateTriangleHarmonic(uint32_t sampleNum, double harmonicNum, uint32_t tableLength)
	{
		return pow(-1.0, harmonicNum)*(1.0 / pow((2.0 * harmonicNum + 1.0), (float)2.0))*sin(2.0*kPi*(2.0*harmonicNum + 1)*sampleNum / tableLength);
	}

	// --- add more mathematical table generation


	// --- state
	bool isEnabled() { return enabled; }
	void setEnabled(bool _enabled) { enabled = _enabled; }

private:
	// --- name of this bank
	std::string bankName;

	// --- vector of wavetables
	std::vector<Wavetable*> wavetables;

	// --- currently selected wavetable
	//Wavetable* selectedWT = nullptr;

	// --- state (for user loading)
	bool enabled = false;

	// --- helper
	inline uint32_t calculateNumTables(uint32_t seedMIDINote, uint32_t tableIntervalSemitones)
	{
		uint32_t count = 0;
		uint32_t nextMIDINote = seedMIDINote;
		if (seedMIDINote > 127) return 0;
		if (seedMIDINote == 127) return 1;
		while (nextMIDINote < 128)
		{
			nextMIDINote += tableIntervalSemitones;
			count++;
		}
		return count;
	}
};



#endif /* defined(__wavetableData_h__) */
