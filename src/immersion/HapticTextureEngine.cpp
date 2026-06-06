#include "HapticTextureEngine.h"
#include <algorithm>
namespace dnxr { HapticTextureLayer HapticTextureEngine::synthesize(const HapticSurfaceInput& i) const { HapticTextureLayer l; l.pressure=std::clamp(i.pressure+i.impact*0.25f,0.0f,0.45f); l.vibration=std::clamp(i.vibration+i.slip*0.2f+i.roughness*0.1f,0.0f,0.35f); l.texture=std::clamp(i.roughness+i.contactDurationMs/10000.0f,0.0f,0.35f); return l; } }
