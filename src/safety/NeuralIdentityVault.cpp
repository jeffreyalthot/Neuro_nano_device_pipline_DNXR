#include "NeuralIdentityVault.h"

#include <functional>

namespace dnxr {

std::string NeuralIdentityVault::tokenizeIdentity(const std::string& userId) {
    const auto hash = std::hash<std::string>{}(userId + "::dnxr_identity_salt");
    const std::string token = "nid_" + std::to_string(hash);
    tokenToUser_[token] = userId;
    return token;
}

std::string NeuralIdentityVault::resolveUser(const std::string& token) const {
    const auto it = tokenToUser_.find(token);
    return it == tokenToUser_.end() ? std::string{} : it->second;
}

bool NeuralIdentityVault::isSeparated(const std::string& token, const std::string& neuralStreamId) const {
    if (token.empty() || neuralStreamId.empty()) return false;
    return neuralStreamId.find(resolveUser(token)) == std::string::npos;
}

} // namespace dnxr
