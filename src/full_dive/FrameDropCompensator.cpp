#include "FrameDropCompensator.h"
#include <algorithm>
namespace dnxr { FullDiveNeuralFrame FrameDropCompensator::compensate(const FullDiveNeuralFrame& last, bool drift) const { FullDiveNeuralFrame out=last; auto reduce=[](auto& layer,float scale){ for(auto& c:layer.channels)c.intensity*=scale; }; reduce(out.visual,0.75f); reduce(out.auditory,0.75f); reduce(out.somatosensory,0.5f); reduce(out.vestibular,0.0f); if(drift) reduce(out.proprioception,0.0f); out.realityAnchor.anchorRequired=true; out.realityAnchor.anchorIntensity=1.0f; return out; } }
