#include "runtime/RuntimeMotorIntentStage.h"

#include <cassert>
#include <cmath>
#include <thread>
#include <vector>

namespace {

struct RecordingWorldEgress final : dnxr::IMotorWorldEgress {
    bool publishMotorIntent(std::uint64_t frameId, const std::string& sessionId, const dnxr::VirtualWorldMotorIngress& intent) override {
        ++calls;
        lastFrame = frameId;
        lastSession = sessionId;
        lastIntent = intent;
        return true;
    }
    int calls = 0;
    std::uint64_t lastFrame = 0;
    std::string lastSession;
    dnxr::VirtualWorldMotorIngress lastIntent{};
};

dnxr::RuntimePipelineFrame baseFrame() {
    dnxr::RuntimePipelineFrame frame;
    frame.cycleId = 7;
    frame.timestamp = 1.25;
    frame.worldIngressFrame.metadata.sessionId = "session-A";
    frame.bciAcquisitionFrame.external = true;
    frame.bciAcquisitionFrame.attested = true;
    frame.bciAcquisitionFrame.authenticated = true;
    frame.bciAcquisitionFrame.sequence = 7;
    frame.bciAcquisitionFrame.samples = {0.1f, 0.2f, 0.3f, 0.4f};
    frame.cleanSignals = {0.1f, 0.2f, 0.3f, 0.4f};
    frame.features = {0.7f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.4f, 0.8f, 0.0f};
    frame.diagnostics.intentConfidence = 0.9f;
    return frame;
}

} // namespace

int main() {
    {
        RecordingWorldEgress egress;
        auto out = dnxr::RuntimeMotorIntentStage::execute(baseFrame(), &egress);
        assert(out.motorIntentResult.accepted);
        assert(out.motorIntentResult.submittedToWorld);
        assert(out.motorIntentResult.provenance == "external_bci_acquisition_only");
        assert(out.motorIntentCommand.intent.moveForward01 == 0.7f);
        assert(out.sensoryFrame.empty());
        assert(egress.calls == 1);
        assert(egress.lastSession == "session-A");
    }
    {
        auto frame = baseFrame();
        frame.bciAcquisitionFrame.external = false;
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(!result.accepted);
        assert(result.neutralHold);
        assert(result.command.intent.stopIntent01 == 1.0f);
        assert(result.reason == "motor_intent_requires_valid_external_bci_acquisition");
    }
    {
        auto frame = baseFrame();
        frame.diagnostics.intentConfidence = 0.1f;
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(!result.accepted);
        assert(result.neutralHold);
        assert(result.command.intent.stopIntent01 == 1.0f);
    }
    {
        auto frame = baseFrame();
        frame.bciAcquisitionFrame.samples[1] = std::nanf("");
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(!result.accepted);
        assert(result.reason == "motor_intent_nonfinite_bci_input");
    }
    {
        auto frame = baseFrame();
        frame.bciAcquisitionFrame.qualityFlags = static_cast<std::uint32_t>(dnxr::BciAcquisitionQualityFlag::Stale);
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(!result.accepted);
        assert(result.neutralHold);
    }
    {
        auto frame = baseFrame();
        frame.features[0] = 9.0f;
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(result.accepted);
        assert(result.command.intent.moveForward01 == 1.0f);
    }
    {
        auto frame = baseFrame();
        frame.features[4] = 0.25f;
        auto a = dnxr::RuntimeMotorIntentStage::decode(frame);
        auto b = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(a.command.commandHash == b.command.commandHash);
        frame.features[4] = 0.35f;
        auto c = dnxr::RuntimeMotorIntentStage::decode(frame);
        assert(a.command.commandHash != c.command.commandHash);
    }
    {
        auto frame = baseFrame();
        dnxr::CausalMotorIntentModelIdentity badModel;
        badModel.signedManifest = false;
        auto result = dnxr::RuntimeMotorIntentStage::decode(frame, badModel);
        assert(!result.accepted);
        assert(result.reason == "motor_intent_model_manifest_invalid");
    }
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i) {
            threads.emplace_back([] {
                for (int j = 0; j < 128; ++j) {
                    auto result = dnxr::RuntimeMotorIntentStage::decode(baseFrame());
                    assert(result.accepted);
                }
            });
        }
        for (auto& thread : threads) thread.join();
    }
    return 0;
}
