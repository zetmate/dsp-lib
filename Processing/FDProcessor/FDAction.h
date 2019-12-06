#pragma once
#include "/Users/Macbook/Desktop/juce-dev/libs/zetlib/Utility/Utility.h"

struct ComplexSignal
{
public:
    ComplexSignal (float real, float imag)
        : _real(real), _imag(imag)
    {
    }
    
    void set (float real, float imag)
    {
        _real = real;
        _imag = imag;
    }

    void set (const Complex& other)
    {
        _real = other.real();
        _imag = other.imag();
    }
    
    float real() const { return _real; }
    float imag() const { return _imag; }
private:
    float _real;
    float _imag;
};

struct ComplexSignalStereo
{
    ComplexSignal left;
    ComplexSignal right;
};

using FDActionMono = std::function<void (ComplexSignal&)>;
using FDActionStereo = std::function<void (ComplexSignalStereo&)>;

class FDAction
{
public:
    FDAction() = default;

    FDAction (const FDAction&) = delete;

    virtual ~FDAction() = default;

    // Pure virtual
    virtual void processMono (ComplexSignal& signal) = 0;
    virtual void processStereo (ComplexSignalStereo& signal) = 0;
    
    FDActionMono getMonoFunc()
    {
        return [&] (ComplexSignal& signal)
        {
            this->processMono (signal);
        };
    }
    
    FDActionStereo getStereoFunc()
    {
        return [&] (ComplexSignalStereo& signal)
        {
            this->processStereo (signal);
        };
    }

protected:
    const Complex getMagnitude (float real, float imag)
    {
        return sqrtf (powf (real, 2) + powf (imag, 2));
    }
    
    const Complex getPhase (const float real, const float imag)
    {
        return atan2 (imag, real);
    }
    
    const Complex synthesize (const Complex magnitude, const Complex phase)
    {
        return magnitude * exp (1i * phase);
    }
};
