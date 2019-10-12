#pragma once
#include "FDAction.h"

class FDThread
{
public:
    FDThread (FDAction* fdAction)
    :   forwardFFT (fftOrder),
        inverseFFT (fftOrder),
        windowF (fftSize, dsp::WindowingFunction<float>::hann),
        windowB (fftSize, dsp::WindowingFunction<float>::hann),
        action (fdAction)
    {
    }
    
    virtual ~FDThread()
    {
    }
    
    // Prepare for play function
    void prepare()
    {
        // releaseResources
        releaseResources();
    }
    
    void releaseResources()
    {
        if (!resourcesReleased)
        {
            clearBuffers();
            
            // Reset counters
            c = 0;
            
            // Reset flags
            nextFFTBlockReady = false;
            
            // Set resourcesReleased to true
            resourcesReleased = true;
        }
    }
    
    // SETTERS
    void setAction (FDAction* fdAction)
    {
        action = fdAction;
    }
    
    // PROCESSING FUNCTIONS
    void processSampleMono (float input, float& output);
    void processSampleStereo (float inputL, float inputR, float& outputL, float& outputR);
    
private:
    // internal
    bool resourcesReleased = false;
    
    // FFT
    enum
    {
        fftOrder = 9,
        fftSize = 1 << fftOrder,
        fftDataSize = fftSize * 2
    };
    
    dsp::FFT forwardFFT, inverseFFT;
    dsp::WindowingFunction<float> windowF, windowB;
    
    float fifo [fftSize];
    float fftData [fftDataSize];
    float outputData [fftSize];
    int fifoIndex = 0;
    int c = 0; // counter
    bool nextFFTBlockReady = false;
    
    // action
    FDAction* action;
    
    // DAW transport state object
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    // =========================================================
    
    void pushNextSampleIntoFifo (float input) noexcept
    {
        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
            }
            
            fifoIndex = 0;
        }
        
        fifo[fifoIndex++] = input;
    }
    
    void clearBuffers() noexcept
    {
        // Clear fftData
        for (int sample = 0; sample < fftDataSize; sample++)
        {
            fftData[sample] = 0;
        }
        
        // Clear output data
        for (int sample = 0; sample < fftSize; sample++)
        {
            outputData[sample] = 0;
        }
    }
};

