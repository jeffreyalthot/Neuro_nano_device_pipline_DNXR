#include "NeuralArtifactClassifier.h"
#include <algorithm>
#include <cmath>
namespace dnxr { NeuralArtifactReport NeuralArtifactClassifier::classify(const std::vector<float>& s) const { NeuralArtifactReport r; if(s.empty()){r.signalDropout=1;r.artifactProbability=1;r.dominant="signal_dropout";return r;} float mx=0; for(float v:s) mx=std::max(mx,std::fabs(v)); r.saturation=mx>0.98f?1.0f:0.0f; r.electrodeNoise=mx<0.001f?0.8f:0.0f; r.artifactProbability=std::max(r.saturation,r.electrodeNoise); r.dominant=r.artifactProbability>0.5f?(r.saturation>0?"saturation":"electrode_noise"):"none"; return r; } }
