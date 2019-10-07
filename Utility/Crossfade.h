/*
  ==============================================================================

    Crossfade.h
    Created: 15 Dec 2018 4:49:13pm
    Author:  Macbook

  ==============================================================================
*/

#pragma once
#include "Utility.h"

class Crossfade
{
public:
    Crossfade(){}
    ~Crossfade(){}
    
    void prepare (float lengthMs, double sampleRate)
    {
        ms = lengthMs;
        numSamples = Utility::msToSamples (ms, sampleRate);
    }
    
    void setActive (bool shouldBeActive)
    {
        active = shouldBeActive;
    }
    
    bool isActive()
    {
        return active;
    }
    
    void clear()
    {
        c = 0;
        active = false;
    }
    
    void resetCounter()
    {
        c = 0;
    }
    
    void applyCrossfadeMono (float valIn, float valOut, float& result)
    {
        float factorIn = getFadeInFactor (c, numSamples);
        float factorOut = getFadeOutFactor (c, numSamples);
        
        valIn *= factorIn;
        valOut *= factorOut;
        
        c++;
        
        if (c > numSamples - 1)
        {
            c = 0;
            active = false;
        }
        
        result = getSumOfFactoredInOut (valIn, valOut);
    }
    
    void applyCrossfadeStereo (float valInL, float valInR,
                               float valOutL, float valOutR,
                               float& resultL, float& resultR)
    {
        float factorIn = getFadeInFactor (c, numSamples);
        float factorOut = getFadeOutFactor (c, numSamples);
        
        valInL *= factorIn;
        valInR *= factorIn;
        valOutL *= factorOut;
        valOutR *= factorOut;
        
        c++;
        
        if (c > numSamples - 1)
        {
            c = 0;
            active = false;
        }
        
        resultL = getSumOfFactoredInOut (valInL, valOutL);
        resultR = getSumOfFactoredInOut (valInR, valOutR);
    }
    
    static float getFadeInFactor (float sample, float numSamples)
    {
        return sqrt (sample / numSamples);
    }
    
    static float getFadeOutFactor (float sample, float numSamples)
    {
        return sqrt ((numSamples - sample) / numSamples);
    }
    
    static float getSumOfFactoredInOut (float valIn, float valOut)
    {
        //return pow (valIn, 2) + pow (valOut, 2);
        return valIn + valOut;
    }
    
private:
    bool active = false;
    int numSamples = 0;
    float ms = 0;
    int c = 0;
    
};
