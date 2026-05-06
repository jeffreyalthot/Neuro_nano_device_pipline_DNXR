#pragma once

#include <vector>

namespace dnxr {

class NanoNodeInterface {
public:
    /// Reads the latest neural signal sample window from implanted nodes.
    std::vector<float> readNeuralNodeSignals() const;
    /// Writes a stimulation vector back to neural nodes.
    void writeNeuralStimulus(const std::vector<float>& vec) const;
};

} // namespace dnxr
