#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include <algorithm>
namespace dnxr {
FullDiveChipFrame MultisensoryPhaseLockOrchestrator::synchronize(const FullDiveChipFrame& frame) const { FullDiveChipFrame out=frame; const bool conflict=out.vestibular.visualVestibularMismatch01>0.65f || out.visual.flickerRisk01>0.65f || out.auditory.startleRisk01>0.65f; if(conflict){ out.vestibular.linearAcceleration01=0.0f; out.vestibular.angularVelocity01=0.0f; out.visual.motionFlow01=std::min(out.visual.motionFlow01,0.20f); out.auditory.loudness01=std::min(out.auditory.loudness01,0.30f); out.safety.aggregateRisk01=std::max(out.safety.aggregateRisk01,0.70f); } return out; }
}
