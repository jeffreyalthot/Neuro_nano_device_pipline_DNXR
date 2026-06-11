#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include <algorithm>
#include <cmath>
namespace dnxr { namespace { float c(float v,float fallback=0.0f){ return std::isfinite(v)?std::max(0.0f,std::min(1.0f,v)):fallback; } }
WorldFrameSanitizationResult WorldFrameSanitizer::sanitize(const NeuralChipOnlyWorldFrame& w, const NeuralChipOnlyPipelineConfig& cfg) const {
    NeuralChipOnlyWorldFrame o=w;
    if (o.frameId == 0) return {false, true, "frameId is invalid", o};
    if (!std::isfinite(o.timestampMs)) return {false, true, "timestamp is non-finite", o};
    if (o.timestampMs < 0.0) return {false, true, "timestamp is negative", o};
    o.scene.luminance01=c(o.scene.luminance01); o.scene.contrast01=c(o.scene.contrast01); o.scene.motionFlow01=c(o.scene.motionFlow01); o.scene.hazard01=c(o.scene.hazard01);
    if (!cfg.enableResearchVision) { o.scene.luminance01=o.scene.contrast01=o.scene.motionFlow01=o.scene.hazard01=0.0f; o.visualSafety.allowVisualStimulus=false; }
    o.audio.loudness01=c(o.audio.loudness01); o.audio.lowBand01=c(o.audio.lowBand01); o.audio.midBand01=c(o.audio.midBand01); o.audio.highBand01=c(o.audio.highBand01);
    if (!cfg.enableResearchAuditory) { o.audio=SpatialAudioFrame{}; o.auditorySafety.allowAuditoryStimulus=false; }
    o.contacts.pressure01=c(o.contacts.pressure01); o.contacts.area01=c(o.contacts.area01); o.contacts.slip01=c(o.contacts.slip01); o.contacts.bodyCollision01=c(o.contacts.bodyCollision01);
    o.motion.linearAcceleration01=c(o.motion.linearAcceleration01); o.motion.angularVelocity01=c(o.motion.angularVelocity01); o.motion.gravityDirection01=c(o.motion.gravityDirection01,0.5f); o.motion.sicknessRisk01=c(o.motion.sicknessRisk01);
    if (!cfg.enableResearchVestibular) o.motion=AvatarMotionFrame{};
    if (!cfg.enableExperimentalChemicalChannels) { o.odor=VirtualOdorField{}; o.taste=VirtualTasteFrame{}; o.olfactoryMap.experimentalChannelEnabled=false; o.gustatoryMap.experimentalChannelEnabled=false; }
    o.realityAnchor.orientation01=c(o.realityAnchor.orientation01,1.0f); o.realityAnchor.identityContinuity01=c(o.realityAnchor.identityContinuity01,1.0f); o.realityAnchor.sessionBoundary01=c(o.realityAnchor.sessionBoundary01,1.0f); o.realityAnchor.externalWorldRecall01=c(o.realityAnchor.externalWorldRecall01,1.0f);
    return {true, false, "accepted", o};
}

NeuralChipOnlyWorldFrame WorldFrameSanitizer::apply(const NeuralChipOnlyWorldFrame& w, const NeuralChipOnlyPipelineConfig& cfg) const {
    return sanitize(w, cfg).sanitized;
}
}
