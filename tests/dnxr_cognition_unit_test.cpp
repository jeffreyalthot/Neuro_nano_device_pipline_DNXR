#include "../src/cognition/StateInferenceEngine.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::StateInferenceEngine eng;
    eng.setConfidenceThreshold(0.0F);
    std::vector<float> features = {0.3F, -0.1F, 0.6F, 0.2F};

    auto state = eng.inferState(features, 1.0);
    assert(state.rfind("state:", 0) == 0);

    auto undefined = eng.inferState({}, 2.0);
    assert(undefined == "state:undefined");
    return 0;
}
