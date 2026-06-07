#include "ModalityPhaseLockEngine.h"
#include <algorithm>
#include <cmath>
namespace dnxr {
PhaseLockReport ModalityPhaseLockEngine::evaluate(const FullDiveNeuralFrame& f, const std::map<FullDiveModality,float>& a) const {
    PhaseLockReport r;
    auto add=[&](FullDiveModality m,float exp){
        auto it=a.find(m); ModalityPhaseState s; s.frameTimestamp=f.monotonicTimestampMs; s.expectedArrivalMs=exp;
        s.actualArrivalMs=it==a.end()?exp:it->second; s.jitterMs=std::fabs(s.actualArrivalMs-exp);
        s.phaseError=exp > 0.0f ? s.jitterMs/exp : 1.0f; s.latencySkew=s.jitterMs;
        s.confidence=std::clamp(1.0f-s.phaseError,0.0f,1.0f); r.worstSkewMs=std::max(r.worstSkewMs,s.latencySkew); r.modalities[m]=s;
    };
    add(FullDiveModality::Visual,16); add(FullDiveModality::Auditory,12); add(FullDiveModality::Somatosensory,8);
    add(FullDiveModality::Vestibular,6); add(FullDiveModality::Proprioceptive,8); add(FullDiveModality::Olfactory,40);
    add(FullDiveModality::Gustatory,40); add(FullDiveModality::RealityAnchor,4);
    r.locked=r.worstSkewMs<=8.0f; return r;
}
PhaseLockReport ModalityPhaseLockEngine::evaluate(const FullDiveNeuralFrame& f, const std::array<float,8>& a) const {
    std::map<FullDiveModality,float> m;
    m[FullDiveModality::Visual]=a[0]; m[FullDiveModality::Auditory]=a[1]; m[FullDiveModality::Somatosensory]=a[2];
    m[FullDiveModality::Proprioceptive]=a[3]; m[FullDiveModality::Vestibular]=a[4]; m[FullDiveModality::Olfactory]=a[5];
    m[FullDiveModality::Gustatory]=a[6]; m[FullDiveModality::RealityAnchor]=a[7]; return evaluate(f,m);
}
}
