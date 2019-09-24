#ifndef __wavetableOscillator_h__
#define __wavetableOscillator_h__

// --- includes
#include "synthdefs.h"

// --- wavetable objects and structs
#include "wavetable.h"

// --- .h file examples
#include "analog_square_1.h"


// --- enum for slots in 32-waveform vector (array)
enum wtWaveFormIndex {
	BANK_0_WAVE_0,		/* .h file example*/
	BANK_0_WAVE_1,		/* dynamic load from brick file */
	PARABOLIC_WAVE,
	TRIANGLE_WAVE,
	PURE_SINE_WAVE,

	NUM_ACTIVE_WAVEFORMS
}; // --- keep last

 // --- enum for slots in 32-waveform vector (array)
enum wtWaveBankIndex {
	SIK_TABLES,		/* only one bank for now */


	NUM_ACTIVE_BANKS
}; // --- keep last

// --- enum for slots in hiResWTableSets array
enum wtHiResWaveFormIndex {
	WCP_USER_SAW_0,
	NUM_HIRES_WAVEFORMS }; // --- keep last

// --- object that holds the table data; for sharing tables
class WaveTableBank : public IWaveBank
{

public:
	WaveTableBank()
	{
	}

	// --- add a new wavetable -NOTE: this must be done at the same (just after) time as construction
	inline void addWaveTable(Wavetable* wt)
	{
		// --- add to container
		wavetables.push_back(wt);
	}

	// ---- have tables destroy themselves
	virtual ~WaveTableBank()
	{
		for (int i = 0; i < MAX_NUM_OSC_WAVES; i++)
		{
			wavetables[i]->destroyWaveTables();
		}
	}

	// --- read a wavetable; this can vary considerably depending
	//     on how you implement the wavetable itself; here I just
	//     forward to the wavetable structure
	inline virtual double readWaveTable(double readIndex)
	{
		return selectedWT->readWaveTable(readIndex);
	}

	// --- IWaveTable
	virtual bool resetWaveTables(double sampleRate);

	// --- select a new table based on midi note and waveform
	//     NOTE: the MIDI note number reflects the pitch-modulated oscillator value and always rounds
	//           in the direction of NO aliasing (GUARANTEED)
	virtual uint32_t selectTable(int oscillatorWaveformIndex, uint32_t midiNoteNumber);

	// --- get the number of INITIALIZED waves for this datasource, 
	//     must be <= MAX_NUM_OSC_WAVES = 32
	virtual uint32_t getNumWaveforms() { return NUM_ACTIVE_WAVEFORMS; }

	// --- get the list of 32 strings
	virtual std::vector<std::string> getWaveformNames()
	{
		// --- load up a vector of strings
		std::vector<std::string> stringList;
		for (int i = 0; i < MAX_NUM_OSC_WAVES; i++) {
			stringList.push_back(wavetables[i]->waveformName);
		}

		return stringList;
	}

	// --- this is where you get the bank name, maybe for GUI
	virtual std::string getWaveBankName(){ return bankName; }
	virtual void setWaveBankName(std::string _bankName){ bankName = _bankName; }
	
	// --- create tables
	bool createBandLimitedTables(uint32_t waveform, uint32_t tableInterval, double* multiTable128[MAX_WAVE_TABLES], uint32_t tableLength, double sampleRate);

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

private:
	// --- name of this bank
	std::string bankName;

	// --- vector of wavetables
	std::vector<Wavetable*> wavetables;

	// --- currently selected wavetable
	Wavetable* selectedWT = nullptr;

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

// --- stores N sets of IWaveBanks 
class WaveTableData : public IWaveData
{
public:
	WaveTableData() 
	{
		// --- create wave banks here --- all construction is in this function
		WaveTableBank* bank_0 = new WaveTableBank;
		bank_0->setWaveBankName("SikTables");

		// --- this is where you initialize the 32 waveforms for this bank
		for (int i = 0; i < MAX_NUM_OSC_WAVES; i++)
		{
			Wavetable* wt = new Wavetable;
			switch (i)
			{
				case BANK_0_WAVE_0:
				{
					// --- example of the (favored) high-resolution wavetable set with .h file (factory ROM) table
					wt->initWithHiResWTSet(&analog_square_1TableSet);
					break;
				}
				case BANK_0_WAVE_1:
				{
					// --- example of the (favored) high-resolution wavetable set with brick file 
					wt->initWithBrickFile("C:\\Users\\chris\\Documents\\RAFx Plugins\\SynthCore\\PluginObjects\\wavetables\\analog_square_1.tbl");
					break;
				}
				case PARABOLIC_WAVE:
				{
					// --- example of a set of 1 or more tables (multi-tables) 
					//     spread across a set of 128 slots
					//     NOTE: these are sample-rate dependent so we don't have pointers for them yet
					wt->waveformName.assign("Parabolic");
					wt->tableType = wtTableType::kMultiTable;
					wt->outputComp = 1.0;
					break;
				}
				case TRIANGLE_WAVE:
				{
					// --- example of a set of 1 or more tables (multi-tables) 
					//     spread across a set of 128 slots
					//     NOTE: these are sample-rate dependent so we don't have pointers for them yet
					wt->waveformName.assign("Triangle");
					wt->tableType = wtTableType::kMultiTable;
					wt->outputComp = 1.0;
					break;
				}
				case PURE_SINE_WAVE:
				{
					// --- example of single table, burned into ROM at the top of this file
					wt->pdSingleTable = &sin_n1024[0];		// slot 2: 1024 pt sinusoid
					wt->waveformName.assign("Pure Sine");
					wt->tableType = wtTableType::kSingleTable;
					wt->tableLength = 1024;
					wt->outputComp = 1.0;
					break;
				}

				// --- add more here! do not exceed 32
				// . . . 
				default:
				{
					wt->waveformName.assign("----");
					wt->tableType = wtTableType::kUninitialized;
					wt->tableLength = kDefaultWaveTableLength; // 256! matches wave edit
					break;
				}
			}

			// --- add to bank_0 
			bank_0->addWaveTable(wt);
		}
	
		// --- add the bank to the container
		waveBanks.push_back(bank_0);

		// --- THIS IS WHERE YOU ADD MORE BANKS!!
		//     follow above procedure

	}

	~WaveTableData()
	{
		for (int i = 0; i < waveBanks.size(); i++)
		{
			WaveTableBank* wtBank = waveBanks[i];
			delete wtBank;
		}
		waveBanks.clear();
	}

	virtual bool resetWaveBanks(double sampleRate)
	{
		uint32_t bankCount = getNumWaveBanks();

		for (int i = 0; i <bankCount; i++) {
			waveBanks[i]->resetWaveTables(sampleRate);
		}

		return (bool)bankCount;
	}

	virtual IWaveBank* getInterface(uint32_t waveBankIndex)
	{
		if (waveBankIndex >= waveBanks.size())
			return nullptr;

		return waveBanks[waveBankIndex];
	}

	// --- get the number of waves for this datasource, must be <= MAX_NUM_OSC_WAVES = 32
	virtual uint32_t getNumWaveBanks() { return waveBanks.size(); }

	// --- returns the names of the waveforms, which are identical to the indexes of waveform selection on the GUI
	//     If there is no waveform, returns "" for that
	virtual std::vector<std::string> getWaveBankNames()
	{
		// --- load up a vector of strings
		std::vector<std::string> stringList;
		uint32_t bankCount = getNumWaveBanks();

		for (int i = 0; i <bankCount; i++) {
			stringList.push_back(waveBanks[i]->getWaveBankName());
		}

		return stringList;
	}

private:
	// --- vector of wavetables
	std::vector<WaveTableBank*> waveBanks;

};

/**
\class WaveTableOsc
\ingroup SynthClasses
\brief High_Accuracy wavetable oscillator with three waveforms: AK, FM and VS

\author Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 24
*/
class WaveTableOsc : public ISynthOscillator
{
public:
	WaveTableOsc(const std::shared_ptr<MidiInputData> _midiInputData, 
				 std::shared_ptr<SynthOscParameters> _parameters, 
				 std::shared_ptr<WaveTableData> _waveTableData);

	virtual ~WaveTableOsc();

	// --- ISynthOscillator
	virtual std::vector<std::string> getWaveformNames(uint32_t bankIndex);
	virtual bool reset(double _sampleRate);
	virtual bool update(bool updateAllModRoutings = true);
	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity);
	virtual bool setGlideModulation(uint32_t _startMIDINote, uint32_t _endMIDINote, double glideTime_mSec)
	{
		// --- start up the glide modulator
		return glideModulator.start(_startMIDINote, _endMIDINote, glideTime_mSec, sampleRate);
	}

	// --- main render function
	const OscillatorOutputData renderAudioOutput();

	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}

	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

protected:
	// --- MIDI Data Interface
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- modulators
	GlideModulator glideModulator;
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<SynthOscParameters> parameters = nullptr;

	// --- shared tables, in case they are huge or need a long creation time
	std::shared_ptr<WaveTableData> waveTableData = nullptr;

	// --- currently selected wavetable
	IWaveBank* selectedWaveBank = nullptr;

	// --- for anything
	double readWaveTable(double& readIndex, double _phaseInc);

	// --- the FINAL frequncy after all modulations
	double oscillatorFrequency = 440.0;
	double oscillatorFrequencySlaveOsc = 440.0;

	// --- the note number of the playing note
	uint32_t midiNoteNumber = 0;

	// --- midi note closest to FINAL pitch, but not over
	uint32_t renderMidiNoteNumber = 0;

	// --- the midi pitch, will need to save for portamento
	double midiNotePitch = 0.0;				///<the midi pitch, will need to save for portamento

	// --- our output
	OscillatorOutputData oscillatorAudioData;

	// --- timebase variables
	double modCounter = 0.0;						///<  VA modulo counter 0 to 1.0
	double phaseInc = 0.0;							///<  phase inc = fo/fs
	double sampleRate = 0.0;						///<  fs
	
	// --- WaveRable oscillator variables
	double waveTableReadIndex = 0.0;		///< wavetable read location
	uint32_t currentTableLength = kDefaultWaveTableLength;

	// --- flag indicating state (running or not)
	bool noteOn = false;

};


#endif /* defined(__wavetableOscillator_h__) */
