#include "../../src/assets/ClinicalAssetGate.h"
#include <cassert>
int main(){ dnxr::ClinicalSceneAsset a; a.sceneId="mmorpg"; a.tags={"MMORPG combat"}; a.signedContract=true; a.realityAnchorAvailable=true; a.neutralExitAvailable=true; assert(!dnxr::ClinicalAssetGate().validate(a).allowed); return 0; }
