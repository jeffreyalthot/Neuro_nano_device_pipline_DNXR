#pragma once

#include "../core/RuntimeCompositionRoot.h"

namespace dnxr {

class OpenSSLNeuralPacketProtector final : public INeuralPacketProtector {
public:
    std::vector<std::uint8_t> protect(const std::vector<float>& neuralPayload,
                                      const std::string& keyMaterial) const override;
    std::vector<float> unprotect(const std::vector<std::uint8_t>& protectedPayload,
                                 const std::string& keyMaterial) const override;
    bool cryptographicallySecure() const noexcept override { return true; }
    bool simulationOnly() const noexcept override { return false; }
};

} // namespace dnxr
