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
    void prepare (int numChannels, int numSamples)
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
    
    void keepInRange()
    {
        if (c > AudioBuffer<Type>::getNumSamples() - 1)
        {
            c = 0;
        }
    }
    
    void resetAndClear()
    {
        reset();
        AudioBuffer<Type>::clear();
    }
    
    /** Returns current sample of the circular buffer */
    const Type read (int channel)
    {
        return dataR[channel][c];
    }
    
    /** Rewrites current sample*/
    void write (int channel, Type value)
    {
        dataW[channel][c] = value;
    }

private:
    int c = 0;
    Type** dataR, dataW;
};
