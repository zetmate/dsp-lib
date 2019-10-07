/*
  ==============================================================================

    Filter.cpp
    Created: 1 Jan 2018 3:18:54pm
    Author:  Macbook

  ==============================================================================
*/

#include "Filter.h"

Filter::Filter() :  a0(0.0f), a1(0.0f), a2(0.0f), b1(0.0f), b2(0.0f),
                    sampleA1(2, 1), sampleA2(2, 1), sampleB1(2, 1), sampleB2(2, 1)
{
    clearBuffers();
    
    //apply read buffers
    readA1L = sampleA1.getReadPointer (0);
    readA1R = sampleA1.getReadPointer (1);
    readA2L = sampleA2.getReadPointer (0);
    readA2R = sampleA2.getReadPointer (1);
    readB1L = sampleB1.getReadPointer (0);
    readB1R = sampleB1.getReadPointer (1);
    readB2L = sampleB2.getReadPointer (0);
    readB2R = sampleB2.getReadPointer (1);
    
    //apply write pointers
    writeA1L = sampleA1.getWritePointer (0);
    writeA1R = sampleA1.getWritePointer (1);
    writeA2L = sampleA2.getWritePointer (0);
    writeA2R = sampleA2.getWritePointer (1);
    writeB1L = sampleB1.getWritePointer (0);
    writeB1R = sampleB1.getWritePointer (1);
    writeB2L = sampleB2.getWritePointer (0);
    writeB2R = sampleB2.getWritePointer (1);
};

Filter::~Filter()
{
};

float Filter::filterSignal (float input, int channel)
{
    float output = 0.0;
    if (channel == 0)
    {
        output = a0 * input + a1 * readA1L[0] + a2 * readA2L[0]
        - b1 * readB1L[0] - b2 * readB2L[0];
        
        writeA2L[0] = readA1L[0];
        writeB2L[0] = readB1L[0];
        writeA1L[0] = input;
        writeB1L[0] = output;
    }
    else
    {
        output = a0 * input + a1 * readA1R[0] + a2 * readA2R[0]
        - b1 * readB1R[0] - b2 * readB2R[0];
        
        writeA2R[0] = readA1R[0];
        writeB2R[0] = readB1R[0];
        writeA1R[0] = input;
        writeB1R[0] = output;
    }
    
    return output;
};

void Filter::copyBuffersFromAnotherFilter (Filter* other)
{
    for (int i = 0; i < 2; i++)
    {
        float* dataA1 = sampleA1.getWritePointer(i);
        float* dataA2 = sampleA2.getWritePointer(i);
        float* dataB1 = sampleB1.getWritePointer(i);
        float* dataB2 = sampleB2.getWritePointer(i);
        
        const float* otherA1 = other->sampleA1.getWritePointer(i);
        const float* otherA2 = other->sampleA2.getWritePointer(i);
        const float* otherB1 = other->sampleB1.getWritePointer(i);
        const float* otherB2 = other->sampleB2.getWritePointer(i);
        
        dataA1[0] = otherA1[0];
        dataA2[0] = otherA2[0];
        dataB1[0] = otherB1[0];
        dataB2[0] = otherB2[0];
    }
}

void Filter::clearBuffers()
{
    for (int i = 0; i < 2; i++)
    {
        float* dataA1 = sampleA1.getWritePointer(i);
        float* dataA2 = sampleA2.getWritePointer(i);
        float* dataB1 = sampleB1.getWritePointer(i);
        float* dataB2 = sampleB2.getWritePointer(i);
        
        dataA1[0] = 0.0f;
        dataA2[0] = 0.0f;
        dataB1[0] = 0.0f;
        dataB2[0] = 0.0f;
    }
}


