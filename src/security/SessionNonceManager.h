#pragma once
#include <cstdint>
namespace dnxr { class SessionNonceManager { public: std::uint64_t nextNonce(); bool verifyMonotonic(std::uint64_t nonce); private: std::uint64_t lastNonce_=0; }; }
