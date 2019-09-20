// WaveData Implementation ----------------------------------------------------------------
//
#include "wavedata.h"

#if defined _WINDOWS || defined _WINDLL
#include <windows.h>
#include <mmsystem.h>

typedef struct {

    UCHAR IdentifierString[4];
    DWORD dwLength;

} RIFF_CHUNK, *PRIFF_CHUNK;

typedef struct {

    WORD  wFormatTag;         // Format category
    WORD  wChannels;          // Number of channels
    DWORD dwSamplesPerSec;    // Sampling rate
    DWORD dwAvgBytesPerSec;   // For buffer estimation
    WORD  wBlockAlign;        // Data block size
    WORD  wBitsPerSample;


} WAVE_FILE_HEADER, *PWAVE_FILE_HEADER;


typedef struct _wave_sample {

     WAVEFORMATEX WaveFormatEx;
     char *pSampleData;
     char *pFXSampleData;
     UINT Index;
     UINT FXIndex;
     UINT Size;
     DWORD dwId;
     DWORD bPlaying;
     struct _wave_sample *pNext;

} WAVE_SAMPLE, *PWAVE_SAMPLE;

// union for data conversion
union UWaveData
{
	float f;
	double d;
	int n;
	unsigned int u;
	unsigned long long u64;
};

// WaveData
WaveData::WaveData(const char* pFilePath)
{
	m_bWaveLoaded = false;

	m_pWaveBuffer = NULL;
	m_uLoopCount = 0;
	m_uLoopStartIndex = 0;
	m_uLoopEndIndex = 0;
	m_uLoopType = 0;
	m_uMIDINote = 0;
	m_uMIDIPitchFraction = 0;
	m_uSMPTEFormat = 0;
	m_uSMPTEOffset = 0;

	if(pFilePath)
		m_bWaveLoaded = readWaveFile(pFilePath);
}

WaveData::~WaveData()
{
	if(m_pWaveBuffer)
		delete [] m_pWaveBuffer;

}

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// THE FOLLOWING TYPES ARE SUPPORTED:
//
// WAVE_FORMAT_PCM and WAVE_FORMAT_EXTENSIBLE
//
// 16-BIT Signed Integer PCM
// 24-BIT Signed Integer PCM 3-ByteAlign
// 24-BIT Signed Integer PCM 4-ByteAlign
// 32-BIT Signed Integer PCM
// 32-BIT Floating Point
// 64-BIT Floating Point
bool WaveData::readWaveFile(const char* pFilePath)
{
	bool bSampleLoaded = false;
    bool bFailed = false;
    RIFF_CHUNK RiffChunk = {{0}};
    WAVE_FILE_HEADER WaveFileHeader;
    DWORD dwIncrementBytes;
    WAVE_SAMPLE WaveSample;
	m_uNumChannels = 0;
	m_uSampleRate = 0;
	m_uSampleCount = 0;

    // --- create filestream from open binary file
    ifstream inFile;
    inFile.open(pFilePath, std::ifstream::binary);

    // --- OK?
    bool bIsOpen = inFile.is_open();

    if(!bIsOpen)
        return false;

    // --- this is used to identify each chunk
    char szIdentifier[5] = {0};

    // --- advance 12 chars
    inFile.seekg(12);

    // --- read first RiffChunk and the WaveFileHeader
    inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));
    inFile.read((char*)(&WaveFileHeader), sizeof(WaveFileHeader));

    // --- set the waveformatex
    WaveSample.WaveFormatEx.wFormatTag      = WaveFileHeader.wFormatTag;
    WaveSample.WaveFormatEx.nChannels       = WaveFileHeader.wChannels;
    WaveSample.WaveFormatEx.nSamplesPerSec  = WaveFileHeader.dwSamplesPerSec;
    WaveSample.WaveFormatEx.nAvgBytesPerSec = WaveFileHeader.dwAvgBytesPerSec;
    WaveSample.WaveFormatEx.nBlockAlign     = WaveFileHeader.wBlockAlign;
    WaveSample.WaveFormatEx.wBitsPerSample  = WaveFileHeader.wBitsPerSample;
    WaveSample.WaveFormatEx.cbSize          = 0;

    // --- I don't support these types (compressed, uLaw/aLaw, etc..)
    if(WaveSample.WaveFormatEx.wFormatTag != 1 && WaveSample.WaveFormatEx.wFormatTag != 3)
    {
        inFile.close();
        return false;
    }

    // --- for backing up the first seek
    dwIncrementBytes = sizeof(WaveFileHeader);

    do {
        // RiffChunk.dwLength - dwIncrementBytes sets the file pointer to position 0 first time through
        // advance by RiffChunk.dwLength - dwIncrementBytes
        int position = inFile.tellg();
        inFile.seekg(position + (RiffChunk.dwLength - dwIncrementBytes));

        // --- advanced past end of file?
        bFailed = inFile.fail();

        if(!bFailed)
        {
            // --- read the RiffChunk
            inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));

            // --- this now makes the seekg() advance in RiffChunk.dwLength chunks
            //     which vary with the type of chunk
            dwIncrementBytes = 0;

            // --- extract the chunk identifier
            memcpy(szIdentifier, RiffChunk.IdentifierString, 4);
        }

    } while(strcmp(szIdentifier, "data") && !bFailed);

    // --- AUDIO DATA CHUNK data
    // --- 16 bit
    if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 16)
    {
		WaveSample.pSampleData = (char *)LocalAlloc(LMEM_ZEROINIT, RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        if(m_pWaveBuffer)
            delete [] m_pWaveBuffer;

        m_pWaveBuffer = new float[nSampleCount];
        short* pShorts = new short[nSampleCount];
        memset(pShorts, 0, nSampleCount*sizeof(short));
        int m = 0;
        for(UINT i=0; i<nSampleCount; i++)
        {
            // MSB
            pShorts[i] = (unsigned char)WaveSample.pSampleData[m+1];
            pShorts[i] <<= 8;

            // LSB
            short lsb = (unsigned char)WaveSample.pSampleData[m];
            // in case top of lsb is bad
            lsb &= 0x00FF;
            pShorts[i] |= lsb;
            m+=2;
        }

        // convet to float -1.0 -> +1.0
        for(UINT i = 0; i < nSampleCount; i++)
        {
            m_pWaveBuffer[i] = ((float)pShorts[i])/32768.f;
        }

        delete [] pShorts;
        LocalFree(WaveSample.pSampleData);

        bSampleLoaded = true;
    }
    // --- 24 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 24)
    {
		WaveSample.pSampleData = (char *)LocalAlloc(LMEM_ZEROINIT, RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        if(m_pWaveBuffer)
            delete [] m_pWaveBuffer;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        int* pSignedInts = new int[nSampleCount];
        memset(pSignedInts, 0, nSampleCount*sizeof(long));

        int m = 0;
        int mask = 0x000000FF;

        // 24-bits in 3-byte packs
        if(WaveSample.WaveFormatEx.nBlockAlign/WaveSample.WaveFormatEx.nChannels == 3)
        {
            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+2];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                lsb <<= 8;
                pSignedInts[i] |= lsb;

                m+=3;
            }
        }

        // --- 24-bits in 4-byte packs
        if(WaveSample.WaveFormatEx.nBlockAlign/WaveSample.WaveFormatEx.nChannels == 4)
        {
            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // NSB2
                int nsb2 = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pSignedInts[i] |= nsb2;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pSignedInts[i] |= lsb;

                m+=4;
            }
        }

        // convet to float -1.0 -> +1.0
        for(UINT i = 0; i < nSampleCount; i++)
        {
            m_pWaveBuffer[i] = ((float)pSignedInts[i])/2147483648.0; // 2147483648.0 = 1/2 of 2^32
        }

        delete [] pSignedInts;
        LocalFree(WaveSample.pSampleData);

        bSampleLoaded = true;

    }
    // --- 32 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 32)
    {
		WaveSample.pSampleData = (char *)LocalAlloc(LMEM_ZEROINIT, RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        if(m_pWaveBuffer)
            delete [] m_pWaveBuffer;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        if(WaveSample.WaveFormatEx.wFormatTag == 1)
        {
            int* pSignedInts = new int[nSampleCount];
            memset(pSignedInts, 0, nSampleCount*sizeof(int));

            int m = 0;
            int mask = 0x000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // NSB2
                int nsb2 = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pSignedInts[i] |= nsb2;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pSignedInts[i] |= lsb;

                m+=4;
            }

            // convet to float -1.0 -> +1.0
            for(UINT i = 0; i < nSampleCount; i++)
            {
                m_pWaveBuffer[i] = ((float)pSignedInts[i])/2147483648.0; // 2147483648.0 = 1/2 of 2^32
            }

            delete [] pSignedInts;
            // free(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
        else if(WaveSample.WaveFormatEx.wFormatTag == 3) // float
        {
            unsigned int* pUSignedInts = new unsigned int[nSampleCount];
            memset(pUSignedInts, 0, nSampleCount*sizeof(unsigned int));

            int m = 0;
            int mask = 0x000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pUSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pUSignedInts[i] <<= 24;

                // NSB
                int nsb = (unsigned int)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pUSignedInts[i] |= nsb;

                // NSB2
                int nsb2 = (unsigned int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pUSignedInts[i] |= nsb2;

                // LSB
                int lsb = (unsigned int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pUSignedInts[i] |= lsb;

                m+=4;
            }

            // Use the Union trick to re-use same memory location as two different data types
            //
            // see: http://www.cplusplus.com/doc/tutorial/other_data_types/#union
            UWaveData wd;
            for(UINT i = 0; i < nSampleCount; i++)
            {
                // save uint version
                wd.u = pUSignedInts[i];
                m_pWaveBuffer[i] = wd.f;
            }

            delete []pUSignedInts;
            LocalFree(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
    }
    // --- 64 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 64)
    {
		WaveSample.pSampleData = (char *)LocalAlloc(LMEM_ZEROINIT, RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        if(m_pWaveBuffer)
            delete [] m_pWaveBuffer;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        // floating point only
        if(WaveSample.WaveFormatEx.wFormatTag == 3) // float
        {
            unsigned long long* pUSignedLongLongs = new unsigned long long[nSampleCount];
            memset(pUSignedLongLongs, 0, nSampleCount*sizeof(unsigned long long));

            int m = 0;
            unsigned long long mask = 0x00000000000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pUSignedLongLongs[i] = (unsigned char)WaveSample.pSampleData[m+7];
                pUSignedLongLongs[i] <<= 56;

                // NSB
                unsigned long long nsb = (unsigned long long)WaveSample.pSampleData[m+6];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 48;
                pUSignedLongLongs[i] |= nsb;

                // NSB2
                unsigned long long nsb2 = (unsigned long long)WaveSample.pSampleData[m+5];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 40;
                pUSignedLongLongs[i] |= nsb2;

                // NSB3
                unsigned long long nsb3 = (unsigned long long)WaveSample.pSampleData[m+4];
                // in case top of nsb is bad
                nsb3 &= mask;
                nsb3 <<= 32;
                pUSignedLongLongs[i] |= nsb3;

                // NSB4
                unsigned long long nsb4 = (unsigned long long)WaveSample.pSampleData[m+3];
                // in case top of nsb is bad
                nsb4 &= mask;
                nsb4 <<= 24;
                pUSignedLongLongs[i] |= nsb4;

                // NSB5
                unsigned long long nsb5 = (unsigned long long)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb5 &= mask;
                nsb5 <<= 16;
                pUSignedLongLongs[i] |= nsb5;

                // NSB6
                unsigned long long nsb6 = (unsigned long long)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb6 &= mask;
                nsb6 <<= 8;
                pUSignedLongLongs[i] |= nsb6;

                // LSB
                unsigned long long lsb = (unsigned long long)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pUSignedLongLongs[i] |= lsb;

                m+=8;
            }

            // Use the Union trick to re-use same memory location as two different data types
            //
            // see: http://www.cplusplus.com/doc/tutorial/other_data_types/#union
            UWaveData wd;
            for(UINT i = 0; i < nSampleCount; i++)
            {
                wd.u64 = pUSignedLongLongs[i];
                m_pWaveBuffer[i] = (float)wd.d; // cast the union's double as a float to chop off bottom
            }

            delete [] pUSignedLongLongs;
            LocalFree(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
    }

    // --- now find the loops, MIDI note info, etc... (smpl)
    int dInc = 0;
    do {
        int position = inFile.tellg();
		if(position < 0)
			bFailed = true;
		else
		{
			inFile.seekg(position + dInc);
			bFailed = inFile.fail();
		}

        if(!bFailed)
        {
            // --- read chunk, looking for 'smpl'
            inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));
            dInc = RiffChunk.dwLength;
            memcpy(szIdentifier, RiffChunk.IdentifierString, 4);
        }
    } while(strcmp(szIdentifier, "smpl") && !bFailed) ;

    // Found a smpl chunk
    /* smpl CHUNK format
     Offset	Size	Description			Value
     0x00	4	Chunk ID			"smpl" (0x736D706C)
     0x04	4	Chunk Data Size		36 + (Num Sample Loops * 24) + Sampler Data
     0x08	4	Manufacturer		0 - 0xFFFFFFFF	<------ // SKIPPING THIS //
     0x0C	4	Product				0 - 0xFFFFFFFF	<------ // SKIPPING THIS //
     0x10	4	Sample Period		0 - 0xFFFFFFFF	<------ // SKIPPING THIS (already know it)//
     0x14	4	MIDI Unity Note		0 - 127
     0x18	4	MIDI Pitch Fraction	0 - 0xFFFFFFFF
     0x1C	4	SMPTE Format		0, 24, 25, 29, 30
     0x20	4	SMPTE Offset		0 - 0xFFFFFFFF
     0x24	4	Num Sample Loops	0 - 0xFFFFFFFF
     0x28	4	Sampler Data		0 - 0xFFFFFFFF
     0x2C
     List of Sample Loops ------------------ //

     MIDI Unity Note
     The MIDI unity note value has the same meaning as the instrument chunk's MIDI Unshifted Note field which
     specifies the musical note at which the sample will be played at it's original sample rate
     (the sample rate specified in the format chunk).

     MIDI Pitch Fraction
     The MIDI pitch fraction specifies the fraction of a semitone up from the specified MIDI unity note field.
     A value of 0x80000000 means 1/2 semitone (50 cents) and a value of 0x00000000 means no fine tuning
     between semitones.

     SMPTE Format
     The SMPTE format specifies the Society of Motion Pictures and Television E time format used in the
     following SMPTE Offset field. If a value of 0 is set, SMPTE Offset should also be set to 0.
     Value	SMPTE Format
     0	no SMPTE offset
     24	24 frames per second
     25	25 frames per second
     29	30 frames per second with frame dropping (30 drop)
     30	30 frames per second

     SMPTE Offset
     The SMPTE Offset value specifies the time offset to be used for the synchronization / calibration
     to the first sample in the waveform. This value uses a format of 0xhhmmssff where hh is a signed value
     that specifies the number of hours (-23 to 23), mm is an unsigned value that specifies the
     number of minutes (0 to 59), ss is an unsigned value that specifies the number of seconds
     (0 to 59) and ff is an unsigned value that specifies the number of frames (0 to -1).

     // Sample Loop Data Struct
     Offset	Size	Description		Value
     0x00	4	Cue Point ID	0 - 0xFFFFFFFF
     0x04	4	Type			0 - 0xFFFFFFFF
     0x08	4	Start			0 - 0xFFFFFFFF
     0x0C	4	End				0 - 0xFFFFFFFF
     0x10	4	Fraction		0 - 0xFFFFFFFF
     0x14	4	Play Count		0 - 0xFFFFFFFF

     Loop type:
     Value	Loop Type
     0	Loop forward (normal)
     1	Alternating loop (forward/backward, also known as Ping Pong)
     2	Loop backward (reverse)
     3 - 31	Reserved for future standard types
     32 - 0xFFFFFFFF	Sampler specific types (defined by manufacturer)*/


    // skipping some stuff; could add later
    if(!bFailed)
    {
        dInc = 12; // 3 don't cares 4 bytes each
        m_uLoopType = 0;
        m_uLoopCount = 0;
        m_uLoopStartIndex = 0;
        m_uLoopEndIndex = 0;
        m_uMIDINote = 0;
        m_uMIDIPitchFraction = 0;
        m_uSMPTEFormat = 0;
        m_uSMPTEOffset = 0;

        // found a loop set; currently only taking the FIRST loop set
        // only need one loop set for potentially sustaining waves
        int position = inFile.tellg();
        inFile.seekg(position + dInc);

        // --- MIDI Note Number
        inFile.read((char*)(&m_uMIDINote), 4);

        // --- m_uMIDIPitchFraction
        inFile.read((char*)(&m_uMIDIPitchFraction), 4);

        // --- m_uSMPTEFormat
        inFile.read((char*)(&m_uSMPTEFormat), 4);

        // --- m_uSMPTEOffset
        inFile.read((char*)(&m_uSMPTEOffset), 4);

        // --- MIDI Note Number
        inFile.read((char*)(&m_uLoopCount), 4);

        // --- skip cuepoint & sampledata
        position = inFile.tellg();
        inFile.seekg(position + 8);

        // --- m_uLoopType
        inFile.read((char*)(&m_uLoopType), 4);

        // --- loop start sample
        inFile.read((char*)(&m_uLoopStartIndex), 4);
        m_uLoopStartIndex *= m_uNumChannels;

        // --- loop end sample
        inFile.read((char*)(&m_uLoopEndIndex), 4);
        m_uLoopEndIndex *= m_uNumChannels;
    }
    else  // no loops
    {
        m_uLoopType = 0;
        m_uLoopCount = 0;
        m_uLoopStartIndex = 0;
        m_uLoopEndIndex = 0;
        m_uMIDINote = 0;
        m_uMIDIPitchFraction = 0;
        m_uSMPTEFormat = 0;
        m_uSMPTEOffset = 0;
    }

    // --- close the file
    inFile.close();

    // --- hope we got one loaded!
    return bSampleLoaded;
}

#else
#include <iostream>
#include <fstream>
#include <iomanip>
#include "MacTypes.h"
using namespace std;

// union for data conversion
union UWaveData
{
	Float32 f;
	Float64 d;
	int n;
	UInt32 u;
	UInt64 u64;
};

typedef struct {
    UCHAR  IdentifierString[4];
    UInt32 dwLength;
} RIFF_CHUNK, *PRIFF_CHUNK;

typedef struct {
    UInt16  wFormatTag;         // Format category
    UInt16  wChannels;          // Number of channels
    UInt32  dwSamplesPerSec;    // Sampling rate
    UInt32  dwAvgBytesPerSec;   // For buffer estimation
    UInt16  wBlockAlign;        // Data block size
    UInt16  wBitsPerSample;
} WAVE_FILE_HEADER, *PWAVE_FILE_HEADER;

typedef struct
{
    UInt16        wFormatTag;         /* format type */
    UInt16        nChannels;          /* number of channels (i.e. mono, stereo...) */
    UInt32        nSamplesPerSec;     /* sample rate */
    UInt32        nAvgBytesPerSec;    /* for buffer estimation */
    UInt16        nBlockAlign;        /* block size of data */
    UInt16        wBitsPerSample;     /* number of bits per sample of mono data */
    UInt16        cbSize;             /* the count in bytes of the size of */
    /* extra information (after cbSize) */
} WAVEFORMATEX, *PWAVEFORMATEX;

typedef struct _wave_sample {
    WAVEFORMATEX    WaveFormatEx;
    char            *pSampleData;
    char            *pFXSampleData;
    UInt32          Index;
    UInt32          FXIndex;
    UInt32          Size;
    UInt32          dwId;
    UInt32          bPlaying;
    struct _wave_sample *pNext;

} WAVE_SAMPLE, *PWAVE_SAMPLE;



// WaveData
WaveData::WaveData(const char* pFilePath)
{
	m_bWaveLoaded = false;

	m_pWaveBuffer = NULL;
	m_uLoopCount = 0;
	m_uLoopStartIndex = 0;
	m_uLoopEndIndex = 0;
	m_uLoopType = 0;
	m_uMIDINote = 0;
	m_uMIDIPitchFraction = 0;
	m_uSMPTEFormat = 0;
	m_uSMPTEOffset = 0;

	if(pFilePath)
		m_bWaveLoaded = readWaveFile(pFilePath);
}

WaveData::~WaveData()
{
	if(m_pWaveBuffer)
		delete [] m_pWaveBuffer;

}

// --- prompts with file open dialog, returns TRUE if successfuly
//     opened and parsed the file into the member m_pWaveBuffer
//
/*
 For MacOS there is no easy way to do this in C++; you need NSOpenPanel (objective C)

 However, VSTGUI4 has a platform independent file open dialog CNewFileSelector that we can use.

RackAFX:
  -- You must install the VSTGUI4 main folder (named vstgui4) in your RackAFX projects folder (see the Advanced GUI Tutorials at www.willpirkle.com)
     The include is #include "../vstgui4/vstgui/vstgui.h"

VST3 MacOS:
 --- The VSTGUI4 library is included with the VST3 SDK
     The include is #include "vstgui4/vstgui/vstgui.h"

AU   MacOS:
 --- The VSTGUI4 library is included with the project
     The include is #include "vstgui4/vstgui/vstgui.h"

AAX   MacOS:
--- You must install the VSTGUI4 main folder (named vstgui4) in your ExamplePlugins folder (see www.willpirkle.com)
    The include is #include "vstgui/vstgui.h"

*/

/*#if defined DEVELOPMENT || defined RELEASE
#include "vstgui4/vstgui/vstgui.h" // VST3
#elif defined AAXPLUGIN
#include "vstgui/vstgui.h" // AAX
#elif defined AUPLUGIN
#include #include "vstgui4/vstgui/vstgui.h" // AU
#else
#include "../vstgui4/vstgui/vstgui.h" // RAFX
#endif*/


// THE FOLLOWING TYPES ARE SUPPORTED:
//
// WAVE_FORMAT_PCM and WAVE_FORMAT_EXTENSIBLE
//
// 16-BIT Signed Integer PCM
// 24-BIT Signed Integer PCM 3-ByteAlign
// 24-BIT Signed Integer PCM 4-ByteAlign
// 32-BIT Signed Integer PCM
// 32-BIT Floating Point
// 64-BIT Floating Point
bool WaveData::readWaveFile(const char* pFilePath)
{
    bool bSampleLoaded = false;
    bool bFailed = false;
    RIFF_CHUNK RiffChunk = {{0}};
    WAVE_FILE_HEADER WaveFileHeader;
    DWORD dwIncrementBytes;
    WAVE_SAMPLE WaveSample;
	m_uNumChannels = 0;
	m_uSampleRate = 0;
	m_uSampleCount = 0;

    // --- create filestream from open binary file
    ifstream inFile;
    inFile.open(pFilePath, std::ifstream::binary);

    // --- OK?
    bool bIsOpen = inFile.is_open();

    if(!bIsOpen)
        return false;

    // --- this is used to identify each chunk
    char szIdentifier[5] = {0};

    // --- advance 12 chars
    inFile.seekg(12);

    // --- read first RiffChunk and the WaveFileHeader
    inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));
    inFile.read((char*)(&WaveFileHeader), sizeof(WaveFileHeader));

    // --- set the waveformatex
    WaveSample.WaveFormatEx.wFormatTag      = WaveFileHeader.wFormatTag;
    WaveSample.WaveFormatEx.nChannels       = WaveFileHeader.wChannels;
    WaveSample.WaveFormatEx.nSamplesPerSec  = WaveFileHeader.dwSamplesPerSec;
    WaveSample.WaveFormatEx.nAvgBytesPerSec = WaveFileHeader.dwAvgBytesPerSec;
    WaveSample.WaveFormatEx.nBlockAlign     = WaveFileHeader.wBlockAlign;
    WaveSample.WaveFormatEx.wBitsPerSample  = WaveFileHeader.wBitsPerSample;
    WaveSample.WaveFormatEx.cbSize          = 0;

    // --- I don't support these types (compressed, uLaw/aLaw, etc..)
    if(WaveSample.WaveFormatEx.wFormatTag != 1 && WaveSample.WaveFormatEx.wFormatTag != 3)
    {
        inFile.close();
        return false;
    }

    // --- for backing up the first seek
    dwIncrementBytes = sizeof(WaveFileHeader);

    do {
        // RiffChunk.dwLength - dwIncrementBytes sets the file pointer to position 0 first time through
        // advance by RiffChunk.dwLength - dwIncrementBytes
        int position = inFile.tellg();
        inFile.seekg(position + (RiffChunk.dwLength - dwIncrementBytes));

        // --- advanced past end of file?
        bFailed = inFile.fail();

        if(!bFailed)
        {
            // --- read the RiffChunk
            inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));

            // --- this now makes the seekg() advance in RiffChunk.dwLength chunks
            //     which vary with the type of chunk
            dwIncrementBytes = 0;

            // --- extract the chunk identifier
            memcpy(szIdentifier, RiffChunk.IdentifierString, 4);
        }

    } while(strcmp(szIdentifier, "data") && !bFailed);

    if(m_pWaveBuffer)
        delete [] m_pWaveBuffer;

    // --- AUDIO DATA CHUNK data
    // --- 16 bit
    if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 16)
    {
        WaveSample.pSampleData = (char*)alloca(RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        if(m_pWaveBuffer)
            delete [] m_pWaveBuffer;

        m_pWaveBuffer = new float[nSampleCount];
        UInt16* pShorts = new UInt16[nSampleCount];

        memset(pShorts, 0, nSampleCount*sizeof(UInt16));
        int m = 0;
        for(UINT i=0; i<nSampleCount; i++)
        {
            // MSB
            pShorts[i] = (unsigned char)WaveSample.pSampleData[m+1];
            pShorts[i] <<= 8;

            // LSB
            UInt16 lsb = (unsigned char)WaveSample.pSampleData[m];
            // in case top of lsb is bad
            lsb &= 0x00FF;
            pShorts[i] |= lsb;
            m+=2;
        }

        // convert to float -1.0 -> +1.0
        for(UINT i = 0; i < nSampleCount; i++)
        {
            m_pWaveBuffer[i] = ((float)pShorts[i])/32768.0;
        }

        delete [] pShorts;
        // free(WaveSample.pSampleData);

        bSampleLoaded = true;
    }
    // --- 24 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 24)
    {
        WaveSample.pSampleData = (char*)alloca(RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        int* pSignedInts = new int[nSampleCount];
        memset(pSignedInts, 0, nSampleCount*sizeof(int));

        int m = 0;
        int mask = 0x000000FF;

        // 24-bits in 3-byte packs
        if(WaveSample.WaveFormatEx.nBlockAlign/WaveSample.WaveFormatEx.nChannels == 3)
        {
            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+2];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                lsb <<= 8;
                pSignedInts[i] |= lsb;

                m+=3;
            }
        }

        // --- 24-bits in 4-byte packs
        if(WaveSample.WaveFormatEx.nBlockAlign/WaveSample.WaveFormatEx.nChannels == 4)
        {
            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // NSB2
                int nsb2 = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pSignedInts[i] |= nsb2;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pSignedInts[i] |= lsb;

                m+=4;
            }
        }

        // convet to float -1.0 -> +1.0
        for(UINT i = 0; i < nSampleCount; i++)
        {
            m_pWaveBuffer[i] = ((float)pSignedInts[i])/2147483648.0; // 2147483648.0 = 1/2 of 2^32
        }

        delete [] pSignedInts;
        //// free(WaveSample.pSampleData);

        bSampleLoaded = true;

    }
    // --- 32 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 32)
    {
        WaveSample.pSampleData = (char*)alloca(RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        if(WaveSample.WaveFormatEx.wFormatTag == 1)
        {
            int* pSignedInts = new int[nSampleCount];
            memset(pSignedInts, 0, nSampleCount*sizeof(int));

            int m = 0;
            int mask = 0x000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pSignedInts[i] <<= 24;

                // NSB
                int nsb = (int)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pSignedInts[i] |= nsb;

                // NSB2
                int nsb2 = (int)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pSignedInts[i] |= nsb2;

                // LSB
                int lsb = (int)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pSignedInts[i] |= lsb;

                m+=4;
            }

            // convet to float -1.0 -> +1.0
            for(UINT i = 0; i < nSampleCount; i++)
            {
                m_pWaveBuffer[i] = ((float)pSignedInts[i])/2147483648.0; // 2147483648.0 = 1/2 of 2^32
            }

            delete [] pSignedInts;
            // free(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
        else if(WaveSample.WaveFormatEx.wFormatTag == 3) // float
        {
            UInt32* pUSignedInts = new UInt32[nSampleCount];
            memset(pUSignedInts, 0, nSampleCount*sizeof(UInt32));

            int m = 0;
            int mask = 0x000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pUSignedInts[i] = (unsigned char)WaveSample.pSampleData[m+3];
                pUSignedInts[i] <<= 24;

                // NSB
                UInt32 nsb = (UInt32)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 16;
                pUSignedInts[i] |= nsb;

                // NSB2
                UInt32 nsb2 = (UInt32)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 8;
                pUSignedInts[i] |= nsb2;

                // LSB
                UInt32 lsb = (UInt32)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pUSignedInts[i] |= lsb;

                m+=4;
            }

            // --- Use the Union trick to re-use same memory location as two different data types
            UWaveData wd;
            for(UINT i = 0; i < nSampleCount; i++)
            {
                // save uint version
                wd.u = pUSignedInts[i];
                m_pWaveBuffer[i] = wd.f;
            }

            delete []pUSignedInts;
            // free(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
    }
    // --- 64 bits
    else if(!bFailed && WaveSample.WaveFormatEx.wBitsPerSample == 64)
    {
        WaveSample.pSampleData = (char*)alloca(RiffChunk.dwLength);
        WaveSample.Size = RiffChunk.dwLength;

        inFile.read(WaveSample.pSampleData, RiffChunk.dwLength);

        UINT nSampleCount = (float)RiffChunk.dwLength/(float)(WaveSample.WaveFormatEx.wBitsPerSample/8.0);
        m_uSampleCount = nSampleCount;

        m_uNumChannels = WaveSample.WaveFormatEx.nChannels;
        m_uSampleRate = WaveSample.WaveFormatEx.nSamplesPerSec;

        // our buffer gets created
        m_pWaveBuffer = new float[nSampleCount];

        // floating point only
        if(WaveSample.WaveFormatEx.wFormatTag == 3) // float
        {
            UInt64* pUSignedLongLongs = new UInt64[nSampleCount];
            memset(pUSignedLongLongs, 0, nSampleCount*sizeof(UInt64));

            int m = 0;
            UInt64 mask = 0x00000000000000FF;

            for(UINT i=0; i<nSampleCount; i++)
            {
                // MSB
                pUSignedLongLongs[i] = (unsigned char)WaveSample.pSampleData[m+7];
                pUSignedLongLongs[i] <<= 56;

                // NSB
                UInt64 nsb = (UInt64)WaveSample.pSampleData[m+6];
                // in case top of nsb is bad
                nsb &= mask;
                nsb <<= 48;
                pUSignedLongLongs[i] |= nsb;

                // NSB2
                UInt64 nsb2 = (UInt64)WaveSample.pSampleData[m+5];
                // in case top of nsb is bad
                nsb2 &= mask;
                nsb2 <<= 40;
                pUSignedLongLongs[i] |= nsb2;

                // NSB3
                UInt64 nsb3 = (UInt64)WaveSample.pSampleData[m+4];
                // in case top of nsb is bad
                nsb3 &= mask;
                nsb3 <<= 32;
                pUSignedLongLongs[i] |= nsb3;

                // NSB4
                UInt64 nsb4 = (UInt64)WaveSample.pSampleData[m+3];
                // in case top of nsb is bad
                nsb4 &= mask;
                nsb4 <<= 24;
                pUSignedLongLongs[i] |= nsb4;

                // NSB5
                UInt64 nsb5 = (UInt64)WaveSample.pSampleData[m+2];
                // in case top of nsb is bad
                nsb5 &= mask;
                nsb5 <<= 16;
                pUSignedLongLongs[i] |= nsb5;

                // NSB6
                UInt64 nsb6 = (UInt64)WaveSample.pSampleData[m+1];
                // in case top of nsb is bad
                nsb6 &= mask;
                nsb6 <<= 8;
                pUSignedLongLongs[i] |= nsb6;

                // LSB
                UInt64 lsb = (UInt64)WaveSample.pSampleData[m];
                // in case top of lsb is bad
                lsb &= mask;
                pUSignedLongLongs[i] |= lsb;

                m+=8;
            }

            // Use the Union trick to re-use same memory location as two different data types
            UWaveData wd;
            for(UINT i = 0; i < nSampleCount; i++)
            {
                wd.u64 = pUSignedLongLongs[i];
                m_pWaveBuffer[i] = (float)wd.d; // cast the union's double as a float to chop off bottom
            }

            delete [] pUSignedLongLongs;
            // free(WaveSample.pSampleData);

            bSampleLoaded = true;
        }
    }

    // --- now find the loops, MIDI note info, etc... (smpl)
    int dInc = 0;
    do {
        int position = inFile.tellg();
		if(position < 0)
			bFailed = true;
		else
		{
			inFile.seekg(position + dInc);
			bFailed = inFile.fail();
		}

        if(!bFailed)
        {
            // --- read chunk, looking for 'smpl'
            inFile.read((char*)(&RiffChunk), sizeof(RiffChunk));
            dInc = RiffChunk.dwLength;
            memcpy(szIdentifier, RiffChunk.IdentifierString, 4);
        }
    } while(strcmp(szIdentifier, "smpl") && !bFailed) ;

    // Found a smpl chunk
    /* smpl CHUNK format
     Offset	Size	Description			Value
     0x00	4	Chunk ID			"smpl" (0x736D706C)
     0x04	4	Chunk Data Size		36 + (Num Sample Loops * 24) + Sampler Data
     0x08	4	Manufacturer		0 - 0xFFFFFFFF	<------ // SKIPPING THIS //
     0x0C	4	Product				0 - 0xFFFFFFFF	<------ // SKIPPING THIS //
     0x10	4	Sample Period		0 - 0xFFFFFFFF	<------ // SKIPPING THIS (already know it)//
     0x14	4	MIDI Unity Note		0 - 127
     0x18	4	MIDI Pitch Fraction	0 - 0xFFFFFFFF
     0x1C	4	SMPTE Format		0, 24, 25, 29, 30
     0x20	4	SMPTE Offset		0 - 0xFFFFFFFF
     0x24	4	Num Sample Loops	0 - 0xFFFFFFFF
     0x28	4	Sampler Data		0 - 0xFFFFFFFF
     0x2C
     List of Sample Loops ------------------ //

     MIDI Unity Note
     The MIDI unity note value has the same meaning as the instrument chunk's MIDI Unshifted Note field which
     specifies the musical note at which the sample will be played at it's original sample rate
     (the sample rate specified in the format chunk).

     MIDI Pitch Fraction
     The MIDI pitch fraction specifies the fraction of a semitone up from the specified MIDI unity note field.
     A value of 0x80000000 means 1/2 semitone (50 cents) and a value of 0x00000000 means no fine tuning
     between semitones.

     SMPTE Format
     The SMPTE format specifies the Society of Motion Pictures and Television E time format used in the
     following SMPTE Offset field. If a value of 0 is set, SMPTE Offset should also be set to 0.
     Value	SMPTE Format
     0	no SMPTE offset
     24	24 frames per second
     25	25 frames per second
     29	30 frames per second with frame dropping (30 drop)
     30	30 frames per second

     SMPTE Offset
     The SMPTE Offset value specifies the time offset to be used for the synchronization / calibration
     to the first sample in the waveform. This value uses a format of 0xhhmmssff where hh is a signed value
     that specifies the number of hours (-23 to 23), mm is an unsigned value that specifies the
     number of minutes (0 to 59), ss is an unsigned value that specifies the number of seconds
     (0 to 59) and ff is an unsigned value that specifies the number of frames (0 to -1).

     // Sample Loop Data Struct
     Offset	Size	Description		Value
     0x00	4	Cue Point ID	0 - 0xFFFFFFFF
     0x04	4	Type			0 - 0xFFFFFFFF
     0x08	4	Start			0 - 0xFFFFFFFF
     0x0C	4	End				0 - 0xFFFFFFFF
     0x10	4	Fraction		0 - 0xFFFFFFFF
     0x14	4	Play Count		0 - 0xFFFFFFFF

     Loop type:
     Value	Loop Type
     0	Loop forward (normal)
     1	Alternating loop (forward/backward, also known as Ping Pong)
     2	Loop backward (reverse)
     3 - 31	Reserved for future standard types
     32 - 0xFFFFFFFF	Sampler specific types (defined by manufacturer)*/


    // skipping some stuff; could add later
    if(!bFailed)
    {
        dInc = 12; // 3 don't cares 4 bytes each
        m_uLoopType = 0;
        m_uLoopCount = 0;
        m_uLoopStartIndex = 0;
        m_uLoopEndIndex = 0;
        m_uMIDINote = 0;
        m_uMIDIPitchFraction = 0;
        m_uSMPTEFormat = 0;
        m_uSMPTEOffset = 0;

        // found a loop set; currently only taking the FIRST loop set
        // only need one loop set for potentially sustaining waves
        int position = inFile.tellg();
        inFile.seekg(position + dInc);

        // --- MIDI Note Number
        inFile.read((char*)(&m_uMIDINote), 4);

        // --- m_uMIDIPitchFraction
        inFile.read((char*)(&m_uMIDIPitchFraction), 4);

        // --- m_uSMPTEFormat
        inFile.read((char*)(&m_uSMPTEFormat), 4);

        // --- m_uSMPTEOffset
        inFile.read((char*)(&m_uSMPTEOffset), 4);

        // --- MIDI Note Number
        inFile.read((char*)(&m_uLoopCount), 4);

        // --- skip cuepoint & sampledata
        position = inFile.tellg();
        inFile.seekg(position + 8);

        // --- m_uLoopType
        inFile.read((char*)(&m_uLoopType), 4);

        // --- loop start sample
        inFile.read((char*)(&m_uLoopStartIndex), 4);
        m_uLoopStartIndex *= m_uNumChannels;

        // --- loop end sample
        inFile.read((char*)(&m_uLoopEndIndex), 4);
        m_uLoopEndIndex *= m_uNumChannels;
    }
    else  // no loops
    {
        m_uLoopType = 0;
        m_uLoopCount = 0;
        m_uLoopStartIndex = 0;
        m_uLoopEndIndex = 0;
        m_uMIDINote = 0;
        m_uMIDIPitchFraction = 0;
        m_uSMPTEFormat = 0;
        m_uSMPTEOffset = 0;
    }

    // --- close the file
    inFile.close();

    // --- hope we got one loaded!
    return bSampleLoaded;
}
#endif
