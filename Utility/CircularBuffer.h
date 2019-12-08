#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

/** Circular buffer */
template<typename Type> class CircularBuffer : public AudioBuffer<Type>
{
public:
    CircularBuffer() : AudioBuffer<Type>()
    {
    }
    
    CircularBuffer (int numChannels, int numSamples)
        : AudioBuffer<Type> (numChannels, numSamples)
    {
    }
    
    CircularBuffer (const CircularBuffer&) = delete;
    
    ~CircularBuffer()
    {
    }
    
    /** Sets buffer size and num channels + pointers to channels */
    void setSize (int numChannels, int numSamples)
    {
        AudioBuffer<Type>::setSize (numChannels, numSamples);
        dataR = AudioBuffer<Type>::getArrayOfReadPointers();
        dataW = AudioBuffer<Type>::getArrayOfWritePointers();
    }
    
    void reset()
    {
        c = 0;
    }
    
    void increment()
    {
        c++;
    }
    
    /** Resets the counter if it's larger or equals to the buffer size.
        Returns true if the counter has been reset or false otherwise.
    */
    bool keepInRange()
    {
        if (c > AudioBuffer<Type>::getNumSamples() - 1)
        {
            c = 0;
            return true;
        }
        
        return false;
    }
    
    void resetAndClear()
    {
        reset();
        AudioBuffer<Type>::clear();
    }
    
    /** Returns current sample of the circular buffer */
    const Type read (int channel) const
    {
        return dataR[channel][c];
    }
    
    /** Rewrites current sample*/
    void write (int channel, Type value)
    {
        dataW[channel][c] = value;
    }
    
    // Getters
    int getCounter() const
    {
        return c;
    }

private:
    int c = 0;
    const Type** dataR;
    Type** dataW;
};
