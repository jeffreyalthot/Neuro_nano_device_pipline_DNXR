#include "../src/network/NeuralCryptoTransport.h"
#include "../src/safety/NeuralIdentityVault.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::NeuralCryptoTransport crypto;
    std::vector<float> payload{0.12f, -0.89f, 1.42f, 5.77f};

    assert(crypto.isKeyValid("secure123"));
    const auto enc = crypto.encrypt(payload, "secure123");
    assert(!enc.empty());

    const auto dec = crypto.decrypt(enc, "secure123");
    assert(dec.size() == payload.size());
    for (std::size_t i = 0; i < payload.size(); ++i) {
        assert(dec[i] == payload[i]);
    }

    dnxr::NeuralIdentityVault vault;
    const std::string token = vault.tokenizeIdentity("user_alpha");
    assert(!token.empty());
    assert(vault.resolveUser(token) == "user_alpha");
    assert(vault.isSeparated(token, "stream_neural_01"));
    assert(!vault.isSeparated(token, "stream_for_user_alpha"));

    return 0;
}
