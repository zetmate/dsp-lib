/*
  ==============================================================================

    AutoFilter.cpp
    Created: 9 Nov 2018 6:56:45pm
    Author:  Macbook

  ==============================================================================
*/

#include "AutoFilter.h"

void AutoFilter::processSampleMono (const float input, float& output)
{
    resourcesReleased = false;
    applyRamps();
    
    //calculate averege channel level
    float wtVal;
    
    if (modTypeCrossfade.isActive())
    {
        float valNew = wt->getCurrentValueMono (input);
        
        float valPrev;
        if (prevWt != nullptr)
            valPrev = prevWt->getCurrentValueMono (input);
        else
            valPrev = pointStart;
        
        modTypeCrossfade.applyCrossfadeMono (valNew, valPrev, wtVal);
    }
    else
    {
        if (modOn)
            wtVal = wt->getCurrentValueMono (input);
        else
            wtVal = pointStart;
    }
    
    wtVal = std::max (0.0f, std::min (1.0f, wtVal));
    
    double freq = freqRange.convertFrom0to1 (wtVal);
    
    filter->setFrequency (freq);
    filter->countCoefficients (sampleRate);
    
    float filtered = 0.0f;
    
    if (filterTypeCrossfade.isActive()) // smooth changing of the filter type
    {
        prevFilter->setFrequency (freq);
        prevFilter->setQuality (filter->getQuality());
        prevFilter->countCoefficients (sampleRate);
        
        float newV = filter->filterSignal (input, 0);
        float prevV = prevFilter->filterSignal (input, 0);
        
        filterTypeCrossfade.applyCrossfadeMono (newV, prevV, filtered);
    }
    else //usual case, when filter type is not changing
    {
        filtered = filter->filterSignal (input, 0);
    }
    
    float gc = filter->getGainCompensationFactor();
    filtered *= gc;
    
    output = filtered;
}

void AutoFilter::processSampleStereo (const float inL, const float inR,
                                      float& outL, float& outR)
{
    resourcesReleased = false;
    applyRamps();
    
    //calculate averege channel level
    float avChLev = (abs (inL) + abs (inR)) * 0.5;
    
    float wtValL = 0.0f, wtValR = 0.0f;
    
    if (modTypeCrossfade.isActive())
    {
        float valNewL = 0.0f, valNewR = 0.0f;
        wt->getCurrentValueStereo (avChLev, valNewL, valNewR);

        float valPrevL = 0.0f, valPrevR = 0.0f;
        if (prevWt != nullptr)
            prevWt->getCurrentValueStereo (avChLev, valPrevL, valPrevR);
        else
            valPrevL = valPrevR = pointStart;
        
        modTypeCrossfade.applyCrossfadeStereo (valNewL, valNewR, valPrevL, valPrevR,
                                               wtValL, wtValR);
    }
    else
    {
        if (modOn)
            wt->getCurrentValueStereo (avChLev, wtValL, wtValR);
        else
            wtValL = wtValR = pointStart;
    }
    
    wtValL = std::max (0.0f, std::min (1.0f, wtValL));
    wtValR = std::max (0.0f, std::min (1.0f, wtValR));
    
    double freqL = freqRange.convertFrom0to1 (wtValL);
    double freqR = freqRange.convertFrom0to1 (wtValR);
    
    if (!modStereo)
        freqR = freqL;
    
    filter->setFrequency (freqL);
    filter1->setFrequency (freqR);
    filter->countCoefficients (sampleRate);
    filter1->countCoefficients (sampleRate);
    
    float filteredL = 0.0f;
    float filteredR = 0.0f;
    
    if (filterTypeCrossfade.isActive()) // smooth changing of the filter type
    {
        prevFilter->setFrequency (freqL);
        prevFilter->setQuality (filter->getQuality());
        prevFilter->countCoefficients (sampleRate);
        
        prevFilter1->setFrequency (freqR);
        prevFilter1->setQuality (filter->getQuality());
        prevFilter1->countCoefficients (sampleRate);

        float newL = filter->filterSignal (inL, 0);
        float newR = filter1->filterSignal (inR, 1);
        float prevL = prevFilter->filterSignal (inL, 0);
        float prevR = prevFilter1->filterSignal (inR, 1);

        filterTypeCrossfade.applyCrossfadeStereo (newL, newR, prevL, prevR,
                                                  filteredL, filteredR);
    }
    else //usual case, when filter type is not changing
    {
        filteredL = filter->filterSignal (inL, 0);
        filteredR = filter1->filterSignal (inR, 1);
    }
    
    float gc = filter->getGainCompensationFactor();
    filteredL *= gc;
    filteredR *= gc;
    
    float wet = mix;
    float dry = 1.0 - mix;
    
    outL = filteredL * wet + inL * dry;
    outR = filteredR * wet + inR * dry;
}
