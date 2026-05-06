#pragma once
#include <string>
#include <vector>
namespace dnxr { class IntentModel { public: std::string classifyIntent(const std::vector<float>& features) const; }; }
