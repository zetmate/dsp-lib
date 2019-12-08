#pragma once
#include "FDAction.h"
#include "constants.h"

class FDThread
{
public:
    FDThread() = default;
    
    FDThread (int fftOrder_, FDAction* fdAction)
    :   fftOrder(fftOrder_),
        action (fdAction)
    {
        onFFTOrderChange();
    }
    
    FDThread (const FDThread&) = delete;

    virtual ~FDThread() = default;
    
    // Prepare for play function
    void prepare (double newSampleRate, int bufferSize)
    {
        // Prepare action
        action->prepare (newSampleRate, bufferSize);

        // releaseResources
        releaseResources();
    }
    
    void releaseResources()
    {
        if (!resourcesReleased)
        {
            // Clear buffers
            clearBuffers();
            
            // Release action
            action->releaseResources();
            
            // Reset counters
            c = 0;
            
            // Reset flags
            nextFFTBlockReady = false;
            
            // Set resourcesReleased to true
            resourcesReleased = true;
        }
    }
    
    // SETTERS
    void setAll (int order, FDAction* fdAction)
    {
        setFFTOrder(order);
        setAction(fdAction);
    }
    
    void setFFTOrder (int newFFTOrder) noexcept
    {
        fftOrder = newFFTOrder;
        onFFTOrderChange();
    }
    
    void setFFTOrderSafe (int newFFTOrder) noexcept
    {
        if (newFFTOrder < fftLimits::minOrder)
        {
            fftOrder = fftLimits::minOrder;
        }
        else if (newFFTOrder > fftLimits::maxOrder)
        {
            fftOrder = fftLimits::maxOrder;
        }
        else
        {
            fftOrder = newFFTOrder;
        }
        
        onFFTOrderChange();
    }

    void setAction (FDAction* newAction)
    {
        action = newAction;
    }

    // PROCESSING FUNCTIONS
    void processSampleMono (float input, float& output);
    void processSampleStereo (float inputL, float inputR, float& outputL, float& outputR);
    
private:
    // internal
    bool resourcesReleased = false;

    int fftOrder = fftDefault::order;
    int fftSize = fftDefault::size;
    int fftDataSize = fftDefault::dataSize;
    
    std::unique_ptr<dsp::FFT> forwardFFT, inverseFFT;
    std::unique_ptr<WindowFunc> windowF, windowI;
    
    float fifo[fftLimits::maxSize];
    float fftData[fftLimits::maxDataSize];
    float outputData[fftLimits::maxSize];

    int fifoIndex = 0;
    int c = 0;
    bool nextFFTBlockReady = false;
    
    // action
    FDAction* action;
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
        zeromem (fftData, sizeof (fftData));
        
        // Clear output data
        zeromem (outputData, sizeof (outputData));
    }

    void onFFTOrderChange() noexcept
    {
        fftSize = 1 << fftOrder;
        fftDataSize = fftSize * 2;
        
        windowF = std::make_unique<WindowFunc>(fftSize, WindowFunc::hamming);
        windowI = std::make_unique<WindowFunc>(fftSize, WindowFunc::hamming);
        
        forwardFFT = std::make_unique<dsp::FFT>(fftOrder);
        inverseFFT = std::make_unique<dsp::FFT>(fftOrder);
    }
};

