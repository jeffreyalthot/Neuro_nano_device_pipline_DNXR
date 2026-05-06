#pragma once
#include <vector>
namespace dnxr { class AdaptationLayer { public: std::vector<float> applyUserProfile(const std::vector<float>& vec,float gain) const; }; }
