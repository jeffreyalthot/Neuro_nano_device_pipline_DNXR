#pragma once

#include "FullDiveNeuralFrame.h"

#include <string>
#include <vector>

namespace dnxr {

constexpr std::size_t MAX_EXPECTED_PERCEPTIONS = MAX_VISUAL_CHANNELS + MAX_AUDITORY_CHANNELS + MAX_HAPTIC_CHANNELS + MAX_PROPRIOCEPTIVE_CHANNELS + MAX_VESTIBULAR_CHANNELS + MAX_OLFACTORY_CHANNELS + MAX_GUSTATORY_CHANNELS + MAX_INTEROCEPTIVE_CHANNELS + MAX_REALITY_ANCHOR_CHANNELS;

struct ExpectedPerception {
    std::string description;
    float expectedResponse = 0.0f;
    float tolerance = 0.15f;
};

using ExpectedPerceptionSet = FixedVector<ExpectedPerception, MAX_EXPECTED_PERCEPTIONS>;

class ExpectedPerceptionModel {
public:
    ExpectedPerceptionSet predictFixed(const FullDiveNeuralFrame& frame) const;
    std::vector<ExpectedPerception> predict(const FullDiveNeuralFrame& frame) const;
};

} // namespace dnxr
