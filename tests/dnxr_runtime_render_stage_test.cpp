#include "../src/runtime/RuntimeRenderStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.rawNeuralSignals = {0.1f};
    frame.sensoryFrame = {0.2f, 0.3f};

    bool presented = false;
    bool pushed = false;
    auto out = dnxr::RuntimeRenderStage::execute(frame, dnxr::RuntimeRenderStage::Input{
        [&presented](const std::vector<float>& sensoryFrame) {
            presented = sensoryFrame.size() == 2;
        },
        []() { return true; },
        {},
        {},
        {},
        {},
        {},
        [&pushed](const std::vector<float>& sensoryFrame) { pushed = sensoryFrame.size() == 2; }
    });
    assert(out.continueCycle);
    assert(presented);
    assert(pushed);

    auto stopped = dnxr::RuntimeRenderStage::execute(frame, dnxr::RuntimeRenderStage::Input{
        [](const std::vector<float>&) {},
        []() { return true; },
        {},
        {},
        [](double, bool, float, float) {
            dnxr::WatchdogAssessment assessment;
            assessment.requiresEmergencyStop = true;
            assessment.reason = "stale_renderer";
            return assessment;
        },
        {},
        {},
        [](const std::vector<float>&) {}
    });
    assert(!stopped.continueCycle);
    assert(stopped.emergencyActive);
    return 0;
}
