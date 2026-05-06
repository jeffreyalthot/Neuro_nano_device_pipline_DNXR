#include "IntentModel.h"
namespace dnxr { std::string IntentModel::classifyIntent(const std::vector<float>& features) const { return features.size()>3?"immersive_navigation":"idle"; } }
