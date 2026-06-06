#include "FiveSenseCoherenceEngine.h"
#include <algorithm>
#include <cmath>
namespace dnxr {
template <typename Channels>
static float avg(const Channels& c){ if(c.empty()) return 0.0f; float s=0.0f; for(const auto& x:c) s+=x.intensity; return s/static_cast<float>(c.size()); }
template <typename Channels>
static float lat(const Channels& c){ if(c.empty()) return 0.0f; float s=0.0f; for(const auto& x:c) s+=x.latencyBudgetMs; return s/static_cast<float>(c.size()); }
FiveSenseCoherenceReport FiveSenseCoherenceEngine::evaluate(const FullDiveNeuralFrame& frame) const {
    FiveSenseCoherenceReport r;
    const float visual=avg(frame.visual.channels), vestibular=avg(frame.vestibular.channels), haptic=avg(frame.somatosensory.channels), prop=avg(frame.proprioception.channels);
    r.visualVestibularConflict = std::clamp(std::fabs(visual - vestibular) - 0.25f, 0.0f, 1.0f);
    r.audioVisualLatencySkew = std::min(1.0f, std::fabs(lat(frame.auditory.channels) - lat(frame.visual.channels)) / 80.0f);
    r.hapticProprioceptiveMismatch = std::clamp(std::fabs(haptic - prop) - 0.20f, 0.0f, 1.0f);
    r.smellTasteContextMismatch = std::clamp(std::fabs(avg(frame.smellTaste.olfactory.channels) - avg(frame.smellTaste.gustatory.channels)) - 0.15f, 0.0f, 1.0f);
    r.bodySchemaMismatch = r.hapticProprioceptiveMismatch;
    const float conflict = std::max({r.visualVestibularConflict, r.audioVisualLatencySkew, r.hapticProprioceptiveMismatch, r.smellTasteContextMismatch, r.bodySchemaMismatch});
    r.globalPerceptualCoherence = std::clamp(1.0f - conflict, 0.0f, 1.0f);
    if(conflict>0.85f) r.intervention=CoherenceIntervention::EmergencyStop; else if(conflict>0.55f) r.intervention=CoherenceIntervention::RealityAnchor; else if(conflict>0.35f) r.intervention=CoherenceIntervention::ReduceIntensity; else r.intervention=CoherenceIntervention::None;
    r.reason = conflict>0.35f ? "bounded_multisensory_conflict" : "five_sense_coherent";
    return r;
}
}
