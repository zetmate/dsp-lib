#pragma once
#include "FDAction.h"

class FDActionWhisper   : public FDAction
{
public:
    FDActionWhisper()
    {
    }

    ~FDActionWhisper()
    {
    }
    
    void processMono (const float realIn, const float imagIn, float& realOut, float& imagOut) override
    {
        const Complex magn = getMagnitude (realIn, imagIn);
        const Complex phase = random.nextFloat() * 2.0 - 1.0;
        
        const Complex signal = synthesize (magn, phase);
        
        realOut = signal.real();
        imagOut = signal.imag();
    }

    virtual void processStereo (const float realInL, const float realInR,
                                const float imagInL, const float imagInR,
                                float& realOutL, float& realOutR,
                                float& imagOutL, float& imagOutR ) override
    {
        processMono(realInL, imagInL, realOutL, imagOutL);
        processMono(realInR, imagInR, realOutR, imagOutR);
    }
private:
    Random random;
};
