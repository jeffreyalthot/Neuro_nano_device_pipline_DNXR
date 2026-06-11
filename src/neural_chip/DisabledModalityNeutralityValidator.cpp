#include "neural_chip/DisabledModalityNeutralityValidator.h"
#include "neural_chip/NeuralVectorFieldRegistry.h"
namespace dnxr {
namespace {
template <typename Fn>
bool active(Fn&& fn) {
    bool found = false;
    fn([&](const char*, const char*, float value, bool neutralWhenDisabled) {
        if (neutralWhenDisabled && value != 0.0f) found = true;
    });
    return found;
}
}
DisabledModalityNeutralityResult DisabledModalityNeutralityValidator::validate(const FullDiveChipFrame& f, const ChipCapabilities& c) const {
    if (!c.canVisual && active([&](auto v){ NeuralVectorFieldRegistry::forEachVisual(f.visual, v); })) return {false,"disabled visual modality active"};
    if (!c.canAuditory && active([&](auto v){ NeuralVectorFieldRegistry::forEachAuditory(f.auditory, v); })) return {false,"disabled auditory modality active"};
    if (!c.canOlfactory && active([&](auto v){ NeuralVectorFieldRegistry::forEachOlfactory(f.olfactory, v); })) return {false,"disabled olfactory modality active"};
    if (!c.canGustatory && active([&](auto v){ NeuralVectorFieldRegistry::forEachGustatory(f.gustatory, v); })) return {false,"disabled gustatory modality active"};
    if (!c.canVestibular && active([&](auto v){ NeuralVectorFieldRegistry::forEachVestibular(f.vestibular, v); })) return {false,"disabled vestibular modality active"};
    return {true,"accepted"};
}
}
