#pragma once

namespace dnxr {

enum class NeuralChannelMaturity {
    SupportedSimulation,
    ResearchOnly,
    Experimental,
    Disabled
};

struct PatientModalityProfile {
    float sensitivity01 = 0.5f;
    float threshold01 = 0.1f;
    float maxAllowed01 = 0.80f;
    float confidence01 = 0.0f;
    bool enabled = false;
    NeuralChannelMaturity maturity = NeuralChannelMaturity::Disabled;
};

struct PatientSpecificNeuralMap {
    PatientModalityProfile visual;
    PatientModalityProfile auditory;
    PatientModalityProfile touch;
    PatientModalityProfile proprioception;
    PatientModalityProfile vestibular;
    PatientModalityProfile motor;
    PatientModalityProfile olfactory;
    PatientModalityProfile gustatory;

    static PatientSpecificNeuralMap simulationDefaults();
};

} // namespace dnxr
