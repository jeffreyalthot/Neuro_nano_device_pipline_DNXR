#pragma once

namespace dnxr {

class NeuralChipTimebase {
public:
    double nowMs() const;
    void setSimulatedNowMs(double nowMs);
    void advanceMs(double deltaMs);

private:
    double nowMs_ = 0.0;
};

} // namespace dnxr
