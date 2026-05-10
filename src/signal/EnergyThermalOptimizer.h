#pragma once

#include <vector>

namespace dnxr {

class EnergyThermalOptimizer {
public:
    std::vector<float> optimize(const std::vector<float>& signal, float perceptualCriticality) const;
};

} // namespace dnxr
