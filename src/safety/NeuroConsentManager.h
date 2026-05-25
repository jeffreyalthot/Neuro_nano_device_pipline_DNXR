#pragma once

#include <string>

namespace dnxr {

struct NeuroConsentPolicy {
    bool allowIntentDecoding{false};
    bool allowBiometricFusion{false};
    bool allowSessionReplay{false};
    std::string reason;
};

class NeuroConsentManager {
public:
    void setPolicy(const NeuroConsentPolicy& policy);
    bool isAllowed(const std::string& capability) const;
    NeuroConsentPolicy currentPolicy() const;

private:
    NeuroConsentPolicy policy_;
};

} // namespace dnxr
