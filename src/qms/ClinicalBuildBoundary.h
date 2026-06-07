#pragma once
#include <string>
namespace dnxr { struct ClinicalBoundaryDecision { bool allowed=true; std::string reason="allowed"; }; class ClinicalBuildBoundary { public: ClinicalBoundaryDecision validateModule(const std::string& moduleName, bool humanUseBuild) const; ClinicalBoundaryDecision validateAssetTag(const std::string& assetTag, bool humanUseBuild) const; }; }
