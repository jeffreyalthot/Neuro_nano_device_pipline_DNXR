#pragma once
#include "RuntimePipelineFrame.h"
#include "../full_dive/FiveSenseCoherenceEngine.h"
namespace dnxr { class RuntimeFiveSenseCoherenceStage { public: static const char* name(); static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const FiveSenseCoherenceEngine& engine = FiveSenseCoherenceEngine{}); }; }
