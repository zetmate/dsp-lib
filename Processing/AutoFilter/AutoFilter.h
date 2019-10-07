/*
  ==============================================================================

    AutoFilter.h
    Created: 9 Nov 2018 6:34:58pm
    Author:  Macbook

  ==============================================================================
*/

#pragma once

#include "../Filter/Filter.h"
#include "../Wavetable/Wavetable.h"
#include "../../Utility/Utility.h"
#include "../../Utility/Ramp.h"
#include "../../Utility/Crossfade.h"
#include "../../UI/Parameter.h"

class AutoFilter
{
public:
    AutoFilter (AudioProcessorValueTreeState& valueTree,
                AfParamList& p) : parameters (valueTree),
                                    pl (p), sampleRate (1)
    {
        //set freq range
        freqRange.start = 20;
        freqRange.end = 20000;
        freqRange.setSkewForCentre (1500);
        
        //DEFAULT PARAMETERS
        //default filter
        setFilterTypeWithoutPrepare ((FilterType) pl.filterType.defaultIndex);
        filter->setFrequency (pl.startFreq.defaultVal);
        filter->setQuality (pl.reso.defaultVal);
        filter->setBandwidthOctaves (pl.bandwidth.defaultVal);
        filter1->setFrequency (pl.startFreq.defaultVal);
        filter1->setQuality (pl.reso.defaultVal);
        filter1->setBandwidthOctaves (pl.bandwidth.defaultVal);
        
        //default points
        setStartFreq (pl.startFreq.defaultVal);
        setEndFreq (pl.endFreq.defaultVal);
        
        //default osc parameters
        osc.setShape ((OscShape) pl.oscShape.defaultIndex);
        osc.setAllParameters (pl.mainLfoFreq.defaultVal,
                              osc.getAmplitude(),
                              osc.getAmpOffset(), 0.0, 1000);
        
        //default envelope parameters
        env.setAllParameters (pl.attack.defaultVal, pl.release.defaultVal,
                              pointStart, pointEnd, pointStart, 2.8, -2.8,
                              Utility::fromDb (pl.threshold.defaultVal), 1000);
        
        //default gainRel params
        grl.setSensitivityFrom0to1 (pl.gainRelSens.defaultVal / 100.0);
        
        //default wt
        setModTypeWithoutPrepare ((ModType) pl.mainModType.defaultIndex);
        
        //defaultMix
        mix = 1.0;
        mixRamp.setRange (mix, mix);
    }
    
    AutoFilter (const AutoFilter& other) = delete;
    
    ~AutoFilter()
    {
    }
    
    //Prepare for play function
    void prepare (double newSampleRate)
    {
        //update sample rate
        sampleRate = newSampleRate;
        
        //prepare
        filter->countCoefficients (sampleRate);
        filter1->countCoefficients (sampleRate);
        wt->prepare (sampleRate);
        
        //prepare ramps
        //reso
        resoRamp.setTime (5, sampleRate);
        double reso = filter->getQuality();
        resoRamp.setRange (reso, reso);
        //mix
        mixRamp.setTime (5, sampleRate);
        
        //prepare crossfades
        filterTypeCrossfade.prepare (50, sampleRate);
        modTypeCrossfade.prepare (100, sampleRate);
        
        //releaseResources
        releaseResources();
    }
    
    void releaseResources()
    {
        if (!resourcesReleased)
        {
            //clear filter buffers
            filter->clearBuffers();
            filter1->clearBuffers();
            
            //reset wavetables
            wt->clear();
            prevWt->clear();
            
            //clear crossfades
            filterTypeCrossfade.clear();
            modTypeCrossfade.clear();
            
            //set resourcesReleased to true
            resourcesReleased = true;
        }
    }
    
    void processSampleMono (const float input, float& output);
    
    void processSampleStereo (const float inputL, const float inputR,
                              float& outputL, float& outpuR);
    
    //parameter list
    AudioProcessorValueTreeState& parameters;
    const AfParamList& pl;
    friend class Proc;
    
private:
    double sampleRate;
    double bpm = 120;
    float oneBarHz = 0.5f, oneBarMs = 2000.0f;
    bool resourcesReleased = false;
    
    //values
    bool modOn = true;
    float osc0Freq;
    float freqStart, freqEnd;
    float pointStart, pointEnd;
    FilterType filterType;
    ModType modType;
    bool modStereo = true;
    
    //mix
    double mix;
    Ramp mixRamp;
    
    //filters
    Filter* filter, *filter1;
    Filter* prevFilter = &df, *prevFilter1 = &df1;
    LowPassFilter lp, lp1;
    HighPassFilter hp, hp1;
    BandPassFilter bp, bp1;
    BandStopFilter bs, bs1;
    DevilFilter df, df1;
    
    //modulators
    Wavetable* wt;
    Wavetable* prevWt = &grl;
    Oscillator osc;
    Oscillator prevOsc;
    Envelope env;
    GainRelMod grl;
    
    //ranges
    NormalisableRange<double> freqRange;
    
    //ramps
    Ramp resoRamp;
    
    //crossfades
    Crossfade filterTypeCrossfade;
    Crossfade modTypeCrossfade;
    
    //========================================================================
    //PRIVATE SETTERS
    //===================================================================
    //setters
    //
    //filter setters
    void setFilterType (float filterTypeIndex)
    {
        FilterType ft = (FilterType) roundToInt (filterTypeIndex);
        if (filterType != ft)
        {
            filterType = ft;
            
            //for smooth changing
            prevFilter = filter;
            prevFilter1 = filter1;
            filterTypeCrossfade.setActive (true);
            
            float freq = filter->getFrequency();
            float q = filter->getQuality();
            float bw = filter->getBandwidthOctaves();
            
            switch (ft)
            {
                case lowPass:
                    filter = &lp;
                    filter1 = &lp1;
                    break;
                    
                case highPass:
                    filter = &hp;
                    filter1 = &hp1;
                    break;
                    
                case bandPass:
                    filter = &bp;
                    filter1 = &bp1;
                    break;
                    
                case bandStop:
                    filter = &bs;
                    filter1 = &bs1;
                    break;
                    
                case devil:
                    filter = &df;
                    filter1 = &df1;
                    break;
                    
                default:
                    break;
            }
            
            filter->setFrequency (freq);
            filter->setQuality (q);
            filter->setBandwidthOctaves (bw);
            filter->countCoefficients (sampleRate);
            filter->clearBuffers();
            
            filter1->setFrequency (freq);
            filter1->setQuality (q);
            filter1->setBandwidthOctaves (bw);
            filter1->countCoefficients (sampleRate);
            filter1->clearBuffers();
        }
    }
    
    void setBandwidthOctaves (float numOctaves)
    {
        filter->setBandwidthOctaves (numOctaves);
        filter1->setBandwidthOctaves (numOctaves);
    }
    
    void setStartFreq (float freq)
    {
        freqStart = freq;
        pointStart = freqRange.convertTo0to1 (freqStart);
        env.setPoints (pointStart, pointEnd, pointStart);
        grl.setStartPoint0to1 (pointStart);
        
        osc.setAmpOffset (std::min (pointStart, pointEnd));
        osc.setAmplitude (abs (pointEnd - pointStart));
    }
    
    void setEndFreq (float freq)
    {
        freqEnd = freq;
        pointEnd = freqRange.convertTo0to1 (freq);
        env.setPoints (pointStart, pointEnd, pointStart);
        grl.setEndPoint0to1 (pointEnd);
        
        osc.setAmpOffset (std::min (pointStart, pointEnd));
        osc.setAmplitude (abs (pointEnd - pointStart));
    }
    
    void setResonance (float newResonance)
    {
        resoRamp.setRange (filter->getQuality(), newResonance);
    }
    
    //MAIN MOD setters============
    void setModType (float modTypeIndex)
    {
        ModType newModType = (ModType) roundToInt (modTypeIndex);
        
        if (modType != newModType)
        {
            modType = newModType;
            
            prevWt = wt;
            modTypeCrossfade.setActive (true);
            modTypeCrossfade.resetCounter();
            
            switch (modType)
            {
                case modEnvelope:
                    setWt (env);
                    break;
                    
                case modOsc:
                    setWt (osc);
                    break;
                    
                case modGainRelative:
                    setWt (grl);
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    void setMainModOn (bool shouldBeOn)
    {
        modOn = shouldBeOn;
        if (modOn)
            prevWt = nullptr;
        else
            prevWt = wt;
        
        modTypeCrossfade.setActive (true);
    }
    
    void setModStereo (bool shouldBeStereo)
    {
        modStereo = shouldBeStereo;
    }
    
    //envelope setters
    void setAttackMs (float attackMs)
    {
        env.setAttackMs (attackMs, sampleRate);
    }
    
    void setReleaseMs (float releaseMs)
    {
        env.setReleaseMs (releaseMs, sampleRate);
    }
    
    void setThresholdDb (float threshDb)
    {
        float thresh0to1 = Utility::fromDb (threshDb);
        env.setThreshold0to1 (thresh0to1);
    }
    
    //oscillator setters
    void setOscFrequency (float freq)
    {
        osc.setFrequency (freq, sampleRate);
    }
    
    void setOscNote (float noteIndex, float octave)
    {
        float hz = Utility::noteToHz (noteIndex, octave);
        setOscFrequency (hz);
    }
    
    void setOscShape (float shapeIndex)
    {
        OscShape shape = (OscShape) roundToInt (shapeIndex);
        
        prevOsc.copyParametersFrom (osc);
        prevWt = &prevOsc;
        
        modTypeCrossfade.setActive (true);
        modTypeCrossfade.resetCounter();
        
        osc.setShape (shape);
    }
    
    //gain rel setters
    void setGainRelSensitivity0to100 (float sens0to100)
    {
        grl.setSensitivityFrom0to1 (sens0to100 / 100.0);
    }
    
    void setGainRelIntensity0to100 (float intens)
    {
        grl.setIntensityFrom0to100 (intens);
    }
    
    //tempo setters
    void setAttackTempo (float tempo)
    {
        float ms = Utility::tempoToMs (tempo, bpm, oneBarMs);
        setAttackMs (ms);
    }
    
    void setReleaseTempo (float tempo)
    {
        float ms = Utility::tempoToMs (tempo, bpm, oneBarMs);
        setReleaseMs (ms);
    }
    
    void setOscTempo (float tempo)
    {
        float hz = Utility::tempoToHz (tempo, bpm, oneBarHz);
        setOscFrequency (hz);
    }
    
    //mix
    void setMix (float mix0to100)
    {
        mixRamp.setRange (mix, mix0to100 / 100.0);
    }
    
    //=======================================================================
    //=======================================================================
    void updateBpmRelativeValues()
    {
        bool mainSync = (bool) roundToInt (*parameters.getRawParameterValue (pl.sync.paramId));
        if (mainSync)
        {
            ModType modType = (ModType) getIntFromParam (pl.mainModType.paramId);
            
            if (modType == modOsc)
            {
                Tempo lfoTempo = (Tempo) getIntFromParam (pl.mainLfoTempo.paramId);
                setOscTempo (lfoTempo);
            }
            else
            {
                Tempo tempoA = (Tempo) getIntFromParam (pl.attTempo.paramId);
                Tempo tempoR = (Tempo) getIntFromParam (pl.relTempo.paramId);
                
                setAttackTempo (tempoA);
                setReleaseTempo (tempoR);
            }
        }
    }
    //=====value tree stuff=====================================================
    int getIntFromParam (String paramId)
    {
        return roundToInt (*parameters.getRawParameterValue (paramId));
    }
    
    //======== processing =======================================================
    void applyRamps()
    {
        //reso
        double currentReso = filter->getQuality();
        resoRamp.applyRamp (currentReso);
        filter->setQuality (currentReso);
        filter1->setQuality (currentReso);
        
        //mix
        mixRamp.applyRamp (mix);
    }
    
    
    //INTERNAL SETTERS ==========================================================
    void setWt (Wavetable& newWt)
    {
        wt = &newWt;
        wt->prepare (sampleRate);
        wt->clear();
    }
    
    void setFilterTypeWithoutPrepare (FilterType ft)
    {
        filterType = ft;
        
        switch (ft)
        {
            case lowPass:
                filter = &lp;
                filter1 = &lp1;
                break;
                
            case highPass:
                filter = &hp;
                filter1 = &hp1;
                break;
                
            case bandPass:
                filter = &bp;
                filter1 = &bp1;
                break;
                
            case bandStop:
                filter = &bs;
                filter1 = &bs1;
                break;
                
            case devil:
                filter = &df;
                filter1 = &df1;
                break;
                
            default:
                break;
        }
    }
    
    void setModTypeWithoutPrepare (ModType mt)
    {
        switch (mt)
        {
            case modEnvelope:
                wt = &env;
                break;
                
            case modOsc:
                wt = &osc;
                break;
                
            case modGainRelative:
                wt = &grl;
                break;
                
            default:
                break;
        }
    }
};
