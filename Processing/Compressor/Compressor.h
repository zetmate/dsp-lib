/*
 ==============================================================================
 
 Compressor.h
 Created: 22 Jan 2018 12:30:23pm
 Author:  Macbook
 
 ==============================================================================
 */

#pragma once

#include "../../Utility/Utility.h"

class Compressor
{
public:
    Compressor()  : threshold(-1),
    ratio(INFINITY),
    attackMs(2),
    releaseMs(10),
    makeUpGain(-1),
    xg(0),
    xl(0),
    yg(0),
    yl(0),
    c(0),
    sampleRate(0),
    alphaAttack (0),
    alphaRelease(0),
    yg_prev(-122),
    yl_prev(0)
    
    {
    }
    
    ~Compressor()
    {
    }
    
    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        alphaAttack = expf (-1 / (0.001 * sampleRate * attackMs));
        alphaRelease = expf (-1 / (0.001 * sampleRate * releaseMs));
    }
    
    void clear()
    {
        yg_prev = -122;
        yl_prev = 0;
    }
    //======================================================================
    
    void setAttack (float newAttackMs)
    {
        attackMs = newAttackMs;
        alphaAttack = expf (-1 / (0.001 * sampleRate * attackMs));
    }
    
    void setRelease (float newReleaseMs)
    {
        releaseMs = newReleaseMs;
        alphaRelease = expf (-1 / (0.001 * sampleRate * releaseMs));
    }
    
    void setRatio (float newRatio)
    {
        ratio = newRatio;
    }
    
    void setThresholdDb (float newThresholdDb)
    {
        threshold = newThresholdDb;
    }
    
    void setMakeUpGainDb (float makeUpGainDb)
    {
        makeUpGain = makeUpGainDb;
    }
    //=====================================================================
    
    float getGainReductionValueForSample (const float sampleLevel)
    {
        if (sampleLevel < 0.000001)
            xg = -120;
        else
            xg = Utility::toDb (sampleLevel);
        //==========================================================
        
        //            GAIN COMPUTER (static)
        
        if (xg >= threshold)
            //ygW[sample] = threshold + (xg_prev - threshold) / ratio;
            yg = threshold + (1 / ratio - 1) * (sampleLevel - threshold);
        else
            yg = xg;
        
        xl = xg - yg;
        //==============================================================
        
        //            LEVEL DETECTOR
        
        if (xl > yl_prev)
            yl = alphaAttack * yl_prev + (1 - alphaAttack) * xl;
        else
            yl = alphaRelease * yl_prev + (1 - alphaRelease) * xl;
        //ylW[sample] = alphaRelease * yl_prev;
        
        //            float y1 = std::max(xlR[sample], alphaRelease * y1_prev
        //                                + (1 - alphaRelease) * xlR[sample]);
        //
        //            ylW[sample] = alphaAttack * yl_prev + (1 - alphaAttack) * y1;
        
        //store previous values
        yg_prev = yg;
        yl_prev = yl;
        
        //      Count gain reduction
        return Utility::fromDb (makeUpGain - yl);
    }
    
    float threshold;
    float ratio;
    float attackMs;
    float releaseMs;
    
    float makeUpGain;
    
private:
    float xg, xl, yg, yl, c;
    
    
    double sampleRate;
    
    float alphaAttack, alphaRelease;
    
    float yg_prev;
    float yl_prev;
};

