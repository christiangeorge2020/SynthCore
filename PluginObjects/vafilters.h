#ifndef __vaFilters_h__
#define __vaFilters_h__

// --- includes
#include "synthdefs.h"

const int NUM_SUBFILTERS = 4;

/**
\class ZVAFilterEx
\ingroup SynthClasses
\brief Extends ZVAFilter for use in MOOG filter

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
class ZVAFilterEx : public ZVAFilter
{
public:
	ZVAFilterEx() {}
	~ZVAFilterEx() {}

	// --- added for MOOG - you need access to the s-ports for the first order filter
	double getS0Port() { return integrator_z[0]; }
	double getS1Port() { return integrator_z[1]; } // <--- not used in MOOG 

	// --- need alpha
	double getAlpha() { return alpha; }
	double getLittle_g() 
	{
		double fc = zvaFilterParameters.fc;
		double Q = zvaFilterParameters.Q;
		vaFilterAlgorithm filterAlgorithm = zvaFilterParameters.filterAlgorithm;

		// --- normal Zavalishin SVF calculations here
		//     prewarp the cutoff- these are bilinear-transform filters
		double wd = kTwoPi*fc;
		double T = 1.0 / sampleRate;
		double wa = (2.0 / T)*tan(wd*T / 2.0);
		double g = wa*T / 2.0;

		return g; 
	}

protected:
};

// --- audio filters, 1st and 2nd order currently supported
enum class moogFilterAlgorithm {
	kLPF2, kHPF2, kLPF4, kHPF4

}; 

struct MoogFilterParameters
{
	MoogFilterParameters() {}
	MoogFilterParameters& operator=(const MoogFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		filterAlgorithm = params.filterAlgorithm;
		fc = params.fc;
		Q = params.Q;
		filterOutputGain_dB = params.filterOutputGain_dB;
		enableGainComp = params.enableGainComp;
		enableNLP = params.enableNLP;
		return *this;
	}

	// --- individual parameters
	moogFilterAlgorithm filterAlgorithm = moogFilterAlgorithm::kLPF4;
	double fc = 1000.0;
	double Q = 0.707;
	double filterOutputGain_dB = 0.0; // usually not used
	bool enableGainComp = false;
	bool enableNLP = false;
};


/**
\class MoogFilter
\ingroup SynthClasses
\brief Encapsulates a Moog Ladder Filter.

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
class MoogFilter : public IAudioSignalProcessor
{
public:
	MoogFilter()
	{
		// --- setup four identical ZVAFilters as LPF1 types
		ZVAFilterParameters params = subFilter[0].getParameters();
		params.filterAlgorithm = vaFilterAlgorithm::kLPF1;

		// --- this does not work because it fucks up the phase response at Nyquist
		// params.matchAnalogNyquistLPF = true;

		for (int i = 0; i < NUM_SUBFILTERS; i++)
		{
			subFilter[i].setParameters(params);
		}
	}		/* C-TOR */
	~MoogFilter() {}		/* D-TOR */

	// --- IAudioSignalProcessor
	// --- set sample rate, then update coeffs
	virtual bool reset(double _sampleRate)
	{
		// --- initialize four identical ZVAFilters as LPF1 types
		for (int i = 0; i < NUM_SUBFILTERS; i++)
		{
			subFilter[i].reset(_sampleRate);
		}

		// --- setup
		calculateFilterCoeffs();
		return true;
	}

	// --- get parameters (note using ZVA params)
	MoogFilterParameters getParameters()
	{
		return moogFilterParameters;
	}

	// --- set parameters
	void setParameters(const MoogFilterParameters& params)
	{
		// --- if Fc changed, reset it on all subfilters
		moogFilterParameters = params;

		ZVAFilterParameters zvaParams = subFilter[0].getParameters();
		zvaParams.fc = params.fc;
		zvaParams.filterAlgorithm = vaFilterAlgorithm::kLPF1;

		for (int i = 0; i < NUM_SUBFILTERS; i++)
		{
			if (i > 1 && params.filterAlgorithm == moogFilterAlgorithm::kLPF2)
				zvaParams.filterAlgorithm = vaFilterAlgorithm::kAPF1;

			subFilter[i].setParameters(zvaParams);
		}
		// --- changing fc on sub-filters requires recalc of Betas
		calculateFilterCoeffs();
	}

	// --- calculate MOOG coefficients; the subFilters are updated with subFilter[i].setParameters(params);
	void calculateFilterCoeffs()
	{
		if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kLPF4 ||
			moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kHPF4 ||
			moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kHPF2)
		{
			// --- Q is 1 -> 10 for my plugins; just map it to 0 -> 4
			K = (4.0)*(moogFilterParameters.Q - 1.0) / (10.0 - 1.0);

			double G = subFilter[0].getAlpha();
			double g = subFilter[0].getLittle_g();
			double onePlus_g = 1.0 + g;

			// --- follow cookbook instructions
			beta[0] = (G*G*G / onePlus_g);
			beta[1] = (G*G / onePlus_g);
			beta[2] = (G / onePlus_g);
			beta[3] = (1.0 / onePlus_g);

			// --- alpha0 = 1/(1 + K*G*G*G*G)
			alpha0 = 1.0 / (1.0 + K*G*G*G*G);
		}
		else if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			// --- Q controls always 1 -> 10  kFilterGUI_Q_Range = (10.0 - 1.0)
			//	   this maps qControl = 1 -> 10   to   K = 0 -> 2
			K = (2.0)*(moogFilterParameters.Q - 1.0) / (10.0 - 1.0);

			// the allpass G value
			double G = subFilter[0].getAlpha();
			double g = subFilter[0].getLittle_g();
			double onePlus_g = 1.0 + g;
			double GA = 2.0*G - 1;

			// --- half ladder calcs
			// --- follow cookbook instructions
			beta[0] = (GA*G / onePlus_g);
			beta[1] = (GA / onePlus_g);
			beta[2] = (2.0 / onePlus_g);
			beta[3] = 0.0;

			// calculate alpha0
			alpha0 = 1.0 / (1.0 + K*GA*G*G);
		}
	}

	// --- we just do samples
	virtual bool canProcessAudioFrame() { return false; }

	// --- process audio: run the filter
	virtual double processAudioSample(double xn)
	{
		// --- 4th order MOOG:
		double sigma = 0.0;
		if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			sigma = beta[0] * subFilter[0].getS0Port() +
					beta[1] * subFilter[1].getS0Port() +
					beta[2] * subFilter[2].getS0Port();
		}
		else // all others
		{
			sigma = beta[0] * subFilter[0].getS0Port() +
					beta[1] * subFilter[1].getS0Port() +
					beta[2] * subFilter[2].getS0Port() +
					beta[3] * subFilter[3].getS0Port();
		}

		// --- gain comp is a simple on/off switch LPF ONLY!!!!
		if(moogFilterParameters.enableGainComp)
			xn *= 1.0 + 0.5*K; // --- increase 0.5 for MORE bass

		// --- now figure out u(n) = alpha0*[x(n) - K*sigma]
		double u = alpha0*(xn - K*sigma);

		if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kLPF4)
		{
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double y2 = subFilter[2].processAudioSample(y1);
			double yn = subFilter[3].processAudioSample(y2);

			// --- done
			return yn;
		}
		else if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kLPF2)
		{
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double yn = subFilter[2].processAudioSample(y1);

			// --- done
			return yn;
		}
		else if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kHPF4 ||
				 moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kHPF2)
		{
			double A, B, C, D, E = 0.0;
			if (moogFilterParameters.filterAlgorithm == moogFilterAlgorithm::kHPF4)
			{
				A = 1.0;
				B = -4.0;
				C = 6.0;
				D = -4.0;
				E = 1.0;
			}
			else
			{
				A = 1.0;
				B = -2.0;
				C = 1.0;
				D = 0.0;
				E = 0.0;
			}
			// --- cascade of 4 filters
			// --- send u -> LPF1 and then cascade the outputs to form y(n)
			double y0 = subFilter[0].processAudioSample(u); //< --- NOT x(n), u(n)
			double y1 = subFilter[1].processAudioSample(y0);
			double y2 = subFilter[2].processAudioSample(y1);
			double yn = subFilter[3].processAudioSample(y2);

			yn = A*u + B*y0 + C*y1 + D*y2+ E*yn;
			return yn;
		}

		return xn; // should never happen
	}

protected:
	MoogFilterParameters moogFilterParameters;			// our copy for fc, Q and filter type
	ZVAFilterEx subFilter[NUM_SUBFILTERS];				// --- NUM_SUBFILTERS filters

	// --- four beta values
	double beta[NUM_SUBFILTERS] = { 0.0 };
	double alpha0 = 1.0;	// --- our delay free loop correction coeffient (different from sub filters which also have their own)
	double K = 0.0;			// --- this is 0.0 --> 4.0 = Q from 1 --> 10
};

enum class twinFilterConfig { kBypass, kTWIN_LP4, kTWIN_LP2, kHP_LP4, kHP_LP2 };

// --- Double-moog filters
struct TwinMoogFilterParameters
{
	TwinMoogFilterParameters() {}
	TwinMoogFilterParameters& operator=(const TwinMoogFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		fc1 = params.fc1;
		Q1 = params.Q1;

		fc2 = params.fc2;
		Q2 = params.Q2;

		enableGainComp = params.enableGainComp;

		filterConfiguration = params.filterConfiguration;

		return *this;
	}

	// --- individual parameters
	double fc1 = 0.0;
	double Q1 = 0.707;
	double fc2 = 0.0;
	double Q2 = 0.707;
	bool enableGainComp = false;

	twinFilterConfig filterConfiguration = twinFilterConfig::kTWIN_LP4;
};


/**
\class MoogFilter
\ingroup SynthClasses
\brief Encapsulates a Moog Ladder Filter.

\author Will Pirkle
\version Revision : 1.0
\date Date : 2017 / 09 / 24
*/
const double freqModLow = 20.0;
const double freqModHigh = 18000.0;

class TwinMoogFilters : public ISynthProcessor
{
public:
	TwinMoogFilters(const std::shared_ptr<MidiInputData> _midiInputData, std::shared_ptr<TwinMoogFilterParameters> _parameters)
		: midiInputData(_midiInputData)
		, parameters(_parameters)
	{
		if (!parameters)
			parameters = std::make_shared<TwinMoogFilterParameters>();

		// --- this is the total range; though we may only use half in the calc
		freqModSemitoneRange = semitonesBetweenFrequencies(freqModLow, freqModHigh) / 2.0;

		for (int i = 0; i < 2; i++)
		{
			limiter[i].setThreshold_dB(0.0);
		}

	}		/* C-TOR */
	~TwinMoogFilters() {}		/* D-TOR */

	// --- ISynthProcessor
	inline virtual bool update(bool updateAllModRoutings = true)
	{
		// --- Run priority modulators 

		// --- End Priority modulators
		if (!updateAllModRoutings)
			return true;

		MoogFilterParameters zvaParams1 = subFilter[0].getParameters();
		MoogFilterParameters zvaParams2 = subFilter[1].getParameters();

		if (parameters->filterConfiguration == twinFilterConfig::kHP_LP2)
		{
			zvaParams1.filterAlgorithm = moogFilterAlgorithm::kHPF2;
			zvaParams2.filterAlgorithm = moogFilterAlgorithm::kLPF2;
		}
		else if (parameters->filterConfiguration == twinFilterConfig::kHP_LP4)
		{
			zvaParams1.filterAlgorithm = moogFilterAlgorithm::kHPF4;
			zvaParams2.filterAlgorithm = moogFilterAlgorithm::kLPF4;
		}
		else if (parameters->filterConfiguration == twinFilterConfig::kTWIN_LP2)
		{
			zvaParams1.filterAlgorithm = moogFilterAlgorithm::kLPF2;
			zvaParams2.filterAlgorithm = moogFilterAlgorithm::kLPF2;
		}
		else if (parameters->filterConfiguration == twinFilterConfig::kTWIN_LP4)
		{
			zvaParams1.filterAlgorithm = moogFilterAlgorithm::kLPF4;
			zvaParams2.filterAlgorithm = moogFilterAlgorithm::kLPF4;
		}

		// --- for freq shifting as semitones
		double freqModSemitones_fc1 = 0.5*freqModSemitoneRange*modulators->modulationInputs[kBipolarMod];		// k_BipolarMod -> Fc1
		double freqModSemitones_fc2 = 0.5*freqModSemitoneRange*modulators->modulationInputs[kAuxBipolarMod_1]; // k_AuxBipolarMod_1 -> Fc2

		double fc1 = parameters->fc1 * pitchShiftTableLookup(freqModSemitones_fc1);
		double fc2 = parameters->fc2 * pitchShiftTableLookup(freqModSemitones_fc2);
		boundValue(fc1, freqModLow, freqModHigh);
		boundValue(fc2, freqModLow, freqModHigh);

		if (true)// (fc1 != finalFilter_fc1)
		{
			zvaParams1.fc = fc1;
			zvaParams1.Q = parameters->Q1;
			zvaParams1.enableGainComp = parameters->enableGainComp;
			subFilter[0].setParameters(zvaParams1);
			finalFilter_fc1 = fc1;
		}
		if (true)//(fc2 != finalFilter_fc2)
		{
			zvaParams2.fc = fc2;
			zvaParams2.Q = parameters->Q2;
			zvaParams2.enableGainComp = parameters->enableGainComp;
			subFilter[1].setParameters(zvaParams2);
			finalFilter_fc2 = fc2;
		}

		return true;
	}

	virtual bool doNoteOn(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity) { return true; }
	virtual bool doNoteOff(double midiPitch, uint32_t _midiNoteNumber, uint32_t midiNoteVelocity) { return true; }

	// --- set sample rate, then update coeffs
	virtual bool reset(double _sampleRate)
	{
		// --- initialize four identical ZVAFilters as LPF1 types
		for (int i = 0; i < 2; i++)
		{
			subFilter[i].reset(_sampleRate);
			limiter[i].reset(_sampleRate);
		}

		return true;
	}

	// --- we just do samples
	virtual bool canProcessAudioFrame() { return false; }

	// --- process audio: run the filter
	virtual double processAudioSample(double xn)
	{
		if (parameters->filterConfiguration == twinFilterConfig::kBypass)
			return xn;

		if (parameters->filterConfiguration == twinFilterConfig::kHP_LP2 ||
			parameters->filterConfiguration == twinFilterConfig::kHP_LP4 )
		{
			double y0 = subFilter[0].processAudioSample(xn);
			double lim0 = limiter[0].processAudioSample(y0);

			double y1 = subFilter[1].processAudioSample(lim0);
			double lim1 = limiter[1].processAudioSample(y1);

			return lim1;
		}
		else if (parameters->filterConfiguration == twinFilterConfig::kTWIN_LP2 ||
				 parameters->filterConfiguration == twinFilterConfig::kTWIN_LP4)
		{
			double y0 = subFilter[0].processAudioSample(xn);
			double lim0 = limiter[0].processAudioSample(y0);

			double y1 = subFilter[1].processAudioSample(xn);
			double lim1 = limiter[1].processAudioSample(y1);

			return lim0 + lim1;
		}

		return xn; // neve rhappen
	}

	// --- access to modulators
	// --- get our modulators
	virtual std::shared_ptr<ModInputData> getModulators() {
		return modulators;
	}

	// --- not used here;
	virtual void setModulators(std::shared_ptr<ModInputData> _modulators) {
		modulators = _modulators;
	}

protected:
	// --- MIDI Data Interface
	const std::shared_ptr<MidiInputData> midiInputData = nullptr;

	// --- modulators
	std::shared_ptr<ModInputData> modulators = std::make_shared<ModInputData>();

	// --- parameters
	std::shared_ptr<TwinMoogFilterParameters> parameters = nullptr;
	
	MoogFilter subFilter[2];	// --- twin filters = 2 of each
	PeakLimiter limiter[2];

	double finalFilter_fc1 = 1000.0;
	double finalFilter_fc2 = 1000.0;
	double freqModSemitoneRange = 0.0;
};




#endif /* defined(__vaFilters_h__) */
