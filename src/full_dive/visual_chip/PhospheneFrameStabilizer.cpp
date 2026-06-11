#include "full_dive/visual_chip/PhospheneFrameStabilizer.h"
#include <algorithm>
namespace dnxr { float PhospheneFrameStabilizer::clamp01(float v){return std::max(0.0f,std::min(1.0f,v));} NeuralVisualVector PhospheneFrameStabilizer::stabilize(const NeuralVisualVector& current) const { NeuralVisualVector out=current; if(out.flickerRisk01>0.5f){ out.phospheneDensity01*=0.5f; out.motionFlow01=std::min(out.motionFlow01,0.25f);} out.phospheneStability01=clamp01(std::max(out.phospheneStability01,1.0f-out.flickerRisk01)); return out;} }
