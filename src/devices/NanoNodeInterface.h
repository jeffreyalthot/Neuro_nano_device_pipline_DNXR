#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace dnxr {

struct NanoNodeRuntimeConfig {
    std::string endpoint;
    std::string authToken;
    std::string vendor{"generic"};
    std::string signalProtocol{"neural-stream-v1"};
    std::size_t expectedNodeCount{100000};
};

class NanoNodeInterface {
public:
    /// Builds a real nano-device interface using secure runtime credentials.
    explicit NanoNodeInterface(NanoNodeRuntimeConfig config);

    /// Reads the latest neural signal sample window from implanted nodes.
    std::vector<float> readNeuralNodeSignals() const;

    /// Writes a stimulation vector back to neural nodes.
    void writeNeuralStimulus(const std::vector<float>& vec) const;

    /// Returns the selected vendor profile (for example: neuralink, blackrock, openbci).
    const std::string& vendorProfile() const;

private:
    /// Validates that the configuration is ready for a real hardware transaction.
    void validateRuntimeConfig() const;

    NanoNodeRuntimeConfig config_;
};

} // namespace dnxr
