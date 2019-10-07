/*
  ==============================================================================

    Wavetable.h
    Created: 9 Nov 2018 12:18:52pm
    Author:  Macbook

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Utility/Enums.h"
#include "../Compressor/Compressor.h"

//============================================================================
class RandomLfo
{
public:
    RandomLfo()  : value (random.nextDouble())
    {
    }
    
    RandomLfo (const RandomLfo&) = delete;
    
    ~RandomLfo()
    {
    }
    
    void prepare()
    {
        value = random.nextDouble();
    }
    
    double randomFrom0to1 (double counter, double numSamples,
                           double amplitude, double ampOffset,
                           double phaseOffsetFrom0to1)
    {
        return getValue (counter, 8, numSamples,
                         amplitude, ampOffset, phaseOffsetFrom0to1);
    }
    
    double noiseFrom0to1 (int counter, double numSamples,
                          double amplitude, double ampOffset,
                          double phaseOffsetFrom0to1)
    {
        return getValue (counter, 10000, numSamples,
                         amplitude, ampOffset, phaseOffsetFrom0to1);
    }
    
    //copy
    void copyFrom (const RandomLfo& rl)
    {
        value = rl.value;
    }
    
private:
    Random random;
    double value;
    
    double getValue (double counter, int numValues, int numSamples,
                     double amplitude, double ampOffset,
                     double phaseOffsetFrom0to1)
    {
        float n = numSamples / numValues;
        counter = fmod (counter, n);
        
        if (floor (counter) == 0)
            value = random.nextDouble();
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        return value * correctAmplitude + ampOffset;
    }
};

//===========================================================================================
class Wavetable
{
public:
    Wavetable(){}
    Wavetable (const Wavetable& w) = delete;
    virtual ~Wavetable(){};
    
    virtual void prepare (double sampleRate) = 0;
    virtual double getCurrentValueMono (float channelLevel) = 0;
    virtual void getCurrentValueStereo (float channelLevel, float& resultL, float& resultR) = 0;
    
    virtual void clear() { c = 0; }
    int getCounter() const { return c; }
    int getNumSamples() const { return numSamples; }
    
protected:
    int numSamples = 0;
    int c = 0;
};
//===========================================================================================
class Oscillator  : public Wavetable
{
public:
    Oscillator(){}
    Oscillator (const Oscillator& o) = delete;
    virtual ~Oscillator(){}
    
    void prepare (double sampleRate) override
    {
        numSamples = sampleRate / frequency;
        randomLfoL.prepare();
        randomLfoR.prepare();
        Oscillator::sampleRate = sampleRate;
    }
    
    double getCurrentValueMono (float channelLevel = 1) override
    {
        c = c > numSamples - 1 ? c = 0 : c = c;
        
        double val;
        
        if (shape == wtRandom)
            val = randomLfoL.randomFrom0to1 (c, numSamples, amplitude, ampOffset,
                                             phaseOffsetL);
        else if (shape == wtNoise)
            val = randomLfoL.noiseFrom0to1 (c, numSamples, amplitude, ampOffset,
                                            phaseOffsetL);
        else
            val = getOscValue (c, numSamples, amplitude, ampOffset,
                                phaseOffsetL, sampleRate);
        c++;
        return val;
    }
    
    void getCurrentValueStereo (float channelLevel, float& resultL, float& resultR) override
    {
        c = c > numSamples - 1 ? c = 0 : c = c;
        
        if (shape == wtRandom)
        {
            resultL = randomLfoL.randomFrom0to1 (c, numSamples, amplitude, ampOffset,
                                                 phaseOffsetL);
            resultR = randomLfoL.randomFrom0to1 (c, numSamples, amplitude, ampOffset,
                                                 phaseOffsetR);
        }
        else if (shape == wtNoise)
        {
            resultL = randomLfoL.noiseFrom0to1 (c, numSamples, amplitude, ampOffset,
                                                phaseOffsetL);
            resultR = randomLfoR.noiseFrom0to1 (c, numSamples, amplitude, ampOffset,
                                                phaseOffsetR);
        }
        else
        {
            resultL = getOscValue (c, numSamples, amplitude, ampOffset,
                                   phaseOffsetL, sampleRate);

            resultR = getOscValue (c, numSamples, amplitude, ampOffset,
                                   phaseOffsetR, sampleRate);
        }
        c++;
    }
    
    //setters
    void setShape (OscShape newShape)
    {
        if (shape != newShape)
        {
            shape = newShape;
            switch (shape)
            {
                case wtSin:
                    getOscValue = &Utility::sinFrom0to1;
                    break;
                    
                case wtTriangle:
                    getOscValue = &Utility::triangleFrom0to1;
                    break;
                    
                case wtSaw:
                    getOscValue = &Utility::sawFrom0to1;
                    break;
                    
                case wtSquare:
                    getOscValue = &Utility::squareFrom0to1;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    void setFrequency (double freq, double sampleRate)
    {
        frequency = freq;
        numSamples = sampleRate / frequency;
    }
    
    void setAmplitude (double amp)
    {
        amplitude = amp;
    }
    
    void setAmpOffset (double newAmpOffset)
    {
        ampOffset = newAmpOffset;
    }
    
    void setPhaseOffset0to1 (double newPhaseOffset0to1)
    {
        phaseOffsetL = newPhaseOffset0to1;
    }
    
    void setChannelPhaseOffset (double offset0to1)
    {
        phaseOffsetR = fmod (phaseOffsetL + offset0to1, 1.0);
    }
    
    void setAllParameters (double freq, double amp, double ampOffset,
                           double newPhaseOffset0to1, double sampleRate)
    {
        setFrequency (freq, sampleRate);
        setAmplitude (amp);
        setAmpOffset (ampOffset);
        setPhaseOffset0to1 (newPhaseOffset0to1);
        Oscillator::sampleRate = sampleRate;
    }
    
    //getters
    double getFrequency() const
    {
        return frequency;
    }
    double getAmplitude() const
    {
        return amplitude;
    }
    double getAmpOffset() const
    {
        return ampOffset;
    }
    OscShape getOscShape() const
    {
        return shape;
    }
    
    //operator overloading
    virtual void copyParametersFrom (const Oscillator& o)
    {
        numSamples = o.numSamples;
        c = o.c;
        frequency = o.frequency;
        amplitude = o.amplitude;
        ampOffset = o.ampOffset;
        phaseOffsetL = o.phaseOffsetL;
        phaseOffsetR = o.phaseOffsetR;
        getOscValue = o.getOscValue;
        sampleRate = o.sampleRate;
        randomLfoL.copyFrom (o.randomLfoL);
        randomLfoR.copyFrom (o.randomLfoR);
    }
    
    friend class OscRandom;
    
protected:
    double sampleRate;
    OscShape shape = wtSin;
    double frequency;
    double amplitude = 1.0, ampOffset = 0.0, phaseOffsetL = 0.0, phaseOffsetR = 0.5;
    
private:
    RandomLfo randomLfoL, randomLfoR;
    double (*getOscValue) (double currentSample, double numSamples, double amplitude,
                           double ampOffset, double phaseOffsetFrom0to1,
                           double sampleRate)
    = Utility::sinFrom0to1;
};

//============================================================================================
class Envelope  : public Wavetable
{
public:
    Envelope(){}
    Envelope (const Envelope&) = delete;
    ~Envelope(){}
    
    void prepare (double sampleRate) override
    {
        aSamp = Utility::msToSamples (aMs, sampleRate);
        rSamp = Utility::msToSamples (rMs, sampleRate);
        numSamples = aSamp + rSamp;
    }
    
    //main function
    double getCurrentValueMono (float channelLevel) override
    {
        if (c > numSamples - 1)
        {
            if (channelLevel > threshold)
                clear();
            else
                return aStart0to1;
        }
        
        if (c < aSamp) // if current curve is attack
        {
            if (aUp) // if attack goes Up
            {
                //return function accourding to the curve's shape
                if (aFat)
                {
                    double val =  Utility::fatCurveUp (c, aStart0to1, aEnd0to1,
                                                       aQ, aSamp, aSamp - 1);
                    c++;
                    return val;
                }
                else
                {
                    double val = Utility::thinCurveUp (c, aStart0to1, aEnd0to1,
                                                       aQ, aSamp, 0);
                    c++;
                    return val;
                }
            }
            else // if attack goes Down
            {
                //return function accourding to the curve's shape
                if (aFat)
                {
                    double val = Utility::fatCurveDown (c, aEnd0to1, aStart0to1,
                                                        aQ, aSamp, 0);
                    c++;
                    return val;
                }
                else
                {
                    double val = Utility::thinCurveDown (c, aEnd0to1, aStart0to1,
                                                         aQ, aSamp, aSamp - 1);
                    c++;
                    return val;
                }
            }
        }
        else // if current curve is release
        {
            int xEnd = aSamp + rSamp - 1; //get xEnd
            
            if (rUp) // if release goes Up
            {
                //return function accourding to the curve's shape
                if (rFat)
                {
                    double val = Utility::fatCurveUp (c, aEnd0to1, rEnd0to1,
                                                      rQ, rSamp, xEnd);
                    c++;
                    return val;
                }
                else
                {
                    double val = Utility::thinCurveUp (c, aEnd0to1, rEnd0to1,
                                                       rQ, rSamp, aSamp);
                    c++;
                    return val;
                }
            }
            else // if release goes Down
            {
                //return function accourding to the curve's shape
                if (rFat)
                {
                    double val = Utility::fatCurveDown (c, rEnd0to1, aEnd0to1,
                                                        rQ, rSamp, aSamp);
                    c++;
                    return val;
                }
                else
                {
                    double val = Utility::thinCurveDown (c, rEnd0to1, aEnd0to1,
                                                         rQ, rSamp, xEnd);
                    c++;
                    return val;
                }
            }
        }
    }
    
    void getCurrentValueStereo (float channelLevel, float& resultL, float& resultR) override
    {
        resultL = getCurrentValueMono (channelLevel);
        c = c == 0 ? c = numSamples - 1 : c = c - 1;
        
        float at = aQ;
        float re = rQ;
        aQ += 0.5;
        rQ += 0.5;
        
        resultR = getCurrentValueMono (channelLevel);
        
        aQ = at;
        rQ = re;
    }
    
    //getters
    int getLengthInSamples() const
    {
        return aSamp + rSamp;
    }
    
    //setters
    void setAttackMs (double timeMs, double sampleRate)
    {
        aMs = timeMs;
        aSamp = Utility::msToSamples (timeMs, sampleRate);
        numSamples = aSamp + rSamp;
    }
    
    void setReleaseMs (double timeMs, double sampleRate)
    {
        rMs = timeMs;
        rSamp = Utility::msToSamples (timeMs, sampleRate);
        numSamples = aSamp + rSamp;
    }
    
    void setPoints (double attackStart0to1, double attackEnd0to1, double releaseEnd0to1)
    {
        aStart0to1 = attackStart0to1;
        aEnd0to1 = attackEnd0to1;
        rEnd0to1 = releaseEnd0to1;
        
        aUp = aStart0to1 < aEnd0to1;
        rUp = aEnd0to1 < rEnd0to1;
    }
    
    void setAttackQuality (float q)
    {
        aQ = abs (q);
        aFat = q > 0;
    }
    
    void setReleaseQuality (float q)
    {
        rQ = abs (q);
        rFat = q > 0;
    }
    
    void setThreshold0to1 (float threshold0to1)
    {
        threshold = threshold0to1;
    }
    
    void setAllParameters (float attackMs, float releaseMs,
                           double attackStart0to1, double attackEnd0to1,
                           double releaseEnd0to1,
                           float attackQuality, float releaseQuality,
                           float threshold0to1,
                           double sampleRate)
    {
        setAttackMs (attackMs, sampleRate);
        setReleaseMs (releaseMs, sampleRate);
        
        setPoints (attackStart0to1, attackEnd0to1, releaseEnd0to1);
        
        setAttackQuality (attackQuality);
        setReleaseQuality (releaseQuality);
        
        setThreshold0to1 (threshold0to1);
    }
    
private:
    float threshold = 1.0;
    double aMs = 0, rMs = 0;
    double aStart0to1 = 0.0, aEnd0to1 = 1.0, rEnd0to1 = 0.0;
    int aSamp = 0, rSamp = 0;
    float aQ = 1.0, rQ = 1.0;
    bool aFat = false, rFat = false;
    bool aUp, rUp;
    
    //private funcs
    void swapCurves()
    {
        float attQ = aQ;
        bool attF = aFat;
        aQ = rQ;
        aFat = rFat;
        rQ = attQ;
        rFat = attF;
    }
};


class GainRelMod  : public Wavetable
{
public:
    GainRelMod()
    {
        compressorL.setRatio (12);
        compressorL.setAttack (50);
        compressorL.setRelease (100);
        
        compressorR.setRatio (12);
        compressorR.setAttack (25);
        compressorR.setRelease (125);
    }
    
    GainRelMod (const GainRelMod&) = delete;
    ~GainRelMod() {}
    
    void prepare (double sampleRate) override
    {
        compressorL.prepare (sampleRate);
        compressorL.setMakeUpGainDb (0);
        
        compressorR.prepare (sampleRate);
        compressorR.setMakeUpGainDb (0);
    }
    
    void clear() override
    {
        compressorL.clear();
        compressorR.clear();
    }
    
    void setAttackMs (float attackMs)
    {
        compressorL.setAttack (attackMs);
        compressorR.setRelease (attackMs);
    }
    
    void setReleaseMs (float releaseMs)
    {
        compressorL.setRelease (releaseMs);
        compressorR.setAttack (releaseMs);
    }
    
    void setStartPoint0to1 (float point)
    {
        startPoint = point;
    }
    
    void setEndPoint0to1 (float point)
    {
        endPoint = point;
    }
    
    void setSensitivityFrom0to1 (float sensitivity)
    {
        float thresh = sensitivity * (-50.0);
        compressorL.setThresholdDb (thresh);
        compressorR.setThresholdDb (thresh);
    }
    
    void setIntensityFrom0to100 (float val)
    {
        compressorL.setRatio (val + 1);
        compressorR.setRatio (val + 1);
    }
    
    void setAllParameters (float attackMs, float releaseMs, float sensitivity0to1)
    {
        setAttackMs (attackMs);
        setReleaseMs (releaseMs);
        setSensitivityFrom0to1 (sensitivity0to1);
    }
    
    double getCurrentValueMono (float channelLevel) override
    {
        float val = compressorL.getGainReductionValueForSample (channelLevel);
        
        if (startPoint > endPoint)
        {
            val *= abs (startPoint - endPoint);
            val += endPoint;
        }
        else
        {
            val = abs (1 - val);
            val *= abs (startPoint - endPoint);
            val += startPoint;
        }
        return val;
    }
    
    void getCurrentValueStereo (float channelLevel, float& resultL, float& resultR) override
    {
        resultL = compressorL.getGainReductionValueForSample (channelLevel);
        resultR = compressorR.getGainReductionValueForSample (channelLevel);
        
        float amp = abs (startPoint - endPoint);
        if (startPoint > endPoint)
        {
            resultL *= amp;
            resultL += endPoint;
            
            resultR *= amp;
            resultR += endPoint;
        }
        else
        {
            resultL = abs (1 - resultL);
            resultL *= amp;
            resultL += startPoint;
            
            resultR = abs (1 - resultR);
            resultR *= amp;
            resultR += startPoint;
        }
    }
    
private:
    Compressor compressorL, compressorR;
    float startPoint;
    float endPoint;
};
