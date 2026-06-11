#include "full_dive/safety/RealityAnchorDegradationModel.h"
#include <algorithm>
namespace dnxr {
RealityAnchorDegradationDecision RealityAnchorDegradationModel::decide(const NeuralRealityAnchorVector& a) const {
    if (a.externalWorldRecall01 < 0.65f) return {true,true,0.0f,"reality anchor emergency degradation"};
    if (a.externalWorldRecall01 < 0.75f) return {true,false,0.25f,"reality anchor progressive hold"};
    if (a.externalWorldRecall01 < 0.85f) return {false,false,0.75f,"reality anchor reduced gain"};
    return {};
}
FullDiveChipFrame RealityAnchorDegradationModel::apply(const FullDiveChipFrame& frame, const RealityAnchorDegradationDecision& d) const {
    FullDiveChipFrame out = frame;
    out.visual.globalLuminance01 *= d.globalGain01; out.visual.contrast01 *= d.globalGain01; out.visual.motionFlow01 *= d.globalGain01;
    out.vestibular.linearAcceleration01 *= d.globalGain01; out.vestibular.angularVelocity01 *= d.globalGain01; out.vestibular.motionSicknessRisk01 = std::max(out.vestibular.motionSicknessRisk01, 1.0f - d.globalGain01);
    out.motorIntent.moveForward01 *= d.globalGain01; out.motorIntent.gaitIntent01 *= d.globalGain01; out.motorIntent.stopIntent01 = std::max(out.motorIntent.stopIntent01, 1.0f - d.globalGain01);
    out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, 1.0f - d.globalGain01);
    return out;
}
}
