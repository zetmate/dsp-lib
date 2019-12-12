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
    
    virtual void processMono (ComplexSignal& signal) override
    {
        const Complex magn = getMagnitude (signal.real(), signal.imag());
        const Complex phase = random.nextFloat() * two_pi - float_Pi;
        
        signal.set (synthesize (magn, phase));
    }

    virtual void processStereo (ComplexSignalStereo& signal) override
    {
        processMono (signal.left);
        processMono (signal.right);
    }
    
    
private:
    Random random;
};
