#include "AdaptationLayer.h"
namespace dnxr { std::vector<float> AdaptationLayer::applyUserProfile(const std::vector<float>& vec,float gain) const { auto out=vec; for(auto &v:out){ v*=gain; } return out; } }
