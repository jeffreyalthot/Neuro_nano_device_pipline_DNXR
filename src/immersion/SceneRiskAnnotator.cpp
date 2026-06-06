#include "SceneRiskAnnotator.h"
#include <algorithm>
namespace dnxr { bool SceneRiskAnnotator::allowed(const SceneRiskAnnotation& a) const { return a.allowedInClinicalMode && std::max({a.motionRisk,a.vestibularRisk,a.emotionalRisk,a.seizureRisk,a.dissociationRisk,a.addictionRisk,a.sensoryLoad}) <= 0.35f; } }
