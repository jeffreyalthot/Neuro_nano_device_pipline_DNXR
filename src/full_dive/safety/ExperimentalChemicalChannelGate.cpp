#include "full_dive/safety/ExperimentalChemicalChannelGate.h"

#include <algorithm>

namespace dnxr {

void ExperimentalChemicalChannelGate::setEnabled(bool enabled) { enabled_ = enabled; }
bool ExperimentalChemicalChannelGate::enabled() const { return enabled_; }
float ExperimentalChemicalChannelGate::clampMax(float value, float maximum) { return std::max(0.0f, std::min(maximum, value)); }

FullDiveChipFrame ExperimentalChemicalChannelGate::apply(const FullDiveChipFrame& frame) const {
    FullDiveChipFrame out = frame;
    if (!enabled_) {
        out.olfactory = NeuralOlfactoryVector{};
        out.gustatory = NeuralGustatoryVector{};
        return out;
    }
    out.olfactory.odorPresence01 = clampMax(out.olfactory.odorPresence01, 0.15f);
    out.olfactory.nauseaRisk01 = clampMax(out.olfactory.nauseaRisk01, 0.10f);
    out.olfactory.trigeminalContaminationRisk01 = clampMax(out.olfactory.trigeminalContaminationRisk01, 0.10f);
    out.gustatory.bitter01 = clampMax(out.gustatory.bitter01, 0.05f);
    out.gustatory.nauseaRisk01 = clampMax(out.gustatory.nauseaRisk01, 0.10f);
    out.gustatory.swallowingConflictRisk01 = 0.0f;
    return out;
}

} // namespace dnxr
