#include "SceneCapabilityContract.h"
namespace dnxr { bool SceneCapabilityContract::acceptableForClinical(float b,bool c,bool r) const { return c&&r&&requiresRealityAnchor&&maxSensoryLoad<=b&&vestibularRisk<0.4f&&seizureRisk<0.2f&&dissociationRisk<0.3f&&painRisk==0.0f&&emotionRisk<0.2f; } }
