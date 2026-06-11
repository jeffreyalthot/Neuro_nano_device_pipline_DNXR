#pragma once

#include <cstdint>

namespace dnxr {

struct ModalityDosePolicy {
    float maxInstantLoad01 = 0.80f;
    float maxRollingLoad01 = 0.60f;
    float maxFatigue01 = 0.75f;
    float maxDeltaPerFrame01 = 0.05f;
    std::uint32_t recoveryFrames = 120;
};

struct FullDiveDosePolicy {
    ModalityDosePolicy visual{0.50f, 0.40f, 0.65f, 0.04f, 120};
    ModalityDosePolicy auditory{0.60f, 0.45f, 0.70f, 0.06f, 90};
    ModalityDosePolicy touch{0.70f, 0.55f, 0.75f, 0.08f, 60};
    ModalityDosePolicy proprioception{0.70f, 0.60f, 0.75f, 0.08f, 60};
    ModalityDosePolicy vestibular{0.35f, 0.30f, 0.55f, 0.03f, 180};
    ModalityDosePolicy motor{0.80f, 0.65f, 0.75f, 0.10f, 60};
    ModalityDosePolicy olfactory{0.15f, 0.10f, 0.25f, 0.01f, 240};
    ModalityDosePolicy gustatory{0.10f, 0.08f, 0.25f, 0.01f, 240};
};

class PerModalityRateAndDosePolicy {
public:
    explicit PerModalityRateAndDosePolicy(FullDiveDosePolicy policy = {});
    const FullDiveDosePolicy& policy() const;
    void setPolicy(FullDiveDosePolicy policy);

private:
    FullDiveDosePolicy policy_{};
};

} // namespace dnxr
