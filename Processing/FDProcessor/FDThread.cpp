#include "FDThread.h"
#include <cmath>

void FDThread::processSampleMono (float input, float& output)
{
}

void FDThread::processSampleStereo (float inputL, float inputR, float& outputL, float& outputR)
{
    // Update counters
    if (c > fftSize - 1)
    {
        c = 0;
    }
    
    // Push sample to processing buffer
    pushNextSampleIntoFifo(inputL);
    
    // Process data
    if (nextFFTBlockReady)
    {
        nextFFTBlockReady = false;
        
        windowF->multiplyWithWindowingTable (fftData, fftSize);
        forwardFFT->performRealOnlyForwardTransform(fftData);
        
        for (int bin = 0; bin < fftSize; bin++)
        {
            const float real = fftData[bin];
            const float imag = fftData[bin + 1];
            
            ComplexSignal processed {real, imag};
            actionMono (processed);

            fftData[bin] = processed.real();
            fftData[bin] = processed.imag();
        }
        
        windowI->multiplyWithWindowingTable(fftData, fftSize);
        inverseFFT->performRealOnlyInverseTransform(fftData);
        
        for (int i = 0; i < fftSize; i++)
        {
            outputData[i] = fftData[i];
        }
    }
    
    // Output signal
    outputL = outputData[c];
    outputR = outputData[c];
    
    c++;
}

