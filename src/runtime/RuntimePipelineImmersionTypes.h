#pragma once

#include "../immersion/MultisensoryClock.h"
#include "../immersion/SensoryRenderer.h"

namespace dnxr {

// Immersion-frame payloads shared by the frame builder, safety layer, and renderer.
using RuntimePipelineAlignmentReport = MultisensoryAlignmentReport;
using RuntimePipelineNeuralSensoryFrame = NeuralSensoryFrame;

} // namespace dnxr
