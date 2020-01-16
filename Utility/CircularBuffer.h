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
        
        length = numSamples;
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
        if (c > length - 1)
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
    
    /** Returns specified sample of the circular buffer */
    const Type read (int channel, int sample) const
    {
        return dataR[channel][sample];
    }
    
    /** Returns specified sample (index) of the circular buffer
        fits index into buffers length, supports negative indexes
     */
    const Type readMod (int channel, int sample) const
    {
        return dataR[channel][fitIntoRange (sample)];
    }
    
    /** Returns specified float index of the circular buffer using the most precise algorithm */
    const Type readFloatIndex (int channel, float index) const
    {
        const int indexN = floor (index);
        const float fract = index - indexN;
        
        Utility::optimal8x_6point5order_interpolation (fract, fitIntoRange (indexN - 2),
                                                            fitIntoRange (indexN - 1),
                                                            fitIntoRange (indexN),
                                                            fitIntoRange (indexN + 1),
                                                            fitIntoRange (indexN + 2),
                                                            fitIntoRange (indexN + 3));
    }
    
    /** Returns specified float index of the circular buffer using cubic interpolation algorithm */
    const Type readFloatIndexLight (int channel, float index) const
    {
        const int indexN = floor (index);
        const float fract = index - indexN;
        
        Utility::cubicInterpolation (fract, fitIntoRange (indexN - 1),
                                        fitIntoRange (indexN),
                                        fitIntoRange (indexN + 1),
                                        fitIntoRange (indexN + 2));
    }
    
    /** Returns specified float index of the circular buffer using linear interpolation algorithm */
    const Type readFloatIndexXLight (int channel, float index) const
    {
        const int indexN = floor (index);
        const float fract = index - indexN;
        
        Utility::linearInterpolation (fract, fitIntoRange (indexN), fitIntoRange (indexN + 1));
    }
    
    /** Rewrites current sample*/
    void write (int channel, Type value)
    {
        dataW[channel][c] = value;
    }
    
    /** Rewrites specified sample*/
    void write (int channel, int sample, Type value)
    {
        dataW[channel][sample] = value;
    }
    
    /** Fits specified index into the buffer's size range*/
    float fitIntoRange (float index)
    {
        return Utility::fmodPro (index, length);
    }
    
    // Getters

    /** Returns current sample index*/
    int getCounter() const
    {
        return c;
    }

private:
    int c = 0;
    int length = 0;
    const Type** dataR;
    Type** dataW;
};
