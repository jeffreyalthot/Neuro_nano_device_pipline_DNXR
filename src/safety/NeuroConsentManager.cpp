#include "NeuroConsentManager.h"

namespace dnxr {

void NeuroConsentManager::setPolicy(const NeuroConsentPolicy& policy) { policy_ = policy; }

bool NeuroConsentManager::isAllowed(const std::string& capability) const {
    if (capability == "intent") return policy_.allowIntentDecoding;
    if (capability == "biometric") return policy_.allowBiometricFusion;
    if (capability == "replay") return policy_.allowSessionReplay;
    return false;
}

NeuroConsentPolicy NeuroConsentManager::currentPolicy() const { return policy_; }

} // namespace dnxr
