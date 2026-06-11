#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "full_dive/safety/PerModalityRateAndDosePolicy.h"
#include <cstdint>

namespace dnxr {

struct ModalityExposureState {
    float instantLoad01 = 0.0f;
    float rollingLoad01 = 0.0f;
    float fatigue01 = 0.0f;
    float risk01 = 0.0f;
    bool hold = false;
    std::uint32_t holdFrames = 0;
};

struct ExposureHoldPolicy {
    float holdOn01 = 0.40f;
    float holdOff01 = 0.30f;
    std::uint32_t minimumHoldFrames = 120;
    float recoveryRate01 = 0.01f;
};

struct FullDiveExposureLedger {
    ModalityExposureState visual;
    ModalityExposureState auditory;
    ModalityExposureState touch;
    ModalityExposureState proprioception;
    ModalityExposureState vestibular;
    ModalityExposureState motor;
    ModalityExposureState olfactory;
    ModalityExposureState gustatory;
};

class ModalityExposureLedger {
public:
    FullDiveChipFrame apply(const FullDiveChipFrame& frame);
    FullDiveChipFrame apply(const FullDiveChipFrame& frame, const FullDiveDosePolicy& policy);
    const FullDiveExposureLedger& state() const;
    void reset();

private:
    static float clamp01(float value);
    static void update(ModalityExposureState& state, float instantLoad01, float risk01);
    static void applyHoldPolicy(ModalityExposureState& state, const ExposureHoldPolicy& policy);

    ExposureHoldPolicy holdPolicy_{};
    FullDiveExposureLedger ledger_{};
};

} // namespace dnxr
