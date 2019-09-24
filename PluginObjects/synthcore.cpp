// --- Synth Core v1.0
//
#include "synthcore.h"
#include "trace.h" // for output logging

SynthVoice::SynthVoice(const std::shared_ptr<MidiInputData> _midiInputData, 
						const std::shared_ptr<MidiOutputData> _midiOutputData, 
						std::shared_ptr<SynthVoiceParameters> _parameters, 
						std::shared_ptr<WaveTableData> _waveTableData)
: ISynthComponent()
, midiInputData(_midiInputData)		//<- set our midi dat interface value
, midiOutputData(_midiOutputData)
, parameters(_parameters)	//<- set our parameters
{
	if (!midiInputData)
		;// --- throw exceptuion, etc...

	if (!_midiOutputData)
		;// --- throw exceptuion, etc...

	// --- should not happen, but if so, the first initialized voice creates it
	if (!parameters)
		parameters = std::make_shared<SynthVoiceParameters>();

	// --- create subcomponents
	osc1.reset(new SynthOsc(midiInputData, parameters->osc1Parameters, _waveTableData));
	
	lfo1.reset(new SynthLFO(midiInputData, parameters->lfo1Parameters));

	ampEG.reset(new EnvelopeGenerator(midiInputData, parameters->ampEGParameters));

	dca.reset(new DCA(midiInputData, parameters->dcaParameters));

}

SynthVoice::~SynthVoice()
{

}

std::vector<std::string> SynthVoice::getWaveformNames(uint32_t bankIndex, uint32_t oscIndex)
{
	return osc1->getWaveformNames(bankIndex);

}

// --- NOTE: parameters.dllFolderPath MUST be set before this call!!
bool SynthVoice::initialize(PluginInfo pluginInfo)
{
	// --- use this to setup morphing wave table osc
	// add your code here

	return true;
}

// --- ISynthComponent
bool SynthVoice::reset(double _sampleRate)
{
	// --- clear output array
	synthOutputData.clear();

	// --- reset sub objects
	osc1->reset(_sampleRate);

	lfo1->reset(_sampleRate);

	ampEG->reset(_sampleRate);

	dca->reset(_sampleRate);

	// --- reset grain count
	updateGranularity = 64; // update every 128 render-cycles
	granularityCounter = -1;

	// --- clear modulator output arrays
	lfo1Output.clear();
	ampEGOutput.clear();
	
	// --- filter EG goes here (add more)


	// --- init the source and destination array's of modulator pointers
	initModMatrix();

	return true;
}

// --- run the matrix
void SynthVoice::runModulationMatrix(bool updateAllModRoutings)
{
	// --- sources are bound via pointers
	//
	// --- loop over destination columns
	// --- now calcualate the column modulation value
	double modDestinationValue = 0.0;

	for (int col = 0; col < kNumModDestinations; col++)
	{
		ModDestination destination = modDestinationColumns->at(col);
		modDestinationValue = destination.defautValue;

		if (!updateAllModRoutings && !destination.priorityModulation)
			continue;

		for (int row = 0; row < kNumModSources; row++)
		{		
			if (!modSourceData[row] || destination.channelEnable[row] == 0)
				continue;
			        
			// --- get local modulation value 
			double modSourceValue = *modSourceData[row];

			ModSource source = modSourceRows->at(row);

			// --- scale it
			modSourceValue *= source.masterIntensity;

			// --- without channel intensity
			if (destination.channelHardwire[row])
				modDestinationValue += modSourceValue*destination.hardwireIntensity[row];
			else
				modDestinationValue += destination.masterIntensity*modSourceValue;// *destination.channelEnable[row]; // moved this to above rto save cpu
		}

		// --- write to the output array
		if (modDestinationData[col])
			*modDestinationData[col] = modDestinationValue;
	}
}

const SynthRenderData SynthVoice::renderAudioOutput()
{
	// --- run the granularity counter
	bool updateAllModRoutings = needsComponentUpdate();

	// --- update/render (add more here)
	lfo1->update(updateAllModRoutings);
	lfo1Output = lfo1->renderModulatorOutput();
	
	// --- update/render (add more here)
	ampEG->update(updateAllModRoutings);
	ampEGOutput = ampEG->renderModulatorOutput();

	// --- do all mods	
	runModulationMatrix(updateAllModRoutings);

	// --- update modulate-ees (add more here)
	osc1->update(updateAllModRoutings);
	dca->update(updateAllModRoutings);

	// --- render Oscillators (add more here)
	osc1Output = osc1->renderAudioOutput();

	// --- blend oscillator outputs
	double oscOut = osc1Output.outputs[0]; // +... add more oscillator outputs here

	// --- do the filtering
	// add more here

	// --- this voice is MONO up to this point
	SynthProcessorData audioData;
	audioData.numInputChannels = 1; // mono in
	audioData.numOutputChannels = 2;// stereo out
	audioData.inputs[0] = oscOut;
	
	// --- dca will make stereo and pan
	dca->processSynthAudio(&audioData);

	// --- check for note off condition
	if (voiceIsRunning)
	{
		if (ampEG->getState() == egState::kOff)
		{
			// --- check for steal pending
			if (stealPending)
			{
				// --- turn off old note event
				doNoteOff(voiceMIDIEvent);

				// --- load new note info
				voiceMIDIEvent = voiceStealMIDIEvent;

				// --- turn on the new note
				doNoteOn(voiceMIDIEvent);

				// --- stealing accomplished!
				stealPending = false;
			}
			else
				voiceIsRunning = false;
		}
	}

	// --- transfer int our output - note that this gives the voice a chance to 
	//     relocate, pan, modify, etc.. all of its oscillator outputs
	//
	// --- this voice generates two output channels
	synthOutputData.channelCount = 2;

	// --- summation for the simple core
	synthOutputData.synthOutputs[0] = audioData.outputs[0];
	synthOutputData.synthOutputs[1] = audioData.outputs[1];

	return synthOutputData;
}

bool SynthVoice::doNoteOn(midiEvent& event)
{
	// --- calculate MIDI -> pitch value
	double midiPitch = midiNoteNumberToOscFrequency(event.midiData1); 
	
	// --- OR lookup midiFreqTable[event.midiData1];
	//        double midiPitch = midiFreqTable[event.midiData1];

	// --- set glide modulation, using aux MIDI event data for this rather than adding more parameters
	if (event.auxUintData1 < 128)
	{
		osc1->setGlideModulation(event.auxUintData1, event.midiData1, parameters->portamentoTime_mSec);
	}
	
	// --- start oscillator
	osc1->doNoteOn(midiPitch, event.midiData1, event.midiData2);

	// --- send to dca
	dca->doNoteOn(midiPitch, event.midiData1, event.midiData2);

	//  EGs
	ampEG->doNoteOn(midiPitch, event.midiData1, event.midiData2);

	// --- needed forLFO  modes
	lfo1->doNoteOn(midiPitch, event.midiData1, event.midiData2);

	// --- set the flag
	voiceIsRunning = true; // we are ON
	granularityCounter = -1; // reset to -1
	voiceNoteState = voiceState::kNoteOnState;

	// --- this saves the midi note number and velocity so that we can identify our own note
	voiceMIDIEvent = event;
	return true;
}

bool SynthVoice::doNoteOff(midiEvent& event)
{
	// --- lookup MIDI -> pitch value
	double midiPitch = midiFreqTable[event.midiData1];

	// --- stop the oscillators (for now... HINT: put EG into Release Phase, then stop oscillator after EG expires)
	ampEG->doNoteOff(midiPitch, event.midiData1, event.midiData2);

	// --- set our current state; the ampEG will determine the final state
	voiceNoteState = voiceState::kNoteOffState;
	return true;
}

bool SynthVoice::processMIDIEvent(midiEvent& event)
{
	// --- the voice only needs to process note on and off
	//     Other MIDI info such as CC can be found in global midi table via our midiData interface
	if (event.midiMessage == NOTE_ON)
	{
		// --- clear timestamp
		clearTimestamp();

		// --- detect if we are being stolen:
		if (isVoiceActive())
		{
			// --- save information
			voiceStealMIDIEvent = event;

			// --- set amp EG into shutdown mode
			ampEG->shutdown();

			// --- set the steal flag; see start of render operation for the rest of the code
			stealPending = true; 
			return true;
		}

		// --- call the subfunction
		doNoteOn(event);
	}
	else if (event.midiMessage == NOTE_OFF)
	{
		// --- call the subfunction
		doNoteOff(event);
	}

	return true;
}

SynthEngine::SynthEngine()
{
	// --- this is recommended by the MIDI Manufacturer's Asssociation to force a reset
	//     so that the state is known
	midiInputData->globalMIDIData[kCurrentMIDINoteNumber] = 128;	// --- 128 = not set (yet)
	midiInputData->globalMIDIData[kLastMIDINoteNumber] = 128;		// --- 128 = not set (yet)

	midiInputData->globalMIDIData[kMIDIPitchBendDataLSB] = 0;		// --- this is for 0 pitchbend at startup
	midiInputData->globalMIDIData[kMIDIPitchBendDataMSB] = 64;		// --- this is for 0 pitchbend at startup

	midiInputData->globalMIDIData[kMIDIMasterPBSensCoarse] = 1;		// --- this is for 0 kMIDIMasterPitchBendCoarse at startup
	midiInputData->globalMIDIData[kMIDIMasterPBSensFine] = 0;		// --- this is for 0 kMIDIMasterPitchBendFine at startup

	midiInputData->globalMIDIData[kMIDIMasterTuneCoarseLSB] = 0;	// --- this is for 0 kMIDIMasterPitchBendCoarse at startup
	midiInputData->globalMIDIData[kMIDIMasterTuneCoarseMSB] = 65;	// --- this is for 0 kMIDIMasterPitchBendFine at startup

	midiInputData->globalMIDIData[kMIDIMasterTuneFineLSB] = 0;	// --- this is for 0 kMIDIMasterPitchBendCoarse at startup
	midiInputData->globalMIDIData[kMIDIMasterTuneFineMSB] = 65;	// --- this is for 0 kMIDIMasterPitchBendFine at startup

	midiInputData->globalMIDIData[kMIDIMasterVolumeLSB] = 0;	// --- this is for 0 kMIDIMasterPitchBendCoarse at startup
	midiInputData->globalMIDIData[kMIDIMasterVolumeMSB] = 65;	// --- this is for 0 kMIDIMasterPitchBendFine at startup

	// --- volume/pan need to be preset so synth is not accidentally silent
	// --- CCs
	midiInputData->ccMIDIData[VOLUME_CC07] = 127;	// --- MIDI VOLUME; default this to ON
	midiInputData->ccMIDIData[PAN_CC10] = 64;		// --- MIDI PAN; default this to CENTER

	// --- HARDWIRED MOD ROUTINGS --- //
	//
	// --- kEG1_Normal -> kDCA_EGMod
	parameters.setMM_HardwiredRouting(kEG1_Normal, kDCA_EGMod);

	// --- example of another hardwired routing
	//parameters.setMM_HardwiredRouting(kLFO1_Normal, kOsc1_fo);

	// --- EG2 -> Filter 1 (and 2) Fc ??

	// --- EG3 -> Wave Morph??

	// --- set amp mod default value to prevent silence accidentally
	parameters.setMM_DestDefaultValue(kDCA_AmpMod, 1.0);

	// --- create the smart pointers
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		// --- reset is the constructor for this kind of smartpointer
		//
		//     Pass our this pointer for the IMIDIData interface - safe
		synthVoices[i].reset(new SynthVoice(midiInputData, midiOutputData, parameters.voiceParameters, waveTableData));

		// --- pass safe pointer to voices to share the common matrix core
		synthVoices[i]->setModMatrixPtrs(parameters.modSourceData, parameters.modDestinationData);
	}
}

/**
\brief Destroy all modifiers and voices that were allocated in constructor
*/
SynthEngine::~SynthEngine()
{
}

bool SynthEngine::reset(double _sampleRate)
{
	// --- set new table data
	waveTableData->resetWaveBanks(_sampleRate);

	// --- reset array of voices
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		// --- smart poitner access looks normal (->) 
		synthVoices[i]->reset(_sampleRate); // this calls reset() on the smart-pointers underlying naked pointer
	}

	// --- create FX
	// add more here

	return true;
}

bool SynthEngine::initialize(PluginInfo pluginInfo)
{ 
	// --- parse wavesamples
	std::string targetFolder = pluginInfo.pathToDLL;

	// --- loop
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		// --- init
		synthVoices[i]->initialize(pluginInfo);
		
		// --- do other inits here

	}

	return true;
}

std::vector<std::string> SynthEngine::getOscWaveformNames(uint32_t voiceIndex, uint32_t bankIndex, uint32_t oscillatorIndex)
{
	std::vector<std::string> emptyVector;
	//  voiceIndex is usually just 0, unless you have mixed-mode voices
	if (voiceIndex >= MAX_VOICES)
		return emptyVector;

	return synthVoices[voiceIndex]->getWaveformNames(bankIndex, oscillatorIndex);
}

const SynthRenderData SynthEngine::renderAudioOutput()
{
	// --- clear accumumlators
	synthOutputData.clear();

	// --- temp output of each voicem to be accumuluated in our main synthOutputData
	SynthRenderData voiceRender;

	// --- -12dB per active channel to avoid clipping
	double gainFactor = 0.25; 
	if (parameters.mode == synthMode::kUnison)
		gainFactor = 0.125;

	// --- loop through voices and render/accumulate them
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		// --- blend active voices
		if (synthVoices[i]->isVoiceActive())
		{
			// --- reset array to 0 in case of horrible failure (shreiking noises, etc...)
			voiceRender.clear();

			// --- render the voice
			voiceRender = synthVoices[i]->renderAudioOutput();

			// --- accumulate results
			synthOutputData.synthOutputs[LEFT_CHANNEL] += gainFactor * voiceRender.synthOutputs[0];
			synthOutputData.synthOutputs[RIGHT_CHANNEL] += gainFactor * voiceRender.synthOutputs[1];
		}
	}

	// --- apply master volume
	//     globalMIDIData[kMIDIMasterVolume] = 0 -> 16383
	//	   mapping to -60dB(0.001) to +12dB(4.0)
	double masterVol = midi14_bitToDouble(midiInputData->globalMIDIData[kMIDIMasterVolumeLSB], midiInputData->globalMIDIData[kMIDIMasterVolumeMSB], 0.001, 4.0);
			
	// --- apply master volume
	synthOutputData.synthOutputs[LEFT_CHANNEL] *= masterVol;
	synthOutputData.synthOutputs[RIGHT_CHANNEL] *= masterVol;

	// --- note that this is const, and therefore read-only
	return synthOutputData;
}



/**
\brief The MIDI event handler function; for note on/off messages it finds the voices to turn on/off.
MIDI CC information is placed in the shared CC array.

\param event a single MIDI Event to decode and process

\return true if handled, false otherwise
*/
bool SynthEngine::processMIDIEvent(midiEvent& event)
{
	if (parameters.enableMIDINoteEvents && event.midiMessage == NOTE_ON)
	{
		// --- set current MIDI data
		midiInputData->globalMIDIData[kCurrentMIDINoteNumber] = event.midiData1;
		midiInputData->globalMIDIData[kCurrentMIDINoteVelocity] = event.midiData2;

		// --- stash the last midi note in the event's aux data member!
		event.auxUintData1 = midiInputData->globalMIDIData[kLastMIDINoteNumber];

		TRACE("-- Note On Ch:%d Note:%d Vel:%d \n", event.midiChannel, event.midiData1, event.midiData2);
		TRACE("-- LAST Note On Note:%d \n", event.auxUintData1);

		// --- mono mode
		if (parameters.mode == synthMode::kMono)
		{
			// --- just use voice 0 and do the note EG variables will handle the rest
			synthVoices[0]->processMIDIEvent(event);
		}
		else if (parameters.mode == synthMode::kPoly)
		{
			// --- get index of the next available voice (for note on events)
			int voiceIndex = getFreeVoiceIndex();

			if (voiceIndex < 0)
			{
				voiceIndex = getVoiceIndexToSteal();
				TRACE("-- Note On STEALING -> Voice:%d Note:%d Vel:%d \n", voiceIndex, event.midiData1, event.midiData2);
			}

			// --- trigger next available note
			if (voiceIndex >= 0)
			{
				synthVoices[voiceIndex]->processMIDIEvent(event);
				TRACE("-- Note On -> Voice:%d Note:%d Vel:%d \n", voiceIndex, event.midiData1, event.midiData2);
			}
			else // --- steal voice
				TRACE("-- DID NOT getFreeVoiceIndex index:%d \n", voiceIndex);

			// --- increment all timestamps for note-on voices
			for (int i = 0; i < MAX_VOICES; i++)
			{
				if (synthVoices[i]->isVoiceActive())
					synthVoices[i]->incrementTimestamp();
			}
		}
		else if (parameters.mode == synthMode::kUnison)
		{
			// --- UNISON mode is heavily dependent on the manufacturer's 
			//     implementation and decision
			//     for the synth core, we will use 3 voices detuned as: -parameters.unisonDetune_Cents, 0, +parameters.unisonDetune_Cents
			synthVoices[0]->processMIDIEvent(event);
			synthVoices[1]->processMIDIEvent(event);
			synthVoices[2]->processMIDIEvent(event);
			synthVoices[3]->processMIDIEvent(event);
		}

		// --- need to store these for things like portamento
		// --- store global data for note ON event: set previous note-on data
		midiInputData->globalMIDIData[kLastMIDINoteNumber] = event.midiData1;
		midiInputData->globalMIDIData[kLastMIDINoteVelocity] = event.midiData2;
	}
	else if (parameters.enableMIDINoteEvents && event.midiMessage == NOTE_OFF)
	{
		TRACE("-- Note Off Ch:%d Note:%d Vel:%d \n", event.midiChannel, event.midiData1, event.midiData2);

		// --- for mono, we only use one voice, number [0]
		if (parameters.mode == synthMode::kMono)
		{
			if (synthVoices[0]->isVoiceActive())
			{
				synthVoices[0]->processMIDIEvent(event);
				return true;
			}
		}
		else if (parameters.mode == synthMode::kPoly)
		{
			// --- find the note with this MIDI number (this implies that note numbers and voices are exclusive to each other)
			int voiceIndex = getActiveVoiceIndexInNoteOn(event.midiData1);
			
			if (voiceIndex < 0)
			{
				voiceIndex = getStealingVoiceIndexInNoteOn(event.midiData1);
				if (voiceIndex >= 0)
					TRACE("-- Note OFF on STEAL-PENDING -> Voice:%d Note:%d Vel:%d \n", voiceIndex, event.midiData1, event.midiData2);
			}

			if (voiceIndex >= 0)
			{
				synthVoices[voiceIndex]->processMIDIEvent(event);
				TRACE("-- Note Off -> Voice:%d Note:%d Vel:%d \n", voiceIndex, event.midiData1, event.midiData2);
			}
			else
				TRACE("-- DID NOT FOUND NOTE OFF index:%d \n", voiceIndex);
			// --- this is very bad - it means we probably have a stuck note... :\

			return true;
		}
		else if (parameters.mode == synthMode::kUnison)
		{
			// --- this will get complicated with voice stealing.
			synthVoices[0]->processMIDIEvent(event);
			synthVoices[1]->processMIDIEvent(event);
			synthVoices[2]->processMIDIEvent(event);
			synthVoices[3]->processMIDIEvent(event);

			return true;
		}
	}
	else // --- non-note stuff here!
	{
		// --- store the data in our arrays; sub-components have access to all data via safe IMIDIData pointer
		if (event.midiMessage == PITCH_BEND)
		{
			TRACE("-- Pitch Bend Ch:%d LSB:%d MSB:%d \n", event.midiChannel, event.midiData1, event.midiData2);

			midiInputData->globalMIDIData[kMIDIPitchBendDataLSB] = event.midiData1;
			midiInputData->globalMIDIData[kMIDIPitchBendDataMSB] = event.midiData2;
		}
		if (event.midiMessage == CONTROL_CHANGE)
		{
			TRACE("-- MIDI CC Ch:%d CC:%d Value:%d \n", event.midiChannel, event.midiData1, event.midiData2);

			// --- store CC event in globally shared array
			midiInputData->ccMIDIData[event.midiData1] = event.midiData2;
		}

		// --- NOTE: this synth has GUI controls for items that may also be transmitted via SYSEX-MIDI
		//
		//           If you want your synth plugin to support these messages, you need to add the code here
		//           to handle the MIDI. See any website with MIDI specs details or 
		//			 http://www.somascape.org/midi/tech/spec.html
		//
		//			 The following global MIDI slots are used in this synth core so you want to process them too;
		//			 You may also decide to add more GUI controls and/or MIDI SYSEX message handling.
		//
		//			 globalMIDIData[kMIDIMasterVolume] 
		//			 globalMIDIData[kMIDIMasterPitchBendCoarse] // aka pitch bend sensitivity
		//			 globalMIDIData[kMIDIMasterPitchBendFine]	// aka pitch bend sensitivity
		//			 globalMIDIData[kMIDIMasterTuningCoarse] 
		//			 globalMIDIData[kMIDIMasterTuningFine] 
		// 
	}

	return true;
}

SynthEngineParameters SynthEngine::getParameters()
{
	return parameters;
}

void SynthEngine::setParameters(const SynthEngineParameters& _parameters)
{
	// --- store parameters
	parameters = _parameters;

	// --- master volume maps to MIDI RPN see http://www.somascape.org/midi/tech/spec.html#usx7F0401
	double masterVolumeRaw = dB2Raw(parameters.masterVolume_dB);
	boundValue(masterVolumeRaw, 0.001, 4.0);

	// --- master volume is our own mapping -60 to +12
	uint32_t unipolarValue = mapDoubleToUINT(masterVolumeRaw, 0.001, 4.0, 0, 16383);

	// --- map 0 -> 16383 to MIDI 14-bit
	unipolarIntToMIDI14_bit(unipolarValue, midiInputData->globalMIDIData[kMIDIMasterVolumeLSB], midiInputData->globalMIDIData[kMIDIMasterVolumeMSB]);

	// --- store pitch bend range in midi data table; for a released synth, you want to decode this as SYSEX as well
	// --- sensitivity is in semitones (0 -> 127) and cents (0 -> 127)
	unsigned int pbCoarse = parameters.masterPitchBendSensCoarse;
	boundMIDIValueByte(pbCoarse);

	unsigned int pbFine = parameters.masterPitchBendSensFine;
	boundMIDIValueByte(pbFine);

	midiInputData->globalMIDIData[kMIDIMasterPBSensCoarse] = pbCoarse; // --- unsigned integer part of tuning, in semitones
	midiInputData->globalMIDIData[kMIDIMasterPBSensFine] = pbFine;		// --- fractional part of tuning, converted to cents PRIOR to this call

	// --- store master tuning info from GUI controls
	//     for a released synth, you want to decode this as SYSEX as well
	//
	// --- Master Tuning: See MIDI Spec http://www.somascape.org/midi/tech/spec.html#usx7F0401
	//
	// --- coarse (semitones): -64 to +63 maps-> 0, 127 (7-bit)
	//     This message is transmitted as )LSB, MSB) but LSB is always 0 so we can ignore
	int mtCoarse = parameters.masterTuningCoarse;
	mapIntValue(mtCoarse, -64, +63, 0, 127);		// --- MIDI specified values, not using constants here
	midiInputData->globalMIDIData[kMIDIMasterTuneCoarseMSB] = mtCoarse;
	midiInputData->globalMIDIData[kMIDIMasterTuneCoarseLSB] = 0;	// --- should never change

	// --- fine (cents): -100 to +100 maps-> -8192, 8191 (14-bit)
	int mtFine = parameters.masterTuningFine;
	mapIntValue(mtFine, -100, +100, -8192, 8191, false); // MIDI uses [-8192, 8191] range, false = no rounding (this is a big deal with MIDI)
										
	// --- map -8192 -> 8191 to MIDI 14-bit
	bipolarIntToMIDI14_bit(mtFine, -8192, 8191, midiInputData->globalMIDIData[kMIDIMasterTuneFineLSB], midiInputData->globalMIDIData[kMIDIMasterTuneFineMSB]);

	// --- now trickle down the voice parameters
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		if (synthVoices[i]->isVoiceActive())
		{
			// -- note the special handling for unison mode - you could probably
			//    clean this up
			if (parameters.mode == synthMode::kUnison)
			{	
				if (i == 0)
				{
					parameters.voiceParameters->voiceUnisonDetune_Cents = 0.0;
					parameters.voiceParameters->dcaParameters->panValue = 0.5;
				}
				else if (i == 1)
				{
					parameters.voiceParameters->voiceUnisonDetune_Cents = parameters.masterUnisonDetune_Cents;
					parameters.voiceParameters->dcaParameters->panValue = -1.0;
				}
				else if (i == 2)
				{
					parameters.voiceParameters->voiceUnisonDetune_Cents = -parameters.masterUnisonDetune_Cents;
					parameters.voiceParameters->dcaParameters->panValue = 1.0;
				}
				else if (i == 3)
				{
					parameters.voiceParameters->voiceUnisonDetune_Cents = 0.707*parameters.masterUnisonDetune_Cents;
					parameters.voiceParameters->dcaParameters->panValue = -0.5;
				}
			}
			else
			{
				parameters.voiceParameters->voiceUnisonDetune_Cents = 0.0;
				parameters.voiceParameters->dcaParameters->panValue = 0.0;
			}
		}
	}
}

// --- find the first free voice
int SynthEngine::getFreeVoiceIndex()
{
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		// --- return index of first free voice we find
		if (!synthVoices[i]->isVoiceActive())
			return i;
	}

	// --- didn't find any
	return -1;
}

int SynthEngine::getVoiceIndexToSteal()
{
	// --- find oldest note
	int index = -1;

	// --- add your heuristic code here to return the index of the voice to steal

	// --- index should always be >= 0
	return index;
}


int SynthEngine::getActiveVoiceIndexInNoteOn(unsigned int midiNoteNumber)
{
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		if (synthVoices[i]->isVoiceActive() &&
			synthVoices[i]->getVoiceState() == voiceState::kNoteOnState &&
			synthVoices[i]->getMIDINoteNumber() == midiNoteNumber)
			return i;
	}
	return -1;
}

int SynthEngine::getStealingVoiceIndexInNoteOn(unsigned int midiNoteNumber)
{
	for (unsigned int i = 0; i < MAX_VOICES; i++)
	{
		if (synthVoices[i]->isVoiceActive() &&
			synthVoices[i]->getVoiceState() == voiceState::kNoteOnState &&
			synthVoices[i]->voiceIsStealing() && 
			synthVoices[i]->getStealMIDINoteNumber() == midiNoteNumber)
			return i;
	}
	return -1;
}

