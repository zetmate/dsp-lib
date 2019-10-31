#pragma once
#include "/Users/Macbook/Desktop/juce-dev/libs/zetlib/Utility/Utility.h"

class FDAction
{
public:
    FDAction()
    {
    }
    
    virtual ~FDAction()
    {
    }
    
    // Pure virtual
    virtual void processMono (const float realIn, const float imagIn,
                              float& realOut, float& imagOut) = 0;
    
    virtual void processStereo (const float realInL, const float realInR,
                                const float imagInL, const float imagInR,
                                float& realOutL, float& realOutR,
                                float& imagOutL, float& imagOutR ) = 0;

    // Virtual
    virtual void prepare (double newSampleRate, int bufferSize)
    {
    }

    virtual void releaseResources()
    {
        resourcesReleased = true;
    }

protected:
    // Properties
    bool resourcesReleased = false;

    // Methods
    const Complex getMagnitude (float real, float imag)
    {
        return sqrtf(powf(real, 2) + powf(imag, 2));
    }
    
    const Complex getPhase (const float real, const float imag)
    {
        return atan2(imag, real);
    }
    
    const Complex synthesize (const Complex magnitude, const Complex phase)
    {
        return magnitude * exp(1i * phase);
    }
};
