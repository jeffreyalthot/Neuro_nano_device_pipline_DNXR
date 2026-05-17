#include "../src/signal/SignalStreamProcessor.h"
#include "../src/signal/FeatureVectorizer.h"
#include "../src/cognition/StateInferenceEngine.h"
#include "../src/calibration/NeuralCalibrationEngine.h"
#include "../src/immersion/ImmersionCore.h"
#include "../src/safety/PhysiologicalGuard.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::SignalStreamProcessor dsp;
    dnxr::FeatureVectorizer fv;
    dnxr::StateInferenceEngine brain;
    dnxr::NeuralCalibrationEngine calib;
    dnxr::ImmersionCore immersion;
    dnxr::PhysiologicalGuard guard;

    std::vector<float> raw = {0.2F, 0.1F, 0.4F, 0.3F, 0.5F};
    auto clean = dsp.denoise(raw);
    auto features = fv.vectorize(clean);
    auto state = brain.inferState(features, 1.0);
    auto calibrated = calib.calibrateForImmersion(features, 1.0F, 1.0);
    auto frame = immersion.buildImmersiveFrame(calibrated);
    bool safe = guard.enforce({72.0F, 16.0F});

    assert(!state.empty());
    assert(!frame.empty());
    assert(safe);
    return 0;
}
