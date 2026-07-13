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

struct PatientVisualChipMap {
    float phospheneThreshold01 = 0.10f;
    float phospheneSpread01 = 0.20f;
    float retinotopicConfidence01 = 0.0f;
    float flickerSensitivity01 = 0.50f;
};

struct PatientMotorChipMap {
    float forwardBaseline01 = 0.0f;
    float lateralBaseline01 = 0.0f;
    float handGripBaseline01 = 0.0f;
    float tremorSensitivity01 = 0.50f;
    float decoderDrift01 = 0.0f;
};

struct PatientTouchChipMap {
    float handSensitivity01 = 0.50f;
    float footSensitivity01 = 0.50f;
    float torsoSensitivity01 = 0.50f;
    float textureSensitivity01 = 0.50f;
    float pressureThreshold01 = 0.10f;
};

struct PatientChemicalChipMap {
    float nauseaSensitivity01 = 0.75f;
    float habituationRate01 = 0.10f;
    float allowedComplexity01 = 0.0f;
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
    PatientVisualChipMap visualChip;
    PatientMotorChipMap motorChip;
    PatientTouchChipMap touchChip;
    PatientChemicalChipMap chemicalChip;

#if DNXR_SIMULATION_ONLY
    static PatientSpecificNeuralMap simulationDefaults();
#endif
};

} // namespace dnxr
