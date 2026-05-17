#include "../src/signal/SignalStreamProcessor.h"
#include "../src/signal/FeatureVectorizer.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

int main() {
    dnxr::SignalStreamProcessor dsp;
    dnxr::FeatureVectorizer fv;

    std::vector<float> raw = {2.0F, -2.0F, std::numeric_limits<float>::quiet_NaN(), 0.5F};
    auto clean = dsp.denoise(raw);
    assert(clean.size() == raw.size());
    for (float v : clean) {
        assert(std::isfinite(v));
        assert(v <= 1.0F && v >= -1.0F);
    }

    auto features = fv.vectorize(clean);
    assert(features.size() == 4U);
    for (float v : features) assert(std::isfinite(v));
    assert(fv.getLastFeatureEnergy() >= 0.0F);
    return 0;
}
