#include <Syntacts/Syntacts.hpp>
#include "Helpers.hpp"

namespace tact {

Cue::Cue() :
    m_sigs(0),
    m_env(std::move(create<Envelope>()))
{

}

Cue::Cue(Ptr<Oscillator> osc, float duration, float amplitude) :
    m_env(std::move(create<AmplitudeEnvelope>(duration, amplitude)))
{
    chain(std::move(osc));
}

Cue::Cue(Ptr<Oscillator> osc, Ptr<Envelope> env) :
    m_env(std::move(env))
{
    chain(std::move(osc));
}

Cue::Cue(Ptr<Oscillator> osc, Ptr<Oscillator> mod, Ptr<Envelope> env) :
    m_env(std::move(env))
{
    chain(std::move(osc));
    chain(std::move(mod));
}

Cue::~Cue() {

}

void Cue::setEnvelope(Ptr<Envelope> env) {
    m_env = env;
}

Ptr<Envelope> Cue::getEnvelope() const {
    return m_env;
}

void Cue::chain(Ptr<Signal> gen) {
    m_sigs.push_back(std::move(gen));
}

float Cue::sample(float t) const {
    if (m_sigs.empty())
        return 0.0f;
    float compoundSample = 1.0f;
    for (auto& g : m_sigs)
        compoundSample *= g->sample(t);
    compoundSample *= m_env->sample(t);
    return compoundSample;
}

int Cue::sampleCount(int sampleRate) const {
    return (int)(m_env->getDuration() * sampleRate);
}

};