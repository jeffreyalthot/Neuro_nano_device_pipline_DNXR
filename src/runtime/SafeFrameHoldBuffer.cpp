#include "SafeFrameHoldBuffer.h"
#include <algorithm>
namespace dnxr {
void SafeFrameHoldBuffer::rememberIfSafe(const FullDiveNeuralFrame& f){
    const bool safe = f.safety.aggregateRisk<0.35f && f.safety.painSimulationBlocked &&
                      f.safety.emergencyCutoffAvailable && f.realityAnchor.anchorRequired &&
                      f.interoceptiveSafety.patientStopSignalAlive;
    if(safe){last_=f;has_=true;}
}
FullDiveNeuralFrame SafeFrameHoldBuffer::holdWithoutRiskyModalities() const {
    FullDiveNeuralFrame f=last_;
    f.vestibular.channels.clear();
    f.smellTaste.olfactory.channels.clear();
    f.smellTaste.gustatory.channels.clear();
    f.proprioception.channels.clear();
    for(auto& c:f.somatosensory.channels)c.intensity*=0.35f;
    f.realityAnchor.anchorRequired=true;
    f.realityAnchor.anchorIntensity=1.0f;
    f.safety.aggregateRisk=std::min(f.safety.aggregateRisk,0.34f);
    return f;
}
bool SafeFrameHoldBuffer::hasSafeFrame() const{return has_;}
}
