#include "ClinicalNeutralSceneController.h"
namespace dnxr { ClinicalSceneGateResult ClinicalNeutralSceneController::evaluate(const ClinicalSceneContentFlags& f) const { if(f.combat||f.fall||f.drowning||f.fire||f.suffocation||f.claustrophobia||f.pursuit||f.horror||f.addictiveReward||f.intenseNarrative) return {false,"clinical_scene_content_blocked"}; return {}; } }
