#include "full_dive/chip_pipeline/SensorEncoderOutputContract.h"
#include "neural_chip/NeuralVectorFieldRegistry.h"

#include <cmath>

namespace dnxr {
namespace {
bool ok(float v) { return std::isfinite(v) && v >= 0.0f && v <= 1.0f; }
SensorEncoderContractResult r(bool a, const char* e, const char* m) { return {a, e, m}; }

template <typename Fn>
SensorEncoderContractResult validateFields(const char* encoder, Fn&& fields) {
    const char* badField = nullptr;
    fields([&](const char*, const char* field, float value, bool) {
        if (!badField && !ok(value)) badField = field;
    });
    if (badField) return r(false, encoder, badField);
    return r(true, encoder, "accepted");
}

template <typename Fn>
bool disabledActive(Fn&& fields) {
    bool active = false;
    fields([&](const char*, const char*, float value, bool neutralWhenDisabled) {
        if (neutralWhenDisabled && value != 0.0f) active = true;
    });
    return active;
}
}

SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralVisualVector& v) const {
    return validateFields("visual", [&](auto visitor) { NeuralVectorFieldRegistry::forEachVisual(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralAuditoryVector& v) const {
    return validateFields("auditory", [&](auto visitor) { NeuralVectorFieldRegistry::forEachAuditory(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralTouchVector& v) const {
    return validateFields("touch", [&](auto visitor) { NeuralVectorFieldRegistry::forEachTouch(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralProprioceptionVector& v) const {
    return validateFields("proprioception", [&](auto visitor) { NeuralVectorFieldRegistry::forEachProprioception(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralVestibularVector& v) const {
    return validateFields("vestibular", [&](auto visitor) { NeuralVectorFieldRegistry::forEachVestibular(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralOlfactoryVector& v, bool enabled) const {
    if (!enabled && disabledActive([&](auto visitor) { NeuralVectorFieldRegistry::forEachOlfactory(v, visitor); })) return r(false, "olfactory", "disabled olfactory encoder output active");
    return validateFields("olfactory", [&](auto visitor) { NeuralVectorFieldRegistry::forEachOlfactory(v, visitor); });
}
SensorEncoderContractResult SensorEncoderOutputContract::validate(const NeuralGustatoryVector& v, bool enabled) const {
    if (!enabled && disabledActive([&](auto visitor) { NeuralVectorFieldRegistry::forEachGustatory(v, visitor); })) return r(false, "gustatory", "disabled gustatory encoder output active");
    return validateFields("gustatory", [&](auto visitor) { NeuralVectorFieldRegistry::forEachGustatory(v, visitor); });
}
}
