#ifndef __synthCore_h__
#define __synthCore_h__

// --- we need these
#include "synthdefs.h"
#include "fxobjects.h"

// --- contains wavetable object
#include "synthoscillator.h" 
#include "vafilters.h"
#include "synthlfo.h"
#include "dca_eg.h"

#include <array>

/**
\enum modSource
\ingroup SynthStructures
\brief Enumeration for accessing an array of modulator source (read from here)

*/
enum modSource
{
	// --- LFOs here
	kLFO1_Normal,
	kLFO1_QuadPhase,

	// --- EGs here
	kEG1_Normal,	// EG1 = amp EG
	kEG1_Biased,

	// --- oscillators here
	kOsc1_Normal, // osc 1 output

	// --- other modulators (e.g. filter output) here


	// --- remain last, will always be the size of modulator array
	kNumModSources
};

/**
\enum modDestination
\ingroup SynthStructures
\brief Enumeration for accessing an array of modulator destination locations (write to here)

*/
enum modDestination
{
	// --- oscillator pitch (add more here)
	kOsc1_fo,

	// --- FILTER (add more here)
	kFilter1_fc, // Fc

	// --- DCA (add more here)
	kDCA_EGMod, // EG Input
	kDCA_AmpMod,// Amp Mod Input

	// --- remain last, will always be the size of modulator array
	kNumModDestinations
};


/**
\struct SynthVoiceParameters
\ingroup SynthStructures
\brief Contains voice-level modifiers and component modifer pointers that are shared across all voices. A "modifier" is any variable that *may* be connected to a
GUI control, however modifiers are not required to be connected to anything and their default values are set in the structure.
*A nodifier may also be used by an outer container object to modify parameters externally!*

NOTE: any variable in this structure must be sharable across all voices; stuff that is particular to the
owning voice must be privately owned. For example see voiceIsRunning
*/
struct SynthVoiceParameters
{
	SynthVoiceParameters() {}

	SynthVoiceParameters& operator=(const SynthVoiceParameters& params)
	{
		if (this == &params)
			return *this;

		enablePortamento = params.enablePortamento;
		portamentoTime_mSec = params.portamentoTime_mSec;
		legatoMode = params.legatoMode;
		freeRunOscMode = params.freeRunOscMode;
		voiceUnisonDetune_Cents = params.voiceUnisonDetune_Cents;

		osc1Parameters = params.osc1Parameters;
		dcaParameters = params.dcaParameters;

		lfo1Parameters = params.lfo1Parameters;
		ampEGParameters = params.ampEGParameters;

		return *this;
	}

	// --- portamento (glide)
	bool enablePortamento = false;

	// --- glide time
	double portamentoTime_mSec = 0.0;

	// --- legato mode
	bool legatoMode = false;

	// --- freerun
	bool freeRunOscMode = false;

	// --- unison Detune - each voice will be detuned differently
	double voiceUnisonDetune_Cents = 0.0;

	// --- GUI CONTROL INTERFACE -------------------------------- //
	// --- pitched oscillators
	std::shared_ptr<SynthOscParameters> osc1Parameters = std::make_shared<SynthOscParameters>();

	// --- LFO oscillators
	std::shared_ptr<SynthLFOParameters> lfo1Parameters = std::make_shared<SynthLFOParameters>();

	// --- EGs
	std::shared_ptr<EGParameters> ampEGParameters = std::make_shared<EGParameters>();

	// --- DCA
	std::shared_ptr<DCAParameters> dcaParameters = std::make_shared<DCAParameters>();
};

struct ModSource
{
	ModSource() {}
	ModSource& operator=(const ModSource& params)
	{
		if (this == &params)
			return *this;

		masterIntensity = params.masterIntensity;

		return *this;
	}

	// --- fast clearing of array
	void clear() {
		masterIntensity = 1.0;
	}

	// --- intensity only 
	double masterIntensity = 1.0; // < one final intensity knob
};

struct ModDestination
{
	ModDestination() { clear(); }
	ModDestination& operator=(const ModDestination& params)
	{
		if (this == &params)
			return *this;

		memcpy(channelEnable, params.channelEnable, MAX_MODULATION_CHANNELS * sizeof(uint32_t));
		memcpy(channelIntensity, params.channelIntensity, MAX_MODULATION_CHANNELS * sizeof(double));
		
		memcpy(channelHardwire, params.channelHardwire, MAX_MODULATION_CHANNELS * sizeof(bool));
		memcpy(hardwireIntensity, params.hardwireIntensity, MAX_MODULATION_CHANNELS * sizeof(double));

		masterIntensity = params.masterIntensity;
		defautValue = params.defautValue;

		enableChannelIntensity = params.enableChannelIntensity;
		priorityModulation = params.priorityModulation;

		return *this;
	}

	// --- fast clearing of array
	void clear() {
		memset(channelEnable, 0, MAX_MODULATION_CHANNELS * sizeof(uint32_t));
		memset(channelIntensity, 0, MAX_MODULATION_CHANNELS * sizeof(double));
		memset(channelHardwire, 0, MAX_MODULATION_CHANNELS * sizeof(double));
		memset(hardwireIntensity, 1, MAX_MODULATION_CHANNELS * sizeof(bool));
		masterIntensity = 1.0;
	}

	// --- channel enable and intensity controls
	uint32_t channelEnable[MAX_MODULATION_CHANNELS] = { 0 };
	double channelIntensity[MAX_MODULATION_CHANNELS] = { 0.0 };	
	
	bool channelHardwire[MAX_MODULATION_CHANNELS] = { 0.0 };
	double hardwireIntensity[MAX_MODULATION_CHANNELS] = { 1.0 };

	// --- use separate intensities for each channel
	bool enableChannelIntensity = false;

	double masterIntensity = 1.0;		// < one final intensity knob
	double defautValue = 0.0;			// < to allow max down, etc...
	bool priorityModulation = false;	// < for high-priority, e.g. FM
};


// --- engine mode: poly, mono or unison
enum class voiceState { kNoteOnState, kNoteOffState };

/**
\class SynthVoice
\ingroup SynthClasses
\brief Encapsulates one voice: a set of components and a modulation matrix to connect them and all updating/rendring code


\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
class SynthVoice : public ISynthComponent
{
public:
	SynthVoice(const std::shared_ptr<MidiInputData> _midiInputData, 
				const std::shared_ptr<MidiOutputData> _midiOutputData, 
				std::shared_ptr<SynthVoiceParameters> _parameters, 
				std::shared_ptr<WaveTableData> _waveTableData);

	virtual ~SynthVoice();

	// --- ISynthComponent
	virtual bool reset(double _sampleRate);
	virtual const SynthRenderData renderAudioOutput();
	virtual bool processMIDIEvent(midiEvent& event);
	virtual bool initialize(PluginInfo pluginInfo);

	// --- function to populate waveform name GUI fields
	//     oscIndex is [0, 31]
	//     bankIndex is variable; can have as many banks as you want (for now)
	std::vector<std::string> getWaveformNames(uint32_t bankIndex, uint32_t oscIndex);

	// --- local handlers for stealing
	bool doNoteOn(midiEvent& event);
	bool doNoteOff(midiEvent& event);

	// --- specialized getters
	bool isVoiceActive() { return voiceIsRunning; }

	// --- voice state
	voiceState getVoiceState() { return voiceNoteState; }
	
	// --- timestamps for determining note age; public because containing object needs to manipulate them
	unsigned int getTimestamp() { return timestamp; }	///< get current timestamp, the higher the value, the older the voice has been running
	void incrementTimestamp() { timestamp++; }			///< increment timestamp when a new note is triggered
	void clearTimestamp() { timestamp = 0; }			///< reset timestamp after voice is turned off

	unsigned int getMIDINoteNumber() { return voiceMIDIEvent.midiData1; } // note is data byte 1, velocity is byte 2
	unsigned int getStealMIDINoteNumber() { return voiceStealMIDIEvent.midiData1; } // note is data byte 1, velocity is byte 2
	
	// --- mod matrix i/f
	void setModMatrixPtrs(std::shared_ptr<std::array<ModSource, kNumModSources>> _modSourceData,
		std::shared_ptr<std::array<ModDestination, kNumModDestinations>> _modDestinationData)
	{
		modSourceRows = _modSourceData;
		modDestinationColumns = _modDestinationData;
	}

	bool voiceIsStealing() { return stealPending; }

protected:
	// --- parameters
	std::shared_ptr<SynthVoiceParameters> parameters = nullptr;

	// --- interface pointer
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;
	const std::shared_ptr<MidiOutputData> midiOutputData = nullptr;

	// --- note message state
	voiceState voiceNoteState = voiceState::kNoteOffState;

	// --- MOD MATRIX
	//
	std::shared_ptr<std::array<ModSource, kNumModSources>> modSourceRows = { nullptr };
	std::shared_ptr<std::array<ModDestination, kNumModDestinations>> modDestinationColumns = { nullptr };

	// --- run the matrix
	void runModulationMatrix(bool updateAllModRoutings);

	// --- clear arrays
	void clearModMatrixArrays()
	{
		memset(&modSourceData[0], 0, sizeof(double*)*kNumModSources);
		memset(&modDestinationData[0], 0, sizeof(double*)*kNumModDestinations);
	}

	// --- wire up source and destination arrays
	void initModMatrix()
	{
		// --- wire the source array slots
		modSourceData[kLFO1_Normal] = &lfo1Output.modulationOutputs[kLFONormalOutput];
		modSourceData[kLFO1_QuadPhase] = &lfo1Output.modulationOutputs[kLFOQuadPhaseOutput];
		modSourceData[kEG1_Normal] = &ampEGOutput.modulationOutputs[kEGNormalOutput];
		modSourceData[kEG1_Biased] = &ampEGOutput.modulationOutputs[kEGBiasedOutput];

		// --- destinations
		modDestinationData[kOsc1_fo] = &(osc1->getModulators()->modulationInputs[kBipolarMod]);
		modDestinationData[kDCA_EGMod] = &(dca->getModulators()->modulationInputs[kEGMod]);
		modDestinationData[kDCA_AmpMod] = &(dca->getModulators()->modulationInputs[kMaxDownAmpMod]);
	}

	// --- arrays to hold source/destination
	double* modSourceData[kNumModSources] = { nullptr };
	double* modDestinationData[kNumModDestinations] = { nullptr };

	// --- mod source data: --- modulators ---
	ModOutputData lfo1Output;
	ModOutputData ampEGOutput;

	// --- mod source data: --- filter ---
	// --- ModOutputData filterEGOutput;

	// --- mod source data: --- oscillators ---
	OscillatorOutputData osc1Output;
	// --------------------------------------------------
	
	// --- per-voice stuff
	bool voiceIsRunning = false;
	midiEvent voiceMIDIEvent;

	// --- for voice stealing
	bool stealPending = false;
	midiEvent voiceStealMIDIEvent;

	// --- smart pointers to the oscillator objects
	std::unique_ptr<SynthOsc> osc1;

	// --- filters:
	// add here....

	// --- LFOs
	std::unique_ptr<SynthLFO> lfo1;

	// --- EGs
	std::unique_ptr<EnvelopeGenerator> ampEG;

	// --- DCA(s)
	std::unique_ptr<DCA> dca;

	// --- output data structure
	SynthRenderData synthOutputData;

	// --- voice timestamp, for knowing the age of a voice
	unsigned int timestamp = 0;						///<voice timestamp, for knowing the age of a voice

	// --- granularity counter
	uint32_t updateGranularity = 1;					///< number of sample invervals to wait between component updates
	int granularityCounter = -1;					///< the counter for gramular updating; -1 = update NOW

	// --- granularity counter and rollover updater
	bool needsComponentUpdate()		///< uses the granularity counter to generate component update timing; if the updateGranularity == 1, the components are updated on every sample interval
	{
		bool update = false;

		// --- always update on first render pass
		if (granularityCounter < 0)
		{
			update = true;
		}

		// --- check counter
		granularityCounter++;
		if (granularityCounter == updateGranularity)
		{
			granularityCounter = 0;
			update = true;
		}

		return update;
	}
};

// --- engine mode: poly, mono or unison
enum class synthMode { kPoly, kMono, kUnison };

/**
\struct SynthEngineParameters
\ingroup SynthStructures
\brief Contains modifiers for the Synth Engine component. A "modifier" is any variable that *may* be connected to a
GUI control, however modifiers are not required to be connected to anything and their default values are set in the structure.
*A nodifier may also be used by an outer container object to modify parameters externally!*
*/
struct SynthEngineParameters
{
	SynthEngineParameters() {}

	SynthEngineParameters& operator=(const SynthEngineParameters& params)	
	{
		if (this == &params)
			return *this;

		enableMIDINoteEvents = params.enableMIDINoteEvents;
		mode = params.mode;
		masterVolume_dB = params.masterVolume_dB;
		masterPitchBendSensCoarse = params.masterPitchBendSensCoarse;
		masterPitchBendSensFine = params.masterPitchBendSensFine;

		masterTuningCoarse = params.masterTuningCoarse;
		masterTuningFine = params.masterTuningFine;

		masterUnisonDetune_Cents = params.masterUnisonDetune_Cents;
	
		// --- important! 
		voiceParameters = params.voiceParameters;
		modSourceData = params.modSourceData;
		modDestinationData = params.modDestinationData;

		return *this;
	}

	// --- enable/disable keyboard (MIDI note event) input; when disabled, synth goes into manual mode (Will's VCS3)
	bool enableMIDINoteEvents = true;

	// --- global synth mode
	synthMode mode = synthMode::kMono;// kPoly;

	// --- global master volume control, controls each output DCA's master volume
	double masterVolume_dB = 0.0;

	// --- master pitch bend, in semitones and cents
	unsigned int masterPitchBendSensCoarse = 7; // --- this number is always positive (parse as +/- value) 7 semitones = perfect 5th
	unsigned int masterPitchBendSensFine = 0;	// --- this number is always positive (parse as +/- value) see MIDI RPN 00 00 (sensitivity) and 00 01 (fine tuning, cents -100/+100) and 00 02 (coarse tuning, semintones -63/+64)

	// --- these are actually really important, especially for non-western music styles and 
	//     eccentric electronic music composers too...
	int masterTuningCoarse = 0;		// --- (+/-) semitones, see MIDI spec
	int masterTuningFine = 0;		// --- (+/-) cents see MIDI spec

	// --- unison Detune - this is the max detuning value NOTE a standard (or RPN or NRPN) parameter :/
	double masterUnisonDetune_Cents = 0.0;

	// --- VOICE layer parameters
	std::shared_ptr<SynthVoiceParameters> voiceParameters = std::make_shared<SynthVoiceParameters>();

	// --- modulation matrix rows and columns
	std::shared_ptr<std::array<ModSource, kNumModSources>> modSourceData = std::make_shared<std::array<ModSource, kNumModSources>>();
	std::shared_ptr<std::array<ModDestination, kNumModDestinations>> modDestinationData = std::make_shared<std::array<ModDestination, kNumModDestinations>>();

	// --- helper functions for the mod matrix rows/cols
	void setMM_SourceMasterIntensity(uint32_t destination, double intensity)
	{
		modSourceData->at(destination).masterIntensity = intensity;
	}

	void setMM_ChannelEnable(uint32_t source, uint32_t destination, bool enable)
	{
		modDestinationData->at(destination).channelEnable[source] = enable;
	}

	void setMM_HardwireEnable(uint32_t source, uint32_t destination, bool enable)
	{
		modDestinationData->at(destination).channelHardwire[source] = enable;
	}

	void setMM_ChannelIntensity(uint32_t source, uint32_t destination, double intensity)
	{
		modDestinationData->at(destination).channelIntensity[source] = intensity;
	}

	void setMM_DestHardwireIntensity(uint32_t source, uint32_t destination, double intensity)
	{
		modDestinationData->at(destination).hardwireIntensity[source] = intensity;
	}

	void setMM_DestMasterIntensity(uint32_t destination, double intensity)
	{
		modDestinationData->at(destination).masterIntensity = intensity;
	}

	void setMM_DestDefaultValue(uint32_t destination, double default)
	{
		modDestinationData->at(destination).defautValue = default;
	}

	void setMM_DestHighPriority(uint32_t destination, bool _priorityModulation)
	{
		modDestinationData->at(destination).priorityModulation = _priorityModulation;
	}

	void setMM_HardwiredRouting(uint32_t source, uint32_t destination, double intensity = 1.0)
	{
		setMM_ChannelEnable(source, destination, true);
		setMM_HardwireEnable(source, destination, true);
		setMM_DestMasterIntensity(destination, intensity);
		setMM_DestHardwireIntensity(source, destination, intensity);
	}
};


/**
\class SynthEngine
\ingroup SynthClasses
\brief Encapsulates an entire synth engine, producing one type of sysnthesizer set of voices (e.g. Virtual Analog, Sample Based, FM, etc...) --
This object contains an array of SynthVoice objects to render audio and also processes MIDI events.

Outputs: contains 2 outputs
- Left Channel
- Right Channel

Control I/F:
Use SynthEngineModifiers structure; note that it's SynthVoiceModifiers pointer is shared across all voices

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
class SynthEngine : public ISynthComponent
{
public:
	SynthEngine();
	virtual ~SynthEngine();

	// --- ISynthEngine
	virtual bool reset(double _sampleRate);
	virtual const SynthRenderData renderAudioOutput();
	virtual bool processMIDIEvent(midiEvent& event);
	virtual bool initialize(PluginInfo pluginInfo);

	// --- get parameters
	SynthEngineParameters getParameters();

	// --- set parameters
	void setParameters(const SynthEngineParameters& _parameters);
	
	// --- helper function to get the array index of next available voice
	//     returns -1 if NO more voices are avaialable - time to steal a voice
	int getFreeVoiceIndex();
	int getVoiceIndexToSteal();

	// --- get a currently running voice with a specific number
	int getActiveVoiceIndexInNoteOn(unsigned int midiNoteNumber);
	int getStealingVoiceIndexInNoteOn(unsigned int midiNoteNumber);

	// --- helpers for populating oscillator waveform GUI controls
	//     for synths, these are usually the only dynamic items like this
	std::vector<std::string> getOscWaveformNames(uint32_t voiceIndex, uint32_t bankIndex, uint32_t oscillatorIndex);

protected:
	// --- our outputs, same number as synth voice!
	SynthRenderData synthOutputData;

	// --- our modifiers (parameters)
	SynthEngineParameters parameters;

	// --- shared MIDI tables, via IMIDIData
	std::shared_ptr<MidiInputData> midiInputData = std::make_shared<MidiInputData>();
	std::shared_ptr<MidiOutputData> midiOutputData = std::make_shared<MidiOutputData>();

	// --- array of voice object, via pointers
	std::unique_ptr<SynthVoice> synthVoices[MAX_VOICES] = { 0 };		///< array of voice objects for the engine

	// --- shared tables, in case they are huge or need a long creation time
	std::shared_ptr<WaveTableData> waveTableData = std::make_shared<WaveTableData>();

private:
	// --- ADD FX Here...

};


#endif /* defined(__synthCore_h__) */
