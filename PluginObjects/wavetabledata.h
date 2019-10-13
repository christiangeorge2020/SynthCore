#ifndef __wavetableData_h__
#define __wavetableData_h__

// --- includes
#include "synthdefs.h"

// --- wavetable objects and structs
#include "wavetablebank.h"

#include "wavetables\AKWF_0.h"
#include "wavetables\AKWF_1.h"

// --- stores MAX_BANKS_PER_PLUGIN sets of IWaveBanks (128)
//     NOTE: this is the ONE AND ONLY wavetable datasource for the entire synth
//           So, initialize everything here. 
class WaveTableData : public IWaveData
{
public:
	WaveTableData()
	{
		// --- create wave banks here --- all construction is in this function
		WaveTableBank* bank_0 = new WaveTableBank;
		bank_0->setWaveBankName("Sik Tables");

		// --- init from bank; must #include the .h file with these variables inside; they will be different for each table in the bank
		bank_0->initializeWithHiResWTBank(AKWF_0_BankDescriptor);

		// --- add the bank to the container
		waveBanks.push_back(bank_0);

		WaveTableBank* bank_1 = new WaveTableBank;
		bank_1->setWaveBankName("Found Tables");

		// --- init from bank; must #include the .h file with these variables inside; they will be different for each table in the bank
		bank_1->initializeWithHiResWTBank(AKWF_1_BankDescriptor);

		// --- add the bank to the container
		waveBanks.push_back(bank_1);

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
			WaveTableBank* wtBank = waveBanks[i];
			wtBank->resetWaveTables(sampleRate);
		}

		return (bool)bankCount;
	}

	virtual IWaveBank* getInterface(uint32_t waveBankIndex)
	{
		if (waveBankIndex >= waveBanks.size() - 1)
			waveBankIndex = waveBanks.size() - 1;

		if (waveBankIndex < 0)
			return nullptr; // should never happen; will crash

		return waveBanks[waveBankIndex];
	}

	// --- get the number of banks for this datasource
	virtual uint32_t getNumWaveBanks() { return waveBanks.size(); }

	// --- returns the names of the waveforms, which are identical to the indexes of waveform selection on the GUI
	//     If there is no waveform, returns "" for that
	virtual std::vector<std::string> getWaveBankNames(uint32_t bankSet = 0)
	{
		// --- load up a vector of strings
		std::vector<std::string> stringList;
		uint32_t bankCount = getNumWaveBanks();
		uint32_t start = bankSet*MAX_BANKS_PER_OSCILLATOR;

		for (int i = start; i <MAX_BANKS_PER_OSCILLATOR; i++)
		{
			if (i < bankCount)
				stringList.push_back(waveBanks[i]->getWaveBankName());
			else
				stringList.push_back("-- empty --");
		}

		return stringList;
	}

private:
	// --- vector of wavetables
	std::vector<WaveTableBank*> waveBanks;
};



#endif /* defined(__wavetableData_h__) */
