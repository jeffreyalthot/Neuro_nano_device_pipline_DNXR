#pragma once

#include <string>
#include <unordered_map>

namespace dnxr {

class NeuralIdentityVault {
public:
    std::string tokenizeIdentity(const std::string& userId);
    std::string resolveUser(const std::string& token) const;
    bool isSeparated(const std::string& token, const std::string& neuralStreamId) const;

private:
    std::unordered_map<std::string, std::string> tokenToUser_;
};

} // namespace dnxr
