#include "EnergyThermalOptimizer.h"

#include <algorithm>

namespace dnxr {

std::vector<float> EnergyThermalOptimizer::optimize(const std::vector<float>& signal, float perceptualCriticality) const {
    std::vector<float> tuned = signal;
    const float powerBudget = std::clamp(0.6F + perceptualCriticality * 0.4F, 0.5F, 1.0F);
    for (float& value : tuned) {
        value = std::clamp(value * powerBudget, -1.0F, 1.0F);
    }
    return tuned;
}

} // namespace dnxr
