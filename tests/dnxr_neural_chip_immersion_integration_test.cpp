#include "../src/neurotech/DeepImmersionPlatform.h"
#include "../src/immersion/ImmersionCore.h"
#include "../src/immersion/SensoryRenderer.h"

#include <cassert>
#include <cmath>
#include <vector>

namespace {

bool normalized(float value) {
    return std::isfinite(value) && value >= 0.0F && value <= 1.0F;
}

} // namespace

int main() {
    dnxr::DeepImmersionPlatform platform;
    platform.setConsentPolicy({true, true, true, "neural_chip_immersion_test"});
    platform.setInducedVector({0.03F, -0.01F, 0.02F});

    dnxr::BiometricFrame biometrics{};
    biometrics.heartRate = 72.0F;
    biometrics.respirationRate = 14.0F;
    biometrics.galvanicSkinResponse = 0.18F;
    biometrics.eyeFocus = 0.84F;
    biometrics.postureStability = 0.81F;
    biometrics.movementEnergy = 0.28F;

    const std::vector<float> neuralFrame = {
        0.12F, 0.24F, 0.31F, 0.46F, 0.28F, 0.19F, 0.35F, 0.42F,
        0.17F, 0.23F, 0.29F, 0.34F, 0.38F, 0.41F, 0.26F, 0.21F
    };

    const dnxr::CognitiveState cognitive =
        platform.runInferenceTick(neuralFrame, biometrics, "chip_to_immersion_test");
    const dnxr::XRAdaptationFrame adaptation =
        platform.runAdaptationTick(cognitive, biometrics);
    const dnxr::XRFullBodyCommand body =
        platform.runFullBodyControlTick(neuralFrame, cognitive);
    const dnxr::NeuroPipelineDiagnostics diagnostics =
        platform.lastDiagnostics();

    dnxr::ImmersionCore immersion;
    const dnxr::NeuralSensoryFrame sensory =
        immersion.buildImmersiveFrameFromChip(
            cognitive,
            body,
            adaptation,
            diagnostics);

    const std::vector<float> channels = sensory.toVector();
    assert(channels.size() == 11U);
    for (float channel : channels) {
        assert(normalized(channel));
    }

    const dnxr::NeuralSensoryFrame cached = immersion.getLastNeuralFrame();
    assert(cached.toVector().size() == channels.size());
    assert(immersion.getFrameCount() == 1U);
    assert(immersion.getLastIntensity() >= 0.0F);
    assert(!immersion.getAvatarPoseSummary().empty());

    return 0;
}
