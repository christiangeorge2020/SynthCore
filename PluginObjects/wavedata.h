
#ifndef __wavedata_h__
#define __wavedata_h__

// This is a helper object for reading Wave files into floating point buffers.
// It is NOT optimized for speed (yet)
#if defined _WINDOWS || defined _WINDLL
//#include <windows.h>

// wave file parser
class WaveData
{
public:	// Functions
	//
	// One Time Initialization
	// pFilePath is the FULLY qualified file name + additional path info
	// VALID Examples: audio.wav
	//				   //samples//audio.wav
	WaveData(const char* pFilePath);

	// One Time Destruction
	~WaveData(void);

	unsigned int m_uNumChannels;
	unsigned int m_uSampleRate;
	unsigned int m_uSampleCount;
	unsigned int m_uLoopCount;
	unsigned int m_uLoopStartIndex;
	unsigned int m_uLoopEndIndex;
	unsigned int m_uLoopType;
	unsigned int m_uMIDINote;
	unsigned int m_uMIDIPitchFraction;
	unsigned int m_uSMPTEFormat;
	unsigned int m_uSMPTEOffset;

	bool m_bWaveLoaded;

	// --- the WAV file converted to floats on range of -1.0 --> +1.0
	float* m_pWaveBuffer = nullptr;

protected:
	bool readWaveFile(const char* pFilePath);
	void* m_hFile;
};

#else // MacOS Version here!

// wave file parser
class WaveData
{
public:	// Functions
	//
	// One Time Initialization
	// pFilePath is the FULLY qualified file name + additional path info
	// VALID Examples: audio.wav
	//				   //samples//audio.wav
	WaveData(const char* pFilePath);

	// One Time Destruction
	~WaveData(void);

	UINT m_uNumChannels;
	UINT m_uSampleRate;
	UINT m_uSampleCount;
	UINT m_uLoopCount;
	UINT m_uLoopStartIndex;
	UINT m_uLoopEndIndex;
	UINT m_uLoopType;
	UINT m_uMIDINote;
	UINT m_uMIDIPitchFraction;
	UINT m_uSMPTEFormat;
	UINT m_uSMPTEOffset;

	bool m_bWaveLoaded;

	// the WAV file converted to floats on range of -1.0 --> +1.0
	float* m_pWaveBuffer;

protected:
	bool readWaveFile(const char* pFilePath);
};

#endif

#endif