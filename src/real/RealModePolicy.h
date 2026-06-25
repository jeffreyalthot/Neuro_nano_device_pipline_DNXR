#pragma once
#include "../core/RuntimeDeploymentMode.h"
namespace dnxr {
struct RealModePermissions { bool simulationAllowed=false; bool replayAllowed=false; bool realTransportAllowed=false; bool activeStimulationAllowed=false; bool humanSubjectAllowed=false; bool clinicalAuthorizationRequired=false; };
class RealModePolicy { public: RealModePermissions permissionsFor(RuntimeDeploymentMode mode) const; };
}
