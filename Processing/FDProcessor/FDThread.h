#pragma once
#include "FDAction.h"

class FDThread
{
public:
    FDThread (int fftOrder_, FDAction* fdAction)
    :
        fftOrder(fftOrder_),
        fftSize (1 << fftOrder),
        fftDataSize (fftSize * 2),
        forwardFFT (fftOrder_),
        inverseFFT (fftOrder_),
        windowF (fftSize, dsp::WindowingFunction<float>::hann),
        windowI (fftSize, dsp::WindowingFunction<float>::hann),
        action (fdAction)
    {
    }
    
    FDThread (const FDThread&) = delete;

    virtual ~FDThread()
    {
    }
    
    // Prepare for play function
    void prepare (double newSampleRate, int bufferSize)
    {
        // releaseResources
        releaseResources();
    }
    
    void releaseResources()
    {
        if (!resourcesReleased)
        {
            // Clear buffers
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
    static const int maxFFTOrder = 12;
    static const int maxFFTSize = 1 << maxFFTOrder;

    int fftOrder = 10;
    int fftSize = 1 << fftOrder;
    int fftDataSize = fftSize * 2;
    
    dsp::FFT forwardFFT, inverseFFT;
    dsp::WindowingFunction<float> windowF, windowI;
    
    float fifo[maxFFTSize];
    float fftData[maxFFTSize * 2];
    float outputData[maxFFTSize];

    int fifoIndex = 0;
    int c = 0;
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

