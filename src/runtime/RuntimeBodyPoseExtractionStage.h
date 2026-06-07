#pragma once

#include "RuntimePipelineFrame.h"
#include "../immersion/VirtualBodyPoseProvider.h"
#include "../neurotech/PerceivedBodyPoseEstimator.h"

namespace dnxr {

class RuntimeStageServices;

class RuntimeBodyPoseExtractionStage {
public:
    static const char* name();
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame,
                                        const PerceivedBodyPoseEstimator& perceivedEstimator = PerceivedBodyPoseEstimator{},
                                        const VirtualBodyPoseProvider& avatarProvider = VirtualBodyPoseProvider{});
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
