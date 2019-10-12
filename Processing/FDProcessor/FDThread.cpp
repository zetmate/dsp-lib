#include "FDThread.h"
#include <cmath>

void FDThread::processSampleMono (float input, float& output)
{
    // reset flag
    resourcesReleased = false;
}

void FDThread::processSampleStereo (float inputL, float inputR, float& outputL, float& outputR)
{
    // reset flag
    resourcesReleased = false;
    
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
        
        windowF.multiplyWithWindowingTable (fftData, fftSize);
        forwardFFT.performRealOnlyForwardTransform(fftData);
        
        for (int bin = 0; bin < fftSize; bin++)
        {
            const float realIn = fftData[bin];
            const float imagIn = fftData[bin + 1];
            
            float realOut, imagOut = 0.0f;
            action->processMono(realIn, imagIn, realOut, imagOut);

            fftData[bin] = realOut;
            fftData[bin] = imagOut;
        }
        
        windowI.multiplyWithWindowingTable(fftData, fftSize);
        inverseFFT.performRealOnlyInverseTransform(fftData);
        
        for (int i = 0; i < fftSize; i++)
        {
            outputData[i] = fftData[i];
        }
    }
    
    // Output the signal
    outputL = outputData[c];
    outputR = outputData[c];
    
    c++;
}

