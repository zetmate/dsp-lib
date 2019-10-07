/*
  ==============================================================================

    Ramp.h
    Created: 9 Jan 2018 9:14:24am
    Author:  Macbook

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Ramp
{
public:
    Ramp()  :
        startValue(0),
        endValue(0),
        currentValue(0),
        interval(0),
        time(0),
        numSamples(0)
    {
    }
    
    ~Ramp()
    {
    }
    
    double getEndValue() const
    {
        return endValue;
    }
    
    double getCurrentValue() const
    {
        return currentValue;
    }
    
    void setRange (double start, double end)
    {
        startValue = start;
        endValue = end;
        interval = (end - start) / numSamples;
        currentValue = start;
    }

    void setTime (double ms, double sampleRate)
    {
        time = ms;
        numSamples = (sampleRate / 1000) * ms;
    }
    
    double applyRamp (double &value)
    {
        if (interval > 0)
        {
            if (value < endValue)
                value += interval;
        }
        else
        {
            if (value > endValue)
                value += interval;
        }
        currentValue = value;
        
        return value;
    }
    
    
private:
    double startValue;
    double endValue;
    double currentValue;
    
    double interval;
    double time;
    double numSamples;
    
    double sampleRate;
};
