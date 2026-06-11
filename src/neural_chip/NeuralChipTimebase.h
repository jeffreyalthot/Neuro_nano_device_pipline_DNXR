#pragma once

namespace dnxr {

struct NeuralChipTimebasePolicy {
    double defaultStepMs = 16.6667;
    bool advanceOnEveryStep = true;
};

class NeuralChipTimebase {
public:
    double nowMs() const;
    void setSimulatedNowMs(double nowMs);
    void advanceMs(double deltaMs);
    void beginStep(double worldTimestampMs);
    void setPolicy(NeuralChipTimebasePolicy policy);
    const NeuralChipTimebasePolicy& policy() const;

private:
    double nowMs_ = 0.0;
    NeuralChipTimebasePolicy policy_{};
};

} // namespace dnxr
