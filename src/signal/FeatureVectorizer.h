#pragma once
#include <vector>
namespace dnxr { class FeatureVectorizer { public: std::vector<float> vectorize(const std::vector<float>& cleanSignal) const; }; }
