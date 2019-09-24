Follow these instructions to add the wavetable set:

1. add the /wavetables/ folder to your <RAFX Project>/PluginObjects folder

2. add the analog_square_1.h file to your <RAFX Project>/PluginObjects folder

3. open the wavetableoscillator.h file and go to the constructor for WaveTableData(), line 151; here is where you add new tables, from .h files or brick (.tbl) files, or from scratch (see example code).

Go to the case statement for the .tbl file and change the path to match YOUR plugin project path; keep the .tbl file the same.

case BANK_0_WAVE_1:
{
	// --- example of the (favored) high-resolution wavetable set with brick file 
	wt->initWithBrickFile("C:\\RackAFX Projects\\SimpleSynth\\PluginObjects\\wavetables\\analog_square_1.tbl");
	break;
}

4. in wavetableoscillator.cpp go to line 168 and set the oscillatorWaveformIndex to 0 (if it is not already)

uint32_t WaveTableBank::selectTable(int oscillatorWaveformIndex, uint32_t midiNoteNumber)
{
	// --- for testing
	oscillatorWaveformIndex = 0; // for testing remove this.....later.....


--- BUILD AND TEST ---
If you get the squarewave (index 0) then try for the brick file. Change the index to 1:

uint32_t WaveTableBank::selectTable(int oscillatorWaveformIndex, uint32_t midiNoteNumber)
{
	// --- for testing
	oscillatorWaveformIndex = 1; // for testing remove this.....later.....
	
	
	
	
	
	
	
	
	
	
	
