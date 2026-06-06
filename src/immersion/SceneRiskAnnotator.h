#pragma once
namespace dnxr { struct SceneRiskAnnotation { float motionRisk=0,vestibularRisk=0,emotionalRisk=0,seizureRisk=0,dissociationRisk=0,addictionRisk=0,sensoryLoad=0; bool allowedInClinicalMode=true; }; class SceneRiskAnnotator { public: bool allowed(const SceneRiskAnnotation& a) const; }; }
