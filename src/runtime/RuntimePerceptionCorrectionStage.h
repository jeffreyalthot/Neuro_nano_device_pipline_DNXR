#pragma once
#include "RuntimePipelineFrame.h"
#include "../full_dive/PerceptionErrorCorrector.h"
namespace dnxr { class RuntimePerceptionCorrectionStage { public: static const char* name(); static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const PerceptionErrorCorrector& corrector = PerceptionErrorCorrector{}); }; }
