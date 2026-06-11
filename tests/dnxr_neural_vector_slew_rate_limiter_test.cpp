#include "neural_chip/NeuralVectorSlewRateLimiter.h"
#include <cassert>
#include <cmath>

int main(){
    dnxr::NeuralVectorSlewRateLimiter limiter; dnxr::FullDiveChipFrame a; a.frameId=1; a.timestampMs=1; a.visual.globalLuminance01=0.05f; a.auditory.loudness01=0.05f; a.touch.contactPressure01=0.05f; a.vestibular.angularVelocity01=0.05f; a.gustatory.sweet01=0.05f; a.olfactory.odorPresence01=0.05f;
    limiter.apply(a); dnxr::FullDiveChipFrame b=a; b.frameId=2; b.timestampMs=2; b.visual.globalLuminance01=0.80f; b.auditory.loudness01=0.80f; b.touch.contactPressure01=0.80f; b.vestibular.angularVelocity01=0.80f; b.gustatory.sweet01=0.80f; b.olfactory.odorPresence01=0.80f;
    auto out=limiter.apply(b); assert(std::fabs(out.visual.globalLuminance01-0.09f)<0.0001f); assert(std::fabs(out.auditory.loudness01-0.11f)<0.0001f); assert(std::fabs(out.touch.contactPressure01-0.13f)<0.0001f); assert(std::fabs(out.vestibular.angularVelocity01-0.08f)<0.0001f); assert(std::fabs(out.gustatory.sweet01-0.06f)<0.0001f); assert(std::fabs(out.olfactory.odorPresence01-0.06f)<0.0001f);
}
