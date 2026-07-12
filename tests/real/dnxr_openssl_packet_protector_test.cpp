#include "network/OpenSSLNeuralPacketProtector.h"

#include <cassert>
#include <stdexcept>
#include <vector>

int main() {
    dnxr::OpenSSLNeuralPacketProtector protector;
    assert(protector.cryptographicallySecure());
    assert(!protector.simulationOnly());

    const std::string key = "hardware-bench-session-key-material";
    const std::vector<float> payload{0.1f, -0.25f, 0.5f, 1.0f};
    const auto sealed = protector.protect(payload, key);
    assert(sealed.size() > payload.size() * sizeof(float));
    const auto opened = protector.unprotect(sealed, key);
    assert(opened == payload);

    auto tampered = sealed;
    tampered.back() ^= 0x01U;
    bool rejected = false;
    try {
        (void)protector.unprotect(tampered, key);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);

    bool shortKeyRejected = false;
    try {
        (void)protector.protect(payload, "short");
    } catch (const std::invalid_argument&) {
        shortKeyRejected = true;
    }
    assert(shortKeyRejected);
    return 0;
}
