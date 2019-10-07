/*
  ==============================================================================

    Tempo.h
    Created: 15 Jun 2018 11:37:04am
    Author:  Macbook

  ==============================================================================
*/

#pragma once

enum FilterType
{
    lowPass = 0,
    highPass,
    bandPass,
    bandStop,
    devil
};

enum OscShape
{
    wtSin = 0,
    wtTriangle,
    wtSaw,
    wtSquare,
    wtNoise,
    wtRandom
};

enum ModType
{
    modOsc = 0,
    modEnvelope,
    modGainRelative
};

enum OscType
{
    lfo = 0,
    hfo
};

enum Tempo
{
    one64T = 0,
    one64,
    one64D,
    one32T,
    one32,
    one32D,
    one16T,
    one16,
    one16D,
    one8T,
    one8,
    one8D,
    one4T,
    one4,
    one4D,
    one2T,
    one2,
    one2D,
    one1,
    two1,
    four1,
    eight1
};

enum NoteTypeRythm
{
    normalRythm = 0,
    triplet,
    dotted
};

enum Note
{
    c = 0,
    cis,
    d,
    dis,
    e,
    f,
    fis,
    g,
    gis,
    a,
    ais,
    b
};

enum Alteration
{
    natural = 0, //бекар
    sharp,  //диез
    flat    //бемоль
};
