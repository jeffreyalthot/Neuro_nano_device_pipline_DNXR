#pragma once

#include "full_dive/motor_chip/AvatarKinematicRigController.h"

namespace dnxr {

class AvatarSafetyPoseController {
public:
    AvatarRigCommandFrame apply(const AvatarRigCommandFrame& frame, bool keepHandsVisible) const;
};

} // namespace dnxr
