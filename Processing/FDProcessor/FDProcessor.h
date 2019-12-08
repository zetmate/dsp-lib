#pragma once
#include "FDThread.h"
#include "../../Utility/CircularBuffer.h"

class FDProcessor
{
public:
    FDProcessor()
    {
        threads = new FDThread[numThreads]();
    }
    
    ~FDProcessor()
    {
        delete [] threads;
    }
    
    void prepare (double newSampleRate, int newBufferSize)
    {
        sampleRate = newSampleRate;
        bufferSize = newBufferSize;
        
        prepareThreads();
        inputData.setSize (2, pluginDelaySamples);

        releaseResources();
    }
    
    void releaseResources()
    {
        clearBuffers();

        // Release threads
        for (int i = 0; i < numThreads; i++)
        {
            threads[i].releaseResources();
        }

        isReady = false;
    }
    
    void clearBuffers()
    {
        inputData.resetAndClear();
        threadsData.resetAndClear();
        outputData.resetAndClear();
    }
    
    // PROCESSING FUNCTIONS ================================================================
    void processSampleMono (float input, float& output);
    void processSampleStereo (float inputL, float inputR, float& outputL, float& outputR);
    
    // GETTERS =============================================================================
    int getOrder() const
    {
        return fftOrder;
    }
    
    int getWindowSize() const
    {
        return windowSize;
    }
    
    int getHopSize() const
    {
        return hopSize;
    }
    
    int getNumThreads() const
    {
        return numThreads;
    }
    
    // SETTERS ====================================================================
    void setAll (int order, FDAction** actions, int numThreads)
    {
        setFFTOrder (order);
        
        // NB! includes setting actions
        setNumThreads (numThreads, actions);
    }

    void setFFTOrder (int order) noexcept
    {
        order = order < fftLimits::minOrder ? fftLimits::minOrder : order;
        order = order > fftLimits::maxOrder ? fftLimits::maxOrder : order;
        fftOrder = order;
        
        updateSizes();
    
        for (int i = 0; i < numThreads; i++)
        {
            threads[i].setFFTOrder (fftOrder);
        }
    }
    
    void setAction (FDAction** actions)
    {
        for (int i = 0; i < numThreads; i++)
        {
            actions[i]->prepare(sampleRate, bufferSize);
            threads[i].setAction (actions[i]);
        }
    }
    
    void setNumThreads (int newNumThreads, FDAction** actions)
    {
        if (numThreads < 1)
        {
            numThreads = 1;
        }
        else if (numThreads > fftLimits::maxNumThreads)
        {
            numThreads = fftLimits::maxNumThreads;
        }
        
        updateHopSize();
        
        // Resize threads
        delete [] threads;
        threads = new FDThread[numThreads]();
        
        initThreads (actions);
        prepareThreads();
    }
    
private:
    // internal
    double sampleRate;
    int bufferSize;
    const int pluginDelaySamples = fftLimits::maxSize;
    int delaySamples = pluginDelaySamples - fftDefault::size;
    bool isReady = false;
    
    // FFT
    int numThreads = fftDefault::numThreads;
    
    int fftOrder = fftDefault::order;
    int windowSize = fftDefault::size;
    int hopSize = windowSize / numThreads;
    
    // Buffers
    CircularBuffer<float> inputData;
    CircularBuffer<float> threadsData;
    CircularBuffer<float> outputData;

    // Threads
    FDThread* threads;
    
    // Private member functions =================================================
    void updateHopSize()
    {
        hopSize = windowSize / numThreads;
    }

    void updateSizes()
    {
        windowSize = 1 << fftOrder;
        updateHopSize();
        delaySamples = pluginDelaySamples - windowSize;
    }
    
    void prepareThreads()
    {
        for (int i = 0; i < numThreads; i++)
        {
            threads[i].prepare (sampleRate, bufferSize);
        }
    }
    
    // Processing
    void keepBuffersInRange()
    {
        inputData.keepInRange();
        threadsData.keepInRange();
        outputData.keepInRange();
    }
    
    void incrementBuffers()
    {
        inputData.increment();
        threadsData.increment();
        outputData.increment();
    }
    
    void initThreads (FDAction** actions)
    {
        for (int i = 0; i < numThreads; i++)
        {
            threads[i].setAll(fftOrder, actions[i]);
        }
    }
};
