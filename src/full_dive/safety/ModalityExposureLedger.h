#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct ModalityExposureState {
    float instantLoad01 = 0.0f;
    float rollingLoad01 = 0.0f;
    float fatigue01 = 0.0f;
    float risk01 = 0.0f;
    bool hold = false;
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
    const FullDiveExposureLedger& state() const;
    void reset();

private:
    static float clamp01(float value);
    static void update(ModalityExposureState& state, float instantLoad01, float risk01);

    FullDiveExposureLedger ledger_{};
};

} // namespace dnxr
