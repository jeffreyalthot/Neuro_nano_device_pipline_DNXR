#include "neural_chip/FullDiveChipFrameDiff.h"

#include <cmath>

namespace dnxr {
namespace {
void add(std::vector<FrameFieldDelta>& deltas, const char* path, float before, float after, float eps) {
    const float delta = after - before;
    if (std::fabs(delta) > eps) deltas.push_back({path, before, after, delta});
}
} // namespace

std::vector<FrameFieldDelta> FullDiveChipFrameDiff::compare(const FullDiveChipFrame& before, const FullDiveChipFrame& after, float epsilon01) {
    std::vector<FrameFieldDelta> deltas;
#define DNXR_ADD(path, field) add(deltas, path, before.field, after.field, epsilon01)
    DNXR_ADD("visual.globalLuminance01", visual.globalLuminance01); DNXR_ADD("visual.motionFlow01", visual.motionFlow01); DNXR_ADD("visual.flickerRisk01", visual.flickerRisk01); DNXR_ADD("visual.phospheneDensity01", visual.phospheneDensity01);
    DNXR_ADD("auditory.loudness01", auditory.loudness01); DNXR_ADD("auditory.startleRisk01", auditory.startleRisk01);
    DNXR_ADD("touch.contactPressure01", touch.contactPressure01); DNXR_ADD("touch.vibrationTexture01", touch.vibrationTexture01); DNXR_ADD("touch.bodyCollision01", touch.bodyCollision01);
    DNXR_ADD("proprioception.avatarOwnership01", proprioception.avatarOwnership01); DNXR_ADD("proprioception.bodySchemaMismatch01", proprioception.bodySchemaMismatch01);
    DNXR_ADD("vestibular.angularVelocity01", vestibular.angularVelocity01); DNXR_ADD("vestibular.visualVestibularMismatch01", vestibular.visualVestibularMismatch01);
    DNXR_ADD("motorIntent.gaitIntent01", motorIntent.gaitIntent01); DNXR_ADD("motorIntent.confidence01", motorIntent.confidence01); DNXR_ADD("motorIntent.stopIntent01", motorIntent.stopIntent01);
    DNXR_ADD("olfactory.odorPresence01", olfactory.odorPresence01); DNXR_ADD("olfactory.nauseaRisk01", olfactory.nauseaRisk01);
    DNXR_ADD("gustatory.sweet01", gustatory.sweet01); DNXR_ADD("gustatory.bitter01", gustatory.bitter01); DNXR_ADD("gustatory.nauseaRisk01", gustatory.nauseaRisk01); DNXR_ADD("gustatory.swallowingConflictRisk01", gustatory.swallowingConflictRisk01);
    DNXR_ADD("safety.aggregateRisk01", safety.aggregateRisk01);
#undef DNXR_ADD
    return deltas;
}

} // namespace dnxr
