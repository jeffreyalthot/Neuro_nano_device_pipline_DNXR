// FeatureVectorizer.h

#pragma once

#include <vector>

namespace dnxr {

class FeatureVectorizer {
public:
    FeatureVectorizer();

    /// Conversion signal propre -> vecteur de features stables.
    std::vector<float> vectorize(const std::vector<float>& cleanSignal) const;

    void setEpsilon(float eps);
    float getEpsilon() const;

    void enableNormalization(bool enabled);
    bool isNormalizationEnabled() const;

    float getLastFeatureEnergy() const;

private:
    float computeEnergy(const std::vector<float>& f) const;
    void normalize(std::vector<float>& f) const;

private:
    float epsilon_;
    bool normalizationEnabled_;
    mutable float lastFeatureEnergy_;
};

} // namespace dnxr