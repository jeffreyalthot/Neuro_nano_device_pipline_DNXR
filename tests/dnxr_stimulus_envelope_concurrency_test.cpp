#include "../src/safety/NeuralStimulusSafetyEnvelope.h"

#include <cassert>
#include <thread>
#include <vector>

int main() {
    dnxr::UserNeuralCalibrationProfile profile;
    profile.maxDeltaPerFrame = 1.0f;
    dnxr::NeuralStimulusSafetyEnvelope envelope(profile);

    dnxr::SafetyDecision decision;
    decision.allowRender = true;
    decision.riskScore = 0.1f;
    decision.intensityScale = 1.0f;

    std::vector<std::thread> workers;
    for (int worker = 0; worker < 8; ++worker) {
        workers.emplace_back([&envelope, decision, worker]() {
            for (int i = 0; i < 64; ++i) {
                const auto command = envelope.buildCommand(
                    static_cast<std::uint64_t>((worker * 1000) + i),
                    {0.1f, 0.2f, 0.3f, 0.1f, 0.2f, 0.1f, 0.2f, 0.1f, 0.2f, 0.1f, 0.2f, 0.1f},
                    decision,
                    (i % 2 == 0) ? (1.0 / 90.0) : (1.0 / 60.0));
                assert(!command.boundedChannels.empty());
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }

    const auto bounded = envelope.lastBoundedChannels();
    const auto modalityBudget = envelope.lastModalityBudget();
    const auto exposure = envelope.lastExposureReport();
    assert(bounded.size() == 12U);
    assert(modalityBudget.aggregateLoad >= 0.0f);
    assert(!exposure.reason.empty());
    return 0;
}
