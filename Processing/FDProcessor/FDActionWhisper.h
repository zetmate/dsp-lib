#pragma once
#include "FDAction"

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
        
        const signal = synthesize (magn, phase);
        
        realOut = signal.real();
        imagOut = signal.imag();
    }
private:
    Random random;
}
