#include "SafeImpactModel.h"
#include <algorithm>
namespace dnxr { SafeImpactResult SafeImpactModel::mapImpact(float x) const { return {true,true,std::clamp(x,0.0f,0.30f),75}; } }
