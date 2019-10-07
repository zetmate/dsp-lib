/*
  ==============================================================================

    Parameter.h
    Created: 11 Nov 2018 9:45:27pm
    Author:  Macbook

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

struct Parameter
{
public:
    enum RoutingMode
    {
        sum,
        lr,
        cascade
    };
    
    Parameter (String parameterName, String nameSuffix = "", String idSuffix = "",
               AudioProcessorParameter::Category parameterCategory
               = AudioProcessorParameter::genericParameter)
    
        :   name (parameterName + nameSuffix), nameSuff (nameSuffix),
            paramId (name.toLowerCase() + idSuffix),
            category (parameterCategory)
    {
    }

    Parameter (const Parameter&) = delete;

    ~Parameter()
    {
    }
    
    const String name, nameSuff;
    const String paramId;
    AudioProcessorParameter::Category category;
};

struct ParameterFloat : public Parameter
{
public:
    ParameterFloat (String parameterName, float minValue, float maxValue,
                    float paramInterval, float sqewMidPoint, float defaultValue,
                    String nameSuff = "", String idSuff = "",
                    std::function<String (float value, int maximumStringLength)> valueToText = nullptr,
                    std::function<float (const String& text)> textToValue = nullptr)
    
        : Parameter (parameterName, nameSuff, idSuff), min (minValue), max (maxValue),
          interval (paramInterval), midPoint (sqewMidPoint), defaultVal (defaultValue),
          range (min, max, interval, 1.0),
          valueToTextFunction (valueToText), textToValueFunction (textToValue)
    {
        range.setSkewForCentre (midPoint);
    }
    
    ParameterFloat (const ParameterFloat&) = delete;
    
    ~ParameterFloat()
    {
    }
    
    NormalisableRange<float> getRange() const { return range; }
    
    const float min, max, interval, midPoint;
    float defaultVal;
    NormalisableRange <float> range;
    
    std::function<String (float value, int maximumStringLength)> valueToTextFunction;
    std::function<float (const String& text)> textToValueFunction;
};


struct ParameterChoice : public Parameter
{
public:
    ParameterChoice (String parameterName,
                     StringArray parameterChoices, int defaultChoiceIndex,
                     String nameSuff = "", String idSuff = "",
                     std::function<String (int, int)> indexToText = nullptr,
                     std::function<int (const String&)> textToIndex = nullptr)
    
        : Parameter (parameterName, nameSuff, idSuff), choices (parameterChoices),
          defaultIndex (defaultChoiceIndex),
          numChoices (choices.size()),
          indexToTextFunction (indexToText), textToIndexFunction (textToIndex)
    {
    }
    
    ParameterChoice (const ParameterChoice&) = delete;
    
    ~ParameterChoice()
    {
    }
    
    const StringArray choices;
    int defaultIndex;
    const int numChoices;
    std::function<String (int, int)> indexToTextFunction;
    std::function<int (const String&)> textToIndexFunction;
};

struct ParameterBool : public Parameter
{
public:
    ParameterBool (String parameterName,
                   String whenOn, String whenOff, bool shouldBeOnDefault,
                   String nameSuff = "", String idSuff = "",
                   std::function<String (bool, int)> boolToText = nullptr,
                   std::function<bool (const String&)> textToBool = nullptr)
    
    : Parameter (parameterName, nameSuff, idSuff), textWhenOn (whenOn),
    textWhenOff (whenOff), defaultOn (shouldBeOnDefault),
    boolToTextFunction (boolToText), textToBoolFunction (textToBool)
    {
    }
    
    ParameterBool (const ParameterBool&) = delete;
    
    ~ParameterBool()
    {
    }
    
    const String textWhenOn, textWhenOff;
    bool defaultOn;
    std::function<String (bool b, int maximumStringLength)> boolToTextFunction;
    std::function<bool (const String& text)> textToBoolFunction;
};

struct ParameterList
{
public:
    ParameterList()
    {
    }
    
    ParameterList (const ParameterList&) = delete;
    ~ParameterList(){}
    
    
    
    //=========================================================================
    //VALUE TO TEXT FUNCS
    
    static String floatToString (float value, int numDec, String suff)
    {
        if (numDec > 0)
            return String (value, numDec) + suff;
        else
            return String (roundToInt (value)) + suff;
    }
    
    static String defValueToTextDec0 (float f, int maximumStringLength)
    {
        return floatToString (f, 0, "");
    }
    
    static String defValueToTextDec1 (float f, int maximumStringLength)
    {
        return floatToString (f, 1, "");
    }
    
    static String defValueToTextDec2 (float f, int maximumStringLength)
    {
        return floatToString (f, 2, "");
    }
    
    //CUSTOM
    static String bandwidthToText (float f, int maximumStringLength)
    {
        return "Bandwidth " + String (f, 2) + " octaves";
    }
    
    static String filterFreqToText (float f, int maximumStringLength)
    {
        return floatToString (f, 0, " Hz");
    }
    
    static String lfoFreqToText (float f, int maximumStringLength)
    {
        return floatToString (f, 2, " Hz");
    }
    
    static String envMsToText (float f, int maximumStringLength)
    {
        return floatToString (f, 0, " ms");
    }
    
    static String dbToText (float f, int maximumStringLength)
    {
        return floatToString (f, 1, " dB");
    }
    
    static String percentsToText (float f, int maximumStringLength)
    {
        return floatToString (f, 0, "%");
    }
    
    static String curveShapeToText (float f, int maximumStringLength)
    {
        String suff = f < 0 ? "expo " : "log ";
        String text;
        
        if (f != 0)
            text =  suff + String (abs (f), 1);
        else
            text = "linear";
        
        return text;
    }
    
protected:
    StringArray filterTypeChoices() const
    {
        StringArray sa;
        
        sa.insert (FilterType::lowPass, "LPF");
        sa.insert (FilterType::highPass, "HPF");
        sa.insert (FilterType::bandPass, "BPF");
        sa.insert (FilterType::bandStop, "NOTCH");
        sa.insert (FilterType::devil, "DEV");
        
        return sa;
    }
    
    StringArray modTypeChoices() const
    {
        StringArray sa;
        
        sa.insert (ModType::modEnvelope, "Envelope");
        sa.insert (ModType::modOsc, "LFO");
        sa.insert (ModType::modGainRelative, "Gain");
        
        return sa;
    }
    
    StringArray oscTypeChoices() const
    {
        StringArray sa;
        
        sa.insert (OscType::lfo, "LFO");
        sa.insert (OscType::hfo, "FM");
        
        return sa;
    }
    
    StringArray oscShapeChoices() const
    {
        StringArray sa;
        
        sa.insert (OscShape::wtSin, "sin");
        sa.insert (OscShape::wtTriangle, "triangle");
        sa.insert (OscShape::wtSaw, "saw");
        sa.insert (OscShape::wtSquare, "square");
        
        return sa;
    }
    
    StringArray tempoChoices() const
    {
        StringArray sa;
        
        sa.insert (Tempo::one64T, "1/64 T");
        sa.insert (Tempo::one64, "1/64");
        sa.insert (Tempo::one64D, "1/64 D");
        sa.insert (Tempo::one32T, "1/32 T");
        sa.insert (Tempo::one32, "1/32");
        sa.insert (Tempo::one32D, "1/32 D");
        sa.insert (Tempo::one16T, "1/16 T");
        sa.insert (Tempo::one16, "1/16");
        sa.insert (Tempo::one16D, "1/16 D");
        sa.insert (Tempo::one8T, "1/8 T");
        sa.insert (Tempo::one8, "1/8");
        sa.insert (Tempo::one8D, "1/8 D");
        sa.insert (Tempo::one4T, "1/4 T");
        sa.insert (Tempo::one4, "1/4");
        sa.insert (Tempo::one4D, "1/4 D");
        sa.insert (Tempo::one2T, "1/2 T");
        sa.insert (Tempo::one2, "1/2");
        sa.insert (Tempo::one2D, "1/2 D");
        sa.insert (Tempo::one1, "1 BAR");
        sa.insert (Tempo::two1, "2 BARS");
        sa.insert (Tempo::four1, "4 BARS");
        sa.insert (Tempo::eight1, "8 BARS");
        
        return sa;
    }
    
    StringArray noteChoices() const
    {
        StringArray sa;
        
        sa.insert (Note::c, "c");
        sa.insert (Note::cis, "c#");
        sa.insert (Note::d, "d");
        sa.insert (Note::dis, "d#");
        sa.insert (Note::e, "e");
        sa.insert (Note::f, "f");
        sa.insert (Note::fis, "f#");
        sa.insert (Note::g, "g");
        sa.insert (Note::gis, "g#");
        sa.insert (Note::a, "a");
        sa.insert (Note::ais, "a#");
        sa.insert (Note::b, "b");
        
        return sa;
    }
    
    StringArray routingChoices() const
    {
        StringArray sa;
        
        sa.insert (Parameter::sum, "1 + 2");
        sa.insert (Parameter::cascade, "1 -> 2");
        sa.insert (Parameter::lr, "1L 2R");
        
        return sa;
    }
};

struct AfParamList   : public ParameterList
{
public:
    AfParamList (int afNumber, const String nameSuffix, const String idSuffix)
    :   nameSuff (nameSuffix), idSuff (idSuffix),
        filterType ("FilterType", filterTypeChoices(), lowPass, nameSuffix, idSuffix),
        bandwidth ("Bandwidth", 0.2, 4.0, 0.01, 2.0, 1.5, nameSuffix, idSuffix, bandwidthToText),
        startFreq ("Start freq", 60, 20000, 1, 2500, 7000, nameSuffix, idSuffix, filterFreqToText),
        endFreq ("End freq", 60, 20000, 1, 2500, 120, nameSuffix, idSuffix, filterFreqToText),
        reso ("Resonance", 0.7, 100, 0.1, 15, 3.3, nameSuffix, idSuffix, defValueToTextDec1),
    
        attack ("Attack", 10, 2000, 1, 500, 500, nameSuffix, idSuffix, envMsToText),
        release ("Release", 10, 2000, 1, 500, 1000, nameSuffix, idSuffix, envMsToText),
        threshold ("Threshold", -90, 0, 0.1, -12, -12, nameSuffix, idSuffix, dbToText),
        attTempo ("Attack tempo", tempoChoices(), one1, nameSuffix, idSuffix),
        relTempo ("Release tempo", tempoChoices(), two1, nameSuffix, idSuffix),
        oscShape ("Main LFO shape", oscShapeChoices(), wtSin, nameSuffix, idSuffix),
        mainLfoTempo ("Main LFO tempo", tempoChoices(), two1, nameSuffix, idSuffix),
        mainLfoFreq ("Main LFO freq", 0.1, 17, 0.01, 3, 0.5, nameSuffix, idSuffix, lfoFreqToText),
        gainRelSens ("Main GainMod sens", 0, 100, 1, 50, 10, nameSuffix, idSuffix, percentsToText),
        sync ("Main Sync", "sync", "sync", false, nameSuffix, idSuffix),
        modStereo ("Mod width", "stereo", "mono", true, nameSuffix, idSuffix),
        mainModType ("Main Mod Type", modTypeChoices(), modEnvelope, nameSuffix, idSuffix)
    {
    }
    
    AfParamList (const AfParamList&) = delete;
    
    ~AfParamList()
    {
    }
    
    const String nameSuff, idSuff;
    
    //======Filter Editor=============================
    //row 1
    ParameterChoice filterType;
    ParameterFloat bandwidth;
    //row 2
    ParameterFloat startFreq, endFreq, reso;
    
    //======Main Mod=============================
    //Envelope
    ParameterFloat attack, release, threshold;
    ParameterChoice attTempo, relTempo;
    
    //Oscillator
    ParameterChoice oscShape, mainLfoTempo;
    ParameterFloat mainLfoFreq;
    
    //Gain Relative
    ParameterFloat gainRelSens;
    
    //Other control
    ParameterBool sync, modStereo;
    ParameterChoice mainModType;
};


struct MastParamList   : public ParameterList
{
public:
    MastParamList()
    :   inGain ("Input gain", -12, 12, 0.1, 0, 0, nameSuff, idSuff, dbToText),
        outGain ("Output gain", - 12, 12, 0.1, 0, 0, nameSuff, idSuff, dbToText),
        mix ("Mix", 0, 100, 1, 50, 50, nameSuff, idSuff, percentsToText),
        routing ("Routing mode", routingChoices(), Parameter::sum),
        af1Mix ("Filter 1 mix", 0, 200, 1, 50, 100, "", "", percentsToText),
        af2Mix ("Filter 2 mix", 0, 200, 1, 50, 100, "", "", percentsToText)
    {
    }
    
    MastParamList (const MastParamList&) = delete;
    
    ~MastParamList()
    {
    }
    
    const String nameSuff = "", idSuff = "mast";
    ParameterFloat inGain, outGain, mix;
    ParameterChoice routing;
    
    ParameterFloat af1Mix, af2Mix;
};

struct AllParams    : public ParameterList
{
public:
    AllParams()  : af1 (1, " 1", "afa"), af2 (2, " 2", "afb")
    {
    }
    
    AllParams (const AllParams&) = delete;
    ~AllParams(){}
    
    AfParamList af1, af2;
    MastParamList mast;
    
private:
};
