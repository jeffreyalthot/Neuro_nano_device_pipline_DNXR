#include "SensoryRenderer.h"
namespace dnxr { std::vector<float> SensoryRenderer::renderSensoryFrame(const VirtualWorldState& state) const { return {state.intensity, state.intensity*0.8F, state.intensity*0.6F}; } }
