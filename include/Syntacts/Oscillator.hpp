#pragma once

#include <Syntacts/Config.hpp>
#include <Syntacts/Signal.hpp>

namespace tact
{

//=============================================================================

/// Abstract base class for objects which produce a periodic, oscillating signal
class SYNTACTS_API Oscillator : public Signal
{
public:
    /// Constructs an Oscillator with a scalar frequency
    Oscillator(float frequency = 1.0f);

    /// Constructs an Oscillator with a variable frequency
    Oscillator(Ptr<Signal> frequency);

protected:
    Ptr<Signal> m_frequency;    

private:
    SERIALIZE(PARENT(Signal), MEMBER(m_frequency));
};

//=============================================================================

/// A sine wave Oscillator
class SYNTACTS_API SineWave : public Oscillator
{
public:
    /// Inherited Constructor
    using Oscillator::Oscillator;

    /// Implements sine wave oscillation
    virtual float sample(float t) const override;
private:
    SERIALIZE(PARENT(Oscillator))
};

//=============================================================================

/// A square wave Oscillator
class SYNTACTS_API SquareWave : public Oscillator
{
public:
    // Inherited Constructor
    using Oscillator::Oscillator;

    /// Implements square wave oscillation
    virtual float sample(float t) const override;
private:
    SERIALIZE(PARENT(Oscillator));
};

//=============================================================================

/// A saw wave Oscillator
class SYNTACTS_API SawWave : public Oscillator
{
public:
    // Inherited Constructor
    using Oscillator::Oscillator;

    /// Implements saw wave oscillation
    virtual float sample(float t) const override;
private:
    SERIALIZE(PARENT(Oscillator));
};

//=============================================================================

/// A triangle wave Oscillator
class SYNTACTS_API TriWave : public Oscillator
{
public:
    // Inherited Constructor
    using Oscillator::Oscillator;

    /// Implements saw wave oscillation
    virtual float sample(float t) const override;
private:
    SERIALIZE(PARENT(Oscillator));
};

//=============================================================================

/// A frequency modulated sine wave Oscillator
class SYNTACTS_API SineWaveFM : public Oscillator
{
public:
    /// Constructor
    SineWaveFM(float frequency = 1, float modulation = 1, float index = 2);

    /// Sets modulation frequency
    void setModulation(float modulation);

    /// Sets modulation index
    void setIndex(float index);

    /// Implements FM sine wave oscillation
    virtual float sample(float t) const override;

private:
    std::atomic<float> m_modulation; ///< modulation frequency
    std::atomic<float> m_index;      ///< modulation index
private:
    SERIALIZE(PARENT(Oscillator), MEMBER(m_modulation), MEMBER(m_index))
};

/// A linear chirp Oscillator
class SYNTACTS_API Chirp : public Oscillator
{
public:
    /// Constructor
    Chirp(float frequency=1, float chirpyness=1);

    /// Sets the chirpyness (i.e. the chirp rate or (f1-f0)/T)
    void setChirpyness(float chirpyness);

    /// Implements chirp wave oscillation
    virtual float sample(float t) const override;

private:
    std::atomic<float> m_chirpyness; ///< the chirp rate or (f1-f0)/T
private:
    SERIALIZE(PARENT(Oscillator), MEMBER(m_chirpyness))
};

//=============================================================================

/// A square wave with adjustable frequency and duty cycle
class SYNTACTS_API PulseTrain : public Oscillator
{
public:
    /// Constructor
    PulseTrain(float frequency = 1, float dutyCycle = 0.5f);

    /// Sets the duty cycle between 0 and 1
    void setDutyCycle(float dutyCycle);

    /// Implements pulse train oscillation
    virtual float sample(float t) const override;

private:
    std::atomic<float> m_dutyCycle;
    std::atomic<float> m_period;
private:
    SERIALIZE(PARENT(Oscillator), MEMBER(m_dutyCycle), MEMBER(m_period));
};

} // namespace tact