#include "NeuralArtifactClassifier.h"
#include <algorithm>
#include <cmath>

namespace dnxr {
NeuralArtifactReport NeuralArtifactClassifier::classify(const std::vector<float>& s) const {
    NeuralArtifactReport r;
    if(s.empty()){
        r.signalDropout=r.packetDropout=1.0f; r.artifactProbability=1.0f; r.dominant=r.artifactType="packet_dropout";
        r.recommendedAction="hold_last_safe_frame"; r.confidence=1.0f; return r;
    }
    float mx=0.0f, mean=0.0f, diff=0.0f;
    for(std::size_t i=0;i<s.size();++i){
        const float v=s[i]; mx=std::max(mx,std::fabs(v)); mean+=v;
        if(std::fabs(v)>0.98f) r.affectedChannels.push_back(i);
        if(i>0) diff+=std::fabs(v-s[i-1]);
    }
    mean/=static_cast<float>(s.size());
    diff=s.size()>1?diff/static_cast<float>(s.size()-1):0.0f;
    r.saturation=mx>0.98f?1.0f:0.0f;
    r.signalClipping=r.saturation;
    r.flatline=mx<0.001f?1.0f:0.0f;
    r.electrodeNoise=(mx<0.001f||diff>1.4f)?0.8f:0.0f;
    r.lineNoise=(diff>0.9f&&diff<=1.4f)?0.6f:0.0f;
    r.highFrequencyBurst=diff>1.4f?0.9f:0.0f;
    r.lowFrequencyDrift=std::fabs(mean)>0.45f?0.7f:0.0f;
    r.motion=diff>0.75f?0.55f:0.0f;
    r.muscle=diff>1.0f?0.65f:0.0f;
    r.eyeBlink=std::fabs(mean)>0.35f&&diff<0.3f?0.5f:0.0f;
    r.jawClench=mx>0.75f&&diff>0.5f?0.55f:0.0f;
    r.electrodePop=mx>0.95f&&diff>1.0f?0.8f:0.0f;
    r.phaseDiscontinuity=diff>1.2f?0.75f:0.0f;
    r.crossChannelCorrelationFailure=s.size()<4?0.5f:0.0f;
    r.artifactProbability=std::max({r.saturation,r.electrodeNoise,r.flatline,r.highFrequencyBurst,r.lowFrequencyDrift,r.motion,r.muscle,r.electrodePop,r.phaseDiscontinuity,r.crossChannelCorrelationFailure});
    if(r.artifactProbability>0.7f) r.recommendedAction="block_or_recalibrate";
    else if(r.artifactProbability>0.4f) r.recommendedAction="force_reality_anchor";
    if(r.artifactProbability==0.0f){ r.dominant=r.artifactType="none"; }
    else if(r.flatline>0.5f){ r.dominant=r.artifactType="flatline"; }
    else if(r.saturation>0.5f){ r.dominant=r.artifactType="signal_clipping"; }
    else if(r.electrodePop>0.5f){ r.dominant=r.artifactType="electrode_pop"; }
    else if(r.highFrequencyBurst>0.5f){ r.dominant=r.artifactType="high_frequency_burst"; }
    else if(r.lowFrequencyDrift>0.5f){ r.dominant=r.artifactType="low_frequency_drift"; }
    else { r.dominant=r.artifactType="electrode_noise"; }
    r.confidence=std::clamp(0.5f+r.artifactProbability*0.5f,0.0f,1.0f);
    if(r.artifactProbability>0.4f) r.affectedModalities.push_back(FullDiveModality::RealityAnchor);
    return r;
}
}
