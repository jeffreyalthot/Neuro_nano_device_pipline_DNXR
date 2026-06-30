#include "BuildFingerprint.h"
#include "CryptoSignatureVerifier.h"
namespace dnxr { BuildFingerprint BuildFingerprint::current(){ return {CryptoSignatureVerifier::digestHex("dnxr-build-0.6.6"), CryptoSignatureVerifier::digestHex("dnxr-source-manifest-0.6.6")}; } }
