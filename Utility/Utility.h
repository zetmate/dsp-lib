/*
 ==============================================================================
 
 Utilities.h
 Created: 21 Jan 2018 11:55:17am
 Author:  Macbook
 
 ==============================================================================
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Enums.h"
#include <complex>
#include <string>
#include <cmath>

//==================================================================================
typedef std::complex<double> Complex;
typedef dsp::WindowingFunction<float> WindowFunc;
using namespace std::complex_literals;
const double two_pi = double_Pi * 2.0;
//==================================================================================
// my utilities

class Utility
{
public:
    Utility() = delete;
    
    static double magnitude (double n)
    {
        if (n < 0)
            n *= -1;
        
        return n;
    }
    
//    static bool compareTwoDoubles (double x, double y, int numSignificantDigits)
//    {
//        return roundToInt (x * pow (10, numSignificantDigits))
//        == roundToInt (y * pow (10, numSignificantDigits));
//    }
    
    static int bitReverse4 (int n)
    {
        std::string str = std::bitset<4> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<4> (str).to_ulong();
    }
    static int bitReverse5 (int n)
    {
        std::string str = std::bitset<5> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<5> (str).to_ulong();
    }
    static int bitReverse6 (int n)
    {
        std::string str = std::bitset<6> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<6> (str).to_ulong();
    }
    static int bitReverse7 (int n)
    {
        std::string str = std::bitset<7> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<7> (str).to_ulong();
    }
    static int bitReverse8 (int n)
    {
        std::string str = std::bitset<8> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<8> (str).to_ulong();
    }
    static int bitReverse9 (int n)
    {
        std::string str = std::bitset<9> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<9> (str).to_ulong();
    }
    static int bitReverse10 (int n)
    {
        std::string str = std::bitset<10> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<10> (str).to_ulong();
    }
    static int bitReverse11 (int n)
    {
        std::string str = std::bitset<11> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<11> (str).to_ulong();
    }
    static int bitReverse12 (int n)
    {
        std::string str = std::bitset<12> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<12> (str).to_ulong();
    }
    static int bitReverse13 (int n)
    {
        std::string str = std::bitset<13> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<13> (str).to_ulong();
    }
    static int bitReverse14 (int n)
    {
        std::string str = std::bitset<14> (n).to_string();
        reverse(str.begin(), str.end());
        return (int) std::bitset<14> (str).to_ulong();
    }
    
    //==================================================================================
    // value conversions
    
    static double toDb (double value)
    {
        return 20.0 * log10 (abs (value));
    }
    
    static double fromDb (double dB)
    {
        return pow (10.0, dB / 20.0);
    }
    
    static int msToSamples (double ms, double sampleRate)
    {
        return roundToInt (sampleRate / 1000.0 * ms);
    }
    
    static float msToSamplesF (double ms, double sampleRate)
    {
        return sampleRate / 1000.0 * ms;
    }
    
    static double samplesToMs (double samples, double sampleRate)
    {
        return samples / (sampleRate / 1000.0);
    }
    
    //==================================================================================
    // curves
    
    static double fatCurveUp (double xValue, double yMin, double yMax,
                              double quality, double width, double xEnd)
    {
        double x = magnitude (xValue - xEnd);
        double y = - (pow (x, quality)) / pow (width, quality) * (yMax - yMin) + yMax;
        return y;
    }
    
    static double fatCurveDown (double xValue, double yMin, double yMax,
                                double quality, double width, double xStart)
    {
        double x = magnitude (xValue - xStart);
        double y = - (pow (x, quality)) / pow (width, quality) * (yMax - yMin) + yMax;
        return y;
    }
    
    static double thinCurveUp (double xValue, double yMin, double yMax,
                               double quality, double width, double xStart)
    {
        double x = magnitude (xValue - xStart);
        double y = pow (x, quality) / pow (width, quality) * (yMax - yMin) + yMin;
        return y;
    }
    
    static double thinCurveDown (double xValue, double yMin, double yMax,
                                 double quality, double width, double xEnd)
    {
        double x = magnitude (xValue - xEnd);
        double y = pow (x, quality) / pow (width, quality) * (yMax - yMin) + yMin;
        return y;
    }
    
    static double lineUp (double xValue, double yMin, double yMax,
                          double width, double xStart)
    {
        return (xValue - xStart) / width * (yMax - yMin) + yMin;
    }
    
    static double lineDown (double xValue, double yMin, double yMax,
                            double width, double xStart)
    {
        return (width - (xValue - xStart)) / width * (yMax - yMin) + yMin;
    }
    
    //==================================================================================
    //wavetables
    static double applyPhaseOffset (double currentSample, double numSamples,
                                    double phaseOffsetFrom0to1)
    {
        double phaseOffsetInSamples = numSamples * phaseOffsetFrom0to1;
        currentSample += roundToInt (phaseOffsetInSamples);
        
        if (currentSample >= numSamples)
            currentSample = currentSample - numSamples;
        else if (currentSample < 0)
            currentSample = numSamples + currentSample;
        
        return currentSample;
    }
    
    static double sinFrom0to1 (double currentSample, double numSamples, double phaseOffsetFrom0to1)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double x = currentSample / numSamples;
        double period = 2 * double_Pi;
        
        return sin (x * period) / 2 + 0.5;
    }
    
    static double sinFrom0to1 (double currentSample, double numSamples, double amplitude,
                               double ampOffset, double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        double x = currentSample / numSamples;
        double period = 2 * double_Pi;
        
        return sin (x * period) / (2 / correctAmplitude) + (correctAmplitude / 2) + ampOffset;
    }
    
    static double cosFrom0to1 (double currentSample, double numSamples, double phaseOffsetFrom0to1)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double x = currentSample / numSamples;
        double period = 2 * double_Pi;
        
        return cos (x * period) / 2 + 0.5;
    }
    
    static double cosFrom0to1 (double currentSample, double numSamples, double amplitude,
                               double ampOffset, double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        double x = currentSample / numSamples;
        double period = 2 * double_Pi;
        
        return cos (x * period) / (2 / correctAmplitude) + (correctAmplitude / 2) + ampOffset;
    }
    
    static double triangleFrom0to1 (double currentSample, double numSamples,
                                    double amplitude, double ampOffset,
                                    double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double hip = numSamples / 2;
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        if (currentSample < hip)
            return Utility::lineUp (currentSample, ampOffset, ampOffset + correctAmplitude,
                                    hip, 0);
        else
            return Utility::lineDown (currentSample, ampOffset, ampOffset + correctAmplitude,
                                      hip, hip);
    }
    
    static double realSawFrom0to1 (double currentSample, double numSamples,
                               double amplitude, double ampOffset,
                               double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        double x = numSamples - currentSample;
        
        return x / numSamples * correctAmplitude + ampOffset;
    }
    
    static double sawFrom0to1 (double currentSample, double numSamples,
                               double amplitude, double ampOffset,
                               double phaseOffsetFrom0to1, double sampleRate)
    {
        //apply phase offset
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        //smooth stuff
        double smoothSamples = std::min (numSamples * 0.1, sampleRate * 0.020);
        double smoothSamplesStartX = numSamples - smoothSamples;
        
        //amplitude safity
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        if (currentSample < smoothSamplesStartX)
            return Utility::lineDown (currentSample, ampOffset, ampOffset + correctAmplitude,
                                      numSamples - smoothSamples, 0);
        else
            return Utility::lineUp (currentSample, ampOffset, ampOffset + correctAmplitude,
                                    smoothSamples, smoothSamplesStartX);
    }
    
    static double realSquareFrom0to1 (double currentSample, double numSamples,
                                  double amplitude, double ampOffset,
                                  double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double hip = numSamples / 2;
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        
        if (currentSample < hip)
        {
            return correctAmplitude + ampOffset;
        }
        else
        {
            return ampOffset;
        }
    }
    
    static double squareFrom0to1 (double currentSample, double numSamples,
                                  double amplitude, double ampOffset,
                                  double phaseOffsetFrom0to1, double sampleRate)
    {
        currentSample = applyPhaseOffset (currentSample, numSamples, phaseOffsetFrom0to1);
        
        double hip = numSamples / 2.0;
        
        //smoothing stuff
        double smoothSamples = std::min (numSamples * 0.15, sampleRate * 0.020);
        //sm1
        double smoothSamplesStartX1 = hip - smoothSamples;
        double smoothSamplesEndX1 = hip;
        //sm 2
        double smoothSamplesStartX2 = numSamples - smoothSamples;
        
        double correctAmplitude = std::min (1 - ampOffset, amplitude);
        double yMax = correctAmplitude + ampOffset;
        
        if (currentSample > smoothSamplesStartX1 - 1
            && currentSample < smoothSamplesEndX1)
        {
            return lineDown (currentSample, ampOffset, yMax,
                             smoothSamples, smoothSamplesStartX1);
        }
        else if (currentSample > smoothSamplesStartX2 - 1)
        {
            return lineUp (currentSample, ampOffset, yMax,
                           smoothSamples, smoothSamplesStartX2);
        }
        else if (currentSample < hip)
        {
            return yMax;
        }
        else
        {
            return ampOffset;
        }
    }
    
    //==================================================================================
    //sample rate convertions
    
    static void oversample (const AudioSampleBuffer& buffer,
                            AudioSampleBuffer& oversampled, int factor)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        oversampled.setSize (numChannels, numSamples * factor);
        oversampled.clear();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* read = buffer.getReadPointer (channel);
            float* write = oversampled.getWritePointer (channel);
            
            for (int sample = 0; sample < numSamples; ++sample)
                write[sample * factor] = read[sample];
        }
    }
    
    static void downsample (const AudioSampleBuffer& buffer,
                            AudioSampleBuffer& downsampled, int factor)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        downsampled.setSize (numChannels, numSamples / factor);
        downsampled.clear();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* read = buffer.getReadPointer (channel);
            float* write = downsampled.getWritePointer (channel);
            
            for (int sample = 0; sample < numSamples; sample += 4)
                write[sample / 4] = read[sample];
        }
    }
    //==================================================================================
    //INTERPOLATION & FRACTIONAL DELAYS
    
    static float linearInterpolation (float fract, float sampleN, float sampleN1)
    {
        return (1 - fract) * sampleN + fract * sampleN1;
    }
    
    static float cubicInterpolation (float fract, float samplePrev, float sampleN,
                                     float sampleN1, float sampleN2)
    {
        float c3 = -samplePrev + sampleN - sampleN1 + sampleN2;
        float c2 = samplePrev - sampleN - c3;
        float c1 = sampleN1 - samplePrev;
        float c0 = sampleN;
        
        return c3 * powf (fract, 3) + c2 * powf (fract, 2) + c1 * fract + c0;
    }
    
    // Optimal 8x (6-point, 5th-order) (z-form)
    static float optimal8x_6point5order_interpolation (float fract,
                                                       float sMin2, float sMin1, float s0,
                                                       float s1, float s2, float s3)
    {
        float z = fract;
        float even1 = s1 + s0, odd1 = s1 - s0;
        float even2 = s2 + sMin1, odd2 = s2 - sMin1;
        float even3 = s3 + sMin2, odd3 = s3 - sMin2;
        
        float c0 = (even1 * 0.41660797292569773 + even2 * 0.08188468587188069
                    + even3 * 0.00150734119050266);
        
        float c1 = (odd1 * 0.32232780822726981 + odd2 * 0.21076321997422021
                    + odd3 * 0.00907649978070957);
        
        float c2 = (even1 * -0.205219993961471501 + even2 * 0.18282942057327367
                    + even3 * 0.02239057377093268);
        
        float c3 = (odd1 * -0.21022298520246224 + odd2 * 0.02176417471349534
                    + odd3 * 0.02898626924395209);
        
        float c4 = (even1 * 0.04149963966704384 + even2 * -0.06224707096203808
                    + even3 * 0.02074742969707599);
        
        float c5 = (odd1 * 0.07517133281176167 + odd2 * -0.03751837438141215
                    + odd3 * 0.00747588873055296);
        
        return ((((c5 * z + c4) * z + c3) * z + c2) * z + c1) * z + c0;
    }
    
    static float fractDelayCubicInt (const float* delayBuffer,
                                     const float delayInSamples,
                                     const int delayCounter,
                                     const int delayBufferSize)
    {
        int delayInSamplesInt = (int) ceilf (delayInSamples);
        float fraction = delayInSamplesInt - delayInSamples;
        
        //read delayed signal
        int indexPrev = 0;
        int indexN = 0;
        int indexN1 = 0;
        int indexN2 = 0;
        
        //count indexes
        
        if (delayCounter >= delayInSamplesInt)
            indexN = delayCounter - delayInSamplesInt;
        else
            indexN = (delayBufferSize) - (delayInSamplesInt - delayCounter);
        
        if (indexN == 0)
            indexPrev = delayBufferSize - 1;
        else
            indexPrev = indexN - 1;
        
        if (indexN == delayBufferSize - 1)
            indexN1 = 0;
        else
            indexN1 = indexN + 1;
        
        if (indexN1 == delayBufferSize - 1)
            indexN2 = 0;
        else
            indexN2 = indexN1 + 1;
        
        
        //intepolate sample
        
        float interpolatedSample = cubicInterpolation (fraction,
                                                       delayBuffer[indexPrev],
                                                       delayBuffer[indexN],
                                                       delayBuffer[indexN1],
                                                       delayBuffer[indexN2]);
        
        return interpolatedSample;
    }
    
    static float fractDelayComplexInt (const float* delayBuffer,
                                       const float delayInSamples,
                                       const int delayCounter,
                                       const int delayBufferSize)
    {
        int delayInSamplesInt = (int) ceilf (delayInSamples);
        float fraction = delayInSamplesInt - delayInSamples;
        
        //read delayed signal
        int sMin2 = 0;
        int sMin1 = 0;
        int s0 = 0;
        int s1 = 0;
        int s2 = 0;
        int s3 = 0;
        
        //count indexes
        
        if (delayCounter >= delayInSamplesInt)
            s0 = delayCounter - delayInSamplesInt;
        else
            s0 = (delayBufferSize) - (delayInSamplesInt - delayCounter);
        
        if (s0 == 0)
            sMin1 = delayBufferSize - 1;
        else
            sMin1 = s0 - 1;
        
        if (sMin1 == 0)
            sMin2 = delayBufferSize - 1;
        else
            sMin2 = sMin1 - 1;
        
        if (s0 == delayBufferSize - 1)
            s1 = 0;
        else
            s1 = s0 + 1;
        
        if (s1 == delayBufferSize - 1)
            s2 = 0;
        else
            s2 = s1 + 1;
        
        if (s2 == delayBufferSize - 1)
            s3 = 0;
        else
            s3 = s2 + 1;
        
        //intepolate sample
        
        float interpolatedSample = optimal8x_6point5order_interpolation (fraction,
                                                                         delayBuffer[sMin2],
                                                                         delayBuffer[sMin1],
                                                                         delayBuffer[s0],
                                                                         delayBuffer[s1],
                                                                         delayBuffer[s2],
                                                                         delayBuffer[s3]);
        return interpolatedSample;
    }
    
    //========================================================================================
    //SYNC CONVERTIONS
    static double getOne4Hz (double bpm)
    {
        return bpm / 60;
    }
    
    static float getOne4ms (double bpm)
    {
        return 1.0 / (bpm / 60.0) * 1000.0;
    }
    
    static double msToHz (float ms)
    {
        return 1000.0 / ms;
    }
    
    static double hzToMs (float hz)
    {
        return 1000.0 / hz;
    }
    
    static double tempoToHz (float tempoIndex, double bpm, float oneBarHz)
    {
        Tempo tempo = (Tempo) roundToInt (tempoIndex);
        
        const double one4Hz = getOne4Hz (bpm);
        
        float factorT = 3.0 / 2.0;
        float factorD = 0.75;
        
        double hz;
        switch (tempo)
        {
            case one64T:
                hz = one4Hz * 16 * factorT;
                break;
                
            case one64:
                hz = one4Hz * 16;
                break;
                
            case one64D:
                hz = one4Hz * 16 * factorD;
                break;
                
            case one32T:
                hz = one4Hz * 8 * factorT;
                break;
                
            case one32:
                hz = one4Hz * 8;
                break;
                
            case one32D:
                hz = one4Hz * 8 * factorD;
                break;
                
            case one16T:
                hz = one4Hz * 4 * factorT;
                break;
                
            case one16:
                hz = one4Hz * 4;
                break;
                
            case one16D:
                hz = one4Hz * 4 * factorD;
                break;
                
            case one8T:
                hz = one4Hz * 2 * factorT;
                break;
                
            case one8:
                hz = one4Hz * 2;
                break;
                
            case one8D:
                hz = one4Hz * 2 * factorD;
                break;
                
            case one4T:
                hz = one4Hz * factorT;
                break;
                
            case one4:
                hz = one4Hz;
                break;
                
            case one4D:
                hz = one4Hz * factorD;
                break;
                
            case one2T:
                hz = one4Hz / 2 * factorT;
                break;
                
            case one2:
                hz = one4Hz / 2;
                break;
                
            case one2D:
                hz = one4Hz / 2 * factorD;
                break;
                
            case one1:
                hz = oneBarHz;
                break;
                
            case two1:
                hz = oneBarHz / 2;
                break;
                
            case four1:
                hz = oneBarHz / 4;
                break;
                
            case eight1:
                hz = oneBarHz / 8;
                break;
                
            default:
                hz = 1;
                break;
        }
        return hz;
    }
    
    static float tempoToMs (float tempoIndex, double bpm, float oneBarMs)
    {
        Tempo tempo = (Tempo) roundToInt (tempoIndex);
        
        const float one4ms = getOne4ms (bpm);
        
        float factorT = 2.0 / 3.0;
        float factorD = 1.5;
        
        float ms;
        switch (tempo)
        {
            case one64T:
                ms = one4ms / 16 * factorT;
                break;
                
            case one64:
                ms = one4ms / 16;
                break;
                
            case one64D:
                ms = one4ms / 16 * factorD;
                break;
                
            case one32T:
                ms = one4ms / 8 * factorT;
                break;
                
            case one32:
                ms = one4ms / 8;
                break;
                
            case one32D:
                ms = one4ms / 8 * factorD;
                break;
                
            case one16T:
                ms = one4ms / 4 * factorT;
                break;
                
            case one16:
                ms = one4ms / 4;
                break;
                
            case one16D:
                ms = one4ms / 4 * factorD;
                break;
                
            case one8T:
                ms = one4ms / 2 * factorT;
                break;
                
            case one8:
                ms = one4ms / 2;
                break;
                
            case one8D:
                ms = one4ms / 2 * factorD;
                break;
                
            case one4T:
                ms = one4ms * factorT;
                break;
                
            case one4:
                ms = one4ms;
                break;
                
            case one4D:
                ms = one4ms * factorD;
                break;
                
            case one2T:
                ms = one4ms * 2 * factorT;
                break;
                
            case one2:
                ms = one4ms * 2;
                break;
                
            case one2D:
                ms = one4ms * 2 * factorD;
                break;
                
            case one1:
                ms = oneBarMs;
                break;
                
            case two1:
                ms = oneBarMs * 2;
                break;
                
            case four1:
                ms = oneBarMs * 4;
                break;
                
            case eight1:
                ms = oneBarMs * 8;
                break;
                
            default:
                ms = 1;
                break;
        }
        
        return ms;
    }
    
    static bool floatIsCloserToSmallerOfTwo (float val, float a, float b)
    {
        return val < (a + b) / 2.;
    }
    
    static Tempo msToTempoForTap (double ms, double bpm, float oneBarMs)
    {
        const float one4ms = getOne4ms (bpm);
        const float one64ms = one4ms / 16;
        
        const float one32ms = one64ms * 2.;
        
        if (floatIsCloserToSmallerOfTwo (ms, one64ms, one32ms))
            return one64;
        
        const float one16ms = one32ms * 2.;
        if (floatIsCloserToSmallerOfTwo (ms, one16ms, one32ms))
            return one32;
        
        const float factorT = 2. / 3.;
        const float factorD = 1.5;
        
        const float one8ms = one16ms * 2.0;
        const float one8Tms = one8ms * factorT;
        const float one4Tms = one8Tms * 2.0;
        
        if (floatIsCloserToSmallerOfTwo (ms, one16ms, one8Tms))
            return one16;
        else if (floatIsCloserToSmallerOfTwo (ms, one8Tms, one8ms))
            return one8T;
        else if (floatIsCloserToSmallerOfTwo (ms, one8ms, one4Tms))
            return one8;
        
        
        const float one8Dms = one8ms * factorD;
        const float one2Tms = one4Tms * 2.0;
        
        if (floatIsCloserToSmallerOfTwo (ms, one4Tms, one8Dms))
            return one4T;
        else if (floatIsCloserToSmallerOfTwo (ms, one8Dms, one4ms))
            return one8D;
        else if (floatIsCloserToSmallerOfTwo (ms, one4ms, one2Tms))
            return one4;
        
        
        const float one4Dms = one8Dms * 2.0;
        const float one2ms = one4ms * 2.0;
        
        if (floatIsCloserToSmallerOfTwo (ms, one2Tms, one4Dms))
            return one2T;
        else if (floatIsCloserToSmallerOfTwo (ms, one4Dms, one2ms))
            return one4D;
        
        
        if (floatIsCloserToSmallerOfTwo (ms, one2ms, oneBarMs))
        {
            if (one2ms < oneBarMs)
                return one2;
            else
                return one1;
        }
        
        const float twoBarsMs = oneBarMs * 2;
        const float fourBarsMs = oneBarMs * 4;
        const float eightBarsMs = oneBarMs * 8;
        
        if (floatIsCloserToSmallerOfTwo (ms, oneBarMs, twoBarsMs))
            return one1;
        else if (floatIsCloserToSmallerOfTwo (ms, twoBarsMs, fourBarsMs))
            return two1;
        else if (floatIsCloserToSmallerOfTwo (ms, fourBarsMs, eightBarsMs))
            return four1;
        else
            return eight1;
    }
    
    static void tapHz (Time& time, int64& lastTime,
                       bool synced, float bpm, float oneBarMs,
                       float minFreeScaleHz, int maxNumBars,
                       Slider& freeSlider, Slider& tempoSlider)
    {
        int64 currentTime = time.currentTimeMillis();
        int64 interval = currentTime - lastTime;
        
        if (synced)
        {
            if (interval < oneBarMs * (maxNumBars + 1))
            {
                Tempo tempo = Utility::msToTempoForTap (interval, bpm, oneBarMs);
                
                tempoSlider.setValue (tempo, sendNotification);
            }
        }
        else
        {
            if (interval < Utility::hzToMs (minFreeScaleHz) + 50)
                freeSlider.setValue (Utility::msToHz (interval), sendNotification);
        }
        lastTime = currentTime;
    }
    
    //=================================================================================
    static double noteToHz (float noteIndex, float octaveF, float aHz = 440.0)
    {
        Note note = (Note) roundToInt (noteIndex);
        int octave = roundToInt (octaveF);
        
        int n = 24 + (note - 1) + 12.0 * (octave - 1);
        float sem = (float) n - 69.0;
        return aHz * pow (2, sem / 12.0);
    }
    
};
