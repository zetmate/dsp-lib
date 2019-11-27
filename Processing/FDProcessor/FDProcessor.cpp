#include "FDProcessor.h"

void FDProcessor::processSampleMono (float input, float &output)
{
    // IMPLEMENT ME
}

void FDProcessor::processSampleStereo (float inputL, float inputR, float &outputL, float &outputR)
{
    // Keep buffers in range
    keepBuffersInRange();

    // If the buffer has been filled up with data, set isReady to true
    if (inputData.getCounter() >= delaySamples && !isReady)
    {
        isReady = true;
    }

    const float dataInL = inputData.read(0);
    const float dataInR = inputData.read(1);
    
    // Do all the processing
    float sumOutL = 0.0f;
    float sumOutR = 0.0f;

    if (isReady)
    {
        for (int i = 0; i < numThreads; i++)
        {
            if (inputData.getCounter() >= i * hopSize)
            {
                float threadOutL = 0.0f;
                float threadOutR = 0.0f;
                threads[i].processSampleStereo (dataInL, dataInR, threadOutL, threadOutR);
                sumOutL += threadOutL;
                sumOutR += threadOutR;
            }
        }
    }
    
    // Output processed signal
    outputL = sumOutL;
    outputR = sumOutR;
    
    // Store input data
    inputData.write (0, inputL);
    inputData.write (1, inputR);
    
    // Increment buffers
    incrementBuffers();
}
