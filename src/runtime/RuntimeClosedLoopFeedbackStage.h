#pragma once
#include "RuntimePipelineFrame.h"
#include "../hardware/ClosedLoopNeuralFeedbackReader.h"
namespace dnxr { class RuntimeClosedLoopFeedbackStage { public: static const char* name(); static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const ClosedLoopNeuralFeedbackReader& reader = ClosedLoopNeuralFeedbackReader{}); }; }
