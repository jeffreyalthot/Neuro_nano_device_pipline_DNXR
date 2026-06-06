#pragma once
#include "RuntimePipelineFrame.h"
#include "../full_dive/BodySchemaEngine.h"
namespace dnxr { class RuntimeBodySchemaStage { public: static const char* name(); static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const BodyPoseState& perceived, const BodyPoseState& avatar, const BodySchemaEngine& engine = BodySchemaEngine{}); }; }
