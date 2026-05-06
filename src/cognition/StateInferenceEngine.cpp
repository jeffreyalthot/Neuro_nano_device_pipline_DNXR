#include "StateInferenceEngine.h"
namespace dnxr { std::string StateInferenceEngine::inferState(const std::vector<float>& features) const { return "state:"+intent_.classifyIntent(features); } }
