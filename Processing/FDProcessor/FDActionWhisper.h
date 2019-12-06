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
        const Complex phase = random.nextFloat() * 2.0 - 1.0;
        
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
