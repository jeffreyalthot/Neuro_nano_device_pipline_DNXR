#pragma once
#include "../immersion/SceneCapabilityContract.h"
#include <string>
#include <vector>
namespace dnxr { struct ClinicalAssetDecision { bool allowed=false; std::string reason="not_evaluated"; }; struct ClinicalSceneAsset { std::string sceneId; std::vector<std::string> tags; bool signedContract=false; bool realityAnchorAvailable=false; bool neutralExitAvailable=false; SceneCapabilityContract contract; }; class ClinicalAssetGate { public: ClinicalAssetDecision validate(const ClinicalSceneAsset& asset) const; }; }
