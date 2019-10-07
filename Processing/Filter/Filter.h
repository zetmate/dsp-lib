/*
  ==============================================================================

    Filter.h
    Created: 1 Jan 2018 3:18:54pm
    Author:  Macbook

  ==============================================================================
*/

#pragma once

#include "../../Utility/Utility.h"

class Filter
{
public:
    Filter();
    Filter (const Filter&) = delete;
    virtual ~Filter();
    
    virtual void countCoefficients (double sampleRate) = 0;
    
    virtual float filterSignal (float input, int channel);
    
    void setFrequency (double newFeq) { frequency = newFeq; }
    void setQuality (double newQ) { quality = newQ; }
    void setBandwidthOctaves (float numOctaves) { bandwidthOctaves = numOctaves;}
    
    void copyBuffersFromAnotherFilter (Filter* other);
    
    double getFrequency() const  { return frequency; }
    double getQuality() const  { return quality; }
    double getBandwidthOctaves() const { return bandwidthOctaves; }
    float getGainCompensationFactor() const
    {
        return Utility::fromDb (-std::min (quality, 12.0));
    }
    
    
    friend class BandPassFilter;
    friend class BandStopFilter;
    friend class DevilFilter;
    friend class CrossoverBPF;
    friend class BPFcascaded;
    
    virtual void clearBuffers();
    
protected:
    double quality;
    double frequency;
    double bandwidthOctaves;
    
    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
    
private:
    AudioSampleBuffer sampleA1;
    AudioSampleBuffer sampleA2;
    AudioSampleBuffer sampleB1;
    AudioSampleBuffer sampleB2;
    
    //read pointers
    const float* readA1L;
    const float* readA1R;
    const float* readA2L;
    const float* readA2R;
    const float* readB1L;
    const float* readB1R;
    const float* readB2L;
    const float* readB2R;
    
    //write pointers
    float* writeA1L;
    float* writeA1R;
    float* writeA2L;
    float* writeA2R;
    float* writeB1L;
    float* writeB1R;
    float* writeB2L;
    float* writeB2R;
};

struct LowPassFilter  : public Filter
{
    LowPassFilter() {};
    LowPassFilter (const LowPassFilter&) = delete;
    ~LowPassFilter() {};
    
    void countCoefficients (double sampleRate) override
    {
        double angle = 2 * double_Pi * frequency / sampleRate;
        double d = 1 / quality;
        double sinAngle = sin(angle);
        double beta = 0.5 * (1 - d/2 * sinAngle) / (1 + d/2 * sinAngle);
        double gamma = (0.5 + beta) * cos(angle);
        
        a0 = static_cast<float>((0.5 + beta - gamma) / 2);
        a1 = static_cast<float>(0.5 + beta - gamma);
        a2 = a0;
        b1 = static_cast<float>(-2 * gamma);
        b2 = static_cast<float>(2 * beta);
    }
};

struct HighPassFilter  : public Filter
{
    HighPassFilter() {};
    HighPassFilter (const HighPassFilter&) = delete;
    ~HighPassFilter() {};
    
    void countCoefficients (double sampleRate) override
    {
        double angle = 2 * double_Pi * frequency / sampleRate;
        double d = 1 / quality;
        double sinAngle = sin(angle);
        double beta = 0.5 * (1 - d/2 * sinAngle) / (1 + d/2 * sinAngle);
        double gamma = (0.5 + beta) * cos(angle);
        
        a0 = static_cast<float>((0.5 + beta + gamma) / 2);
        a1 = static_cast<float>(-(0.5 + beta + gamma));
        a2 = a0;
        b1 = static_cast<float>(-2 * gamma);
        b2 = static_cast<float>(2 * beta);
    }
};

class BandPassFilter  : public Filter
{
public:
    BandPassFilter() {};
    BandPassFilter (const BandPassFilter&) = delete;
    ~BandPassFilter() {};
    
    void countCoefficients (double sampleRate) override
    {
        bandwidth = frequency * bandwidthOctaves;
        lowFrequency = frequency - bandwidth / 2;
        highFrequency = frequency + bandwidth / 2;
        
        lowFrequency = std::max ((float) lowFrequency, 100.0f);
        highFrequency = std::min ((float) highFrequency, 16000.0f);
        
        countCoefficients (sampleRate, lowFrequency, highFrequency, quality);
    }
    
    void countCoefficients (double sampleRate, double lowFreq, double highFreq, double q)
    {
        lowFrequency = lowFreq;
        highFrequency = highFreq;
        
        lowPass.frequency = highFrequency;
        lowPass.quality = q;
        lowPass.countCoefficients (sampleRate);
        
        highPass.frequency = lowFrequency;
        highPass.quality = q;
        highPass.countCoefficients (sampleRate);
    }
    
    float filterSignal (float input, int channel) override
    {
        float output = 0.0f;
        float lowPassedSignal = lowPass.filterSignal(input, channel);
        float bandPassedSignal = highPass.filterSignal(lowPassedSignal, channel);
        output = bandPassedSignal;
        return output;
    }
    
    friend class BPFcascaded;
    
private:
    LowPassFilter lowPass;
    HighPassFilter highPass;
    double lowFrequency;
    double highFrequency;
    
    double bandwidth;
};

class BPFcascaded  : public Filter
{
public:
    BPFcascaded()  : n (2)
    {
    }
    
    BPFcascaded (int num2orderFilters)  : n (num2orderFilters)
    {
    }
    
    BPFcascaded (const BPFcascaded&) = delete;
    
    ~BPFcascaded()
    {
    }
    
    void setNumFilters (int numFilters)
    {
        n = numFilters;
    }
    
    void setParameters (double lowFrequency, double highFrequency, double q)
    {
        lowFreq = lowFrequency;
        highFreq = highFrequency;
        quality = q;
    }
    
    void countCoefficients (double sampleRate) override
    {
        bpf [0].quality = quality;
        bpf [0].countCoefficients (sampleRate, lowFreq, highFreq, quality);
        
        for (int i = 1; i < n; i++)
            copyCoeffs (bpf [i], bpf [0]);
    }
    
    float filterSignal (float input, int channel) override
    {
        for (int i = 0; i < n; i++)
            input = bpf [i].filterSignal (input, channel);
        
        return input;
    }
    
private:
    BandPassFilter bpf[8];
    int n;
    double lowFreq, highFreq;
    
    void copyCoeffs (BandPassFilter& to, const BandPassFilter& from)
    {
        to.lowPass.a0 = from.lowPass.a0;
        to.lowPass.a1 = from.lowPass.a1;
        to.lowPass.a2 = from.lowPass.a2;
        to.lowPass.b1 = from.lowPass.b1;
        to.lowPass.b2 = from.lowPass.b2;
        
        to.highPass.a0 = from.highPass.a0;
        to.highPass.a1 = from.highPass.a1;
        to.highPass.a2 = from.highPass.a2;
        to.highPass.b1 = from.highPass.b1;
        to.highPass.b2 = from.highPass.b2;
    }
};

struct BandStopFilter  : public Filter
{
    BandStopFilter() {};
    BandStopFilter (const BandStopFilter&) = delete;
    ~BandStopFilter() {};
    
    void countCoefficients (double sampleRate) override
    {
        bandwidth = frequency * bandwidthOctaves;
        lowFrequency = frequency - bandwidth / 2;
        highFrequency = frequency + bandwidth / 2;
        
        
        lowFrequency = std::max ((float) lowFrequency, 100.0f);
        highFrequency = std::min ((float) highFrequency, 16000.0f);
        
        lowPass.frequency = lowFrequency;
        lowPass.quality = quality / 2;
        lowPass.countCoefficients (sampleRate);
        
        highPass.frequency = highFrequency;
        highPass.quality = quality / 2;
        highPass.countCoefficients (sampleRate);
    }
    
    float filterSignal (float input, int channel) override
    {
        float output = 0.0f;
        float lowPassedSignal = lowPass.filterSignal(input, channel);
        float highPassedSignal = highPass.filterSignal(input, channel);
        output = 0.5 * (lowPassedSignal + highPassedSignal);
        return output;
    }
    
private:
    LowPassFilter lowPass;
    HighPassFilter highPass;
    double lowFrequency;
    double highFrequency;
    
    double bandwidth;
};

class DevilFilter  : public Filter
{
public:
    DevilFilter() {}
    DevilFilter (const DevilFilter&) = delete;
    ~DevilFilter() {}
    
    void countCoefficients (double sampleRate) override
    {
        bandwidth = frequency * bandwidthOctaves;
        lowFrequency = frequency - bandwidth / 2;
        highFrequency = frequency + bandwidth / 2;
        
        lowFrequency = std::max ((float) lowFrequency, 100.0f);
        highFrequency = std::min ((float) highFrequency, 16000.0f);
        
        lowPass.frequency = highFrequency;
        lowPass.quality = quality / 2;
        lowPass.countCoefficients (sampleRate);
        
        highPass.frequency = lowFrequency;
        highPass.quality = quality / 2;
        highPass.countCoefficients (sampleRate);
    }
    
    float filterSignal (float input, int channel) override
    {
        float output = 0.0f;
        float lowPassedSignal = lowPass.filterSignal (input, channel);
        float highPassedSignal = highPass.filterSignal (input, channel);
        output = 0.5 * (lowPassedSignal + highPassedSignal);
        return output;
    }
    
private:
    LowPassFilter lowPass;
    HighPassFilter highPass;
    double lowFrequency;
    double highFrequency;
    
    double bandwidth;
};

class CrossoverLPF  : public Filter
{
public:
    CrossoverLPF() {}
    CrossoverLPF (const CrossoverLPF&) = delete;
    ~CrossoverLPF() {}
    
    void countCoefficients (double sampleRate) override
    {
        double omega = double_Pi * frequency;
        double theta = omega / sampleRate;
        double kappa = omega / tan (theta);
        double kappa2 = pow (kappa, 2);
        double omega2 = pow (omega, 2);
        double delta = kappa2 + omega2 + 2.0 * kappa * omega;
        
        a0 = omega2 / delta;
        a1 = 2.0 * a0;
        a2 = a0;
        b1 = (-2.0 * kappa2 + 2.0 * omega2) / delta;
        b2 = (-2.0 * kappa * omega + kappa2 + omega2) / delta;
    }
};

class CrossoverHPF  : public Filter
{
public:
    CrossoverHPF() {}
    CrossoverHPF (const CrossoverHPF&) = delete;
    ~CrossoverHPF() {}
    
    void countCoefficients (double sampleRate) override
    {
        double omega = double_Pi * frequency;
        double theta = omega / sampleRate;
        double kappa = omega / tan (theta);
        double kappa2 = pow (kappa, 2);
        double omega2 = pow (omega, 2);
        double delta = kappa2 + omega2 + 2.0 * kappa * omega;
        
        a0 = kappa2 / delta;
        a1 = -2.0 * a0;
        a2 = a0;
        b1 = (-2.0 * kappa2 + 2.0 * omega2) / delta;
        b2 = (-2.0 * kappa * omega + kappa2 + omega2) / delta;
    }
};

class CrossoverBPF  : public Filter
{
public:
    CrossoverBPF() {}
    CrossoverBPF (const CrossoverBPF&) = delete;
    ~CrossoverBPF() {}
    
    double getHighFrequency() const
    {
        return highFrequency;
    }
    
    double getLowFrequency() const
    {
        return lowFrequency;
    }
    
    void countCoefficients (double sampleRate) override
    {
        bandwidth = frequency * bandwidthOctaves;
        lowFrequency = frequency - bandwidth / 2;
        highFrequency = frequency + bandwidth / 2;
        
        countCoefficients (sampleRate, lowFrequency, highFrequency);
    }
    
    void countCoefficients (double sampleRate, double lowFreq, double highFreq)
    {
        lowFrequency = lowFreq;
        highFrequency = highFreq;
        
        lpf0.setFrequency (highFrequency);
        lpf1.setFrequency (highFrequency);
        hpf0.setFrequency (lowFrequency);
        hpf1.setFrequency (lowFrequency);
        
        lpf0.countCoefficients (sampleRate);
        lpf1.countCoefficients (sampleRate);
        hpf0.countCoefficients (sampleRate);
        hpf1.countCoefficients (sampleRate);
    }
    
    float filterSignal (float input, int channel) override
    {
        input = lpf0.filterSignal (input, channel);
        input = lpf1.filterSignal (input, channel);
        input = hpf0.filterSignal (input, channel);
        input = hpf1.filterSignal (input, channel);
        
        return input;
    }
    
private:
    CrossoverLPF lpf0, lpf1;
    CrossoverHPF hpf0, hpf1;
    
    double lowFrequency;
    double highFrequency;
    
    double bandwidth;
};

struct AllPassFilter  : public Filter
{
    AllPassFilter() {}
    AllPassFilter (const AllPassFilter&) = delete;
    ~AllPassFilter() {}
    
    void countCoefficients (double sampleRate) override
    {
        double c = tan (double_Pi * frequency / sampleRate);
        double alpha = (c - 1) / (c + 1);
        a0 = alpha;
        a1 = 1.0;
        a2 = 0.0;
        b1 = alpha;
        b2 = 0.0;
    }
    
    void copyCoefficients (const AllPassFilter& f)
    {
        a0 = f.a0;
        a1 = f.a1;
        a2 = f.a2;
        b1 = f.b1;
        b2 = f.b2;
    }
};

struct AllPassFilter2  : public Filter
{
    AllPassFilter2() {}
    AllPassFilter2 (const AllPassFilter2&) = delete;
    ~AllPassFilter2() {}
    
    void countCoefficients (double sampleRate) override
    {
        double c = tan (double_Pi * quality / sampleRate);
        double alpha = (c - 1) / (c + 1);
        double gamma = two_pi * frequency / sampleRate;
        double betta = -cos (gamma);
        a0 = -alpha;
        a1 = betta * (1 - alpha);
        a2 = 1.0;
        b1 = a1;
        b2 = a0;
    }
    
    void copyCoefficients (const AllPassFilter2& f)
    {
        a0 = f.a0;
        a1 = f.a1;
        a2 = f.a2;
        b1 = f.b1;
        b2 = f.b2;
    }
};

//class AntiAliasingFilter16  : public Filter
//{
//public:
//    AntiAliasingFilter16()
//    {
//    }
//
//    ~AntiAliasingFilter16()
//    {
//    }
//
//    void countCoefficients (double sampleRate) override
//    {
//        filter1.frequency = frequency;
//        filter1.quality = 0.707;
//        filter1.countCoefficients (sampleRate);
//
//        filter2.frequency = frequency;
//        filter2.quality = 0.707;
//        filter2.countCoefficients (sampleRate);
//
//        filter3.frequency = frequency;
//        filter3.quality = 0.707;
//        filter3.countCoefficients (sampleRate);
//
//        filter4.frequency = frequency;
//        filter4.quality = 0.707;
//        filter4.countCoefficients (sampleRate);
//
//        filter5.frequency = frequency;
//        filter5.quality = 0.707;
//        filter5.countCoefficients (sampleRate);
//
//        filter6.frequency = frequency;
//        filter6.quality = 0.707;
//        filter6.countCoefficients (sampleRate);
//
//        filter7.frequency = frequency;
//        filter7.quality = 0.707;
//        filter7.countCoefficients (sampleRate);
//
//        filter8.frequency = frequency;
//        filter8.quality = 0.707;
//        filter8.countCoefficients (sampleRate);
//    }
//
//    float filterSignal (float input, int channel) override
//    {
//        float output = input;
//
//        output = filter1.filterSignal (input, channel);
//        output = filter2.filterSignal (output, channel);
//        output = filter3.filterSignal (output, channel);
//        output = filter4.filterSignal (output, channel);
//        output = filter5.filterSignal (output, channel);
//        output = filter6.filterSignal (output, channel);
//        output = filter7.filterSignal (output, channel);
//        output = filter8.filterSignal (output, channel);
//
//        return output;
//    }
//
//    void clearBuffers() override
//    {
//        filter1.clearBuffers();
//        filter2.clearBuffers();
//        filter3.clearBuffers();
//        filter4.clearBuffers();
//        filter5.clearBuffers();
//        filter6.clearBuffers();
//        filter7.clearBuffers();
//        filter8.clearBuffers();
//    }
//
//private:
//    LowPassFilter filter1;
//    LowPassFilter filter2;
//    LowPassFilter filter3;
//    LowPassFilter filter4;
//    LowPassFilter filter5;
//    LowPassFilter filter6;
//    LowPassFilter filter7;
//    LowPassFilter filter8;
//};
//
//class AntiAliasingFilter32  : public Filter
//{
//public:
//    AntiAliasingFilter32()
//    {
//    }
//
//    ~AntiAliasingFilter32()
//    {
//    }
//
//    void countCoefficients (double sampleRate) override
//    {
//        filter1.frequency = frequency;
//        filter1.quality = 0.707 / 2.0f;
//        filter1.countCoefficients (sampleRate);
//
//        filter2.frequency = frequency;
//        filter2.quality = 0.707 / 2.0f;
//        filter2.countCoefficients (sampleRate);
//    }
//
//    float filterSignal (float input, int channel) override
//    {
//        float output = input;
//
//        output = filter1.filterSignal (output, channel);
//        output = filter2.filterSignal (output, channel);
//
//        return output;
//    }
//
//    void clearBuffers() override
//    {
//        filter1.clearBuffers();
//        filter2.clearBuffers();
//    }
//
//private:
//    AntiAliasingFilter16 filter1;
//    AntiAliasingFilter16 filter2;
//};




