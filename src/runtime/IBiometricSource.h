#pragma once

#include "../core/RuntimeCompositionRoot.h"

namespace dnxr {

class UnavailableBiometricSource final : public IBiometricSource {
public:
    BiometricSample readBiometricSample(double timestampSeconds) override {
        BiometricSample sample;
        sample.timestampSeconds = timestampSeconds;
        sample.quality = 0.0f;
        sample.provenance = "no_certified_biometric_source_configured";
        sample.validity = BiometricValidity::Unavailable;
        return sample;
    }
};

} // namespace dnxr
