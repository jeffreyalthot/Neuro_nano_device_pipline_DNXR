#include "SessionNonceManager.h"
namespace dnxr { std::uint64_t SessionNonceManager::nextNonce(){ return ++lastNonce_; } bool SessionNonceManager::verifyMonotonic(std::uint64_t nonce){ if(nonce<=lastNonce_) return false; lastNonce_=nonce; return true; } }
