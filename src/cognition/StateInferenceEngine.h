#pragma once
#include "IntentModel.h"
#include <string>
#include <vector>
namespace dnxr { class StateInferenceEngine { public: std::string inferState(const std::vector<float>& features) const; private: IntentModel intent_; }; }
