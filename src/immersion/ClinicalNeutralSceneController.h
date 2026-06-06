#pragma once
#include <string>
namespace dnxr { struct ClinicalSceneContentFlags { bool combat=false,fall=false,drowning=false,fire=false,suffocation=false,claustrophobia=false,pursuit=false,horror=false,addictiveReward=false,intenseNarrative=false; }; struct ClinicalSceneGateResult { bool allowed=true; std::string reason="clinical_neutral_scene_allowed"; }; class ClinicalNeutralSceneController { public: ClinicalSceneGateResult evaluate(const ClinicalSceneContentFlags& f) const; }; }
