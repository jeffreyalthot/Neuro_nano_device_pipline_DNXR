#pragma once

#include "../hardware/NeuralChipHAL.h"
#include "../neurotech/DeepImmersionPlatform.h"

namespace dnxr {

// Neural/cognitive payloads carried across runtime pipeline stages.
using RuntimePipelineNeuralChipFrame = NeuralChipFrame;
using RuntimePipelineBiometricFrame = BiometricFrame;
using RuntimePipelineCognitiveState = CognitiveState;
using RuntimePipelineAdaptationFrame = XRAdaptationFrame;
using RuntimePipelineDiagnosticsFrame = NeuroPipelineDiagnostics;
using RuntimePipelineBodyCommand = XRFullBodyCommand;

} // namespace dnxr
