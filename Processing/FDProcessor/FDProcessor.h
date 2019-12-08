#pragma once
#include "FDThread.h"
#include "../../Utility/CircularBuffer.h"

class FDProcessor
{
public:
    FDProcessor() = default;
    
    FDProcessor (const FDProcessor&) = delete;
    
    ~FDProcessor() = default;
    
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
        threads.forEach ([] (FDThread& thread, int index)
        {
            thread.releaseResources();
        });

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
    void setAll (int order, int numThreads,
                 std::function <FDActionMono (int index)> getMono,
                 std::function <FDActionStereo (int index)> getStereo)
    {
        setFFTOrder (order);
        setNumThreads (numThreads);
        setAction (getMono, getStereo);
    }

    void setFFTOrder (int order) noexcept
    {
        order = order < fftLimits::minOrder ? fftLimits::minOrder : order;
        order = order > fftLimits::maxOrder ? fftLimits::maxOrder : order;
        fftOrder = order;
        
        updateSizes();
        setThreadsFFTOrder();
    }
    
    void setAction (std::function <FDActionMono (int index)> getMono,
                    std::function <FDActionStereo (int index)> getStereo)
    {
        // Copy actions funcs to the local static array
        for (int i = 0; i < maxNumThreads; i++)
        {
            actionsMono[i] = getMono (i);
            actionsStereo[i] = getStereo (i);
        }

        setThreadsAction();
    }
    
    void setNumThreads (int newNumThreads)
    {
        if (numThreads < 1)
        {
            numThreads = 1;
        }
        else if (numThreads > maxNumThreads)
        {
            numThreads = maxNumThreads;
        }
        
        updateHopSize();
    }
    
private:
    // internal
    double sampleRate;
    int bufferSize;
    const int pluginDelaySamples = fftLimits::maxSize;
    int delaySamples = pluginDelaySamples - fftDefault::size;
    bool isReady = false;
    
    // FFT
    static const int maxNumThreads = fftLimits::maxNumThreads;
    int numThreads = fftDefault::numThreads;
    
    int fftOrder = fftDefault::order;
    int windowSize = fftDefault::size;
    int hopSize = windowSize / numThreads;
    
    // Buffers
    CircularBuffer<float> inputData;
    CircularBuffer<float> threadsData;
    CircularBuffer<float> outputData;

    // Actions
    StaticArr<FDActionMono, maxNumThreads> actionsMono;
    StaticArr<FDActionStereo, maxNumThreads> actionsStereo;

    // Threads
    StaticArr<FDThread, maxNumThreads> threads;
    
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
        threads.forEach ([=] (FDThread& thread, int index)
        {
            thread.prepare (sampleRate, bufferSize);
        });
    }

    void setThreadsFFTOrder()
    {
        threads.forEach ([=] (FDThread& thread, int index)
        {
            thread.setFFTOrder (fftOrder);
        });
    }

    void setThreadsAction()
    {
        threads.forEach ([&] (FDThread& thread, int index)
        {
            thread.setAction (actionsMono[index], actionsStereo[index]);
        });
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
};
