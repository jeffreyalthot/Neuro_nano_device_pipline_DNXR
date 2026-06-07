#pragma once

#include "../full_dive/BodySchemaEngine.h"
#include "../runtime/RuntimePipelineFrame.h"

namespace dnxr {

class VirtualBodyPoseProvider {
public:
    BodyPoseState currentAvatarPose(const RuntimePipelineFrame& frame) const;
    static bool isComplete(const BodyPoseState& pose);
};

} // namespace dnxr
