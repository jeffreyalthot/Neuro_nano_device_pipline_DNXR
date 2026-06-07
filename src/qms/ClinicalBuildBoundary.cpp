#include "ClinicalBuildBoundary.h"
#include <algorithm>
namespace dnxr { namespace { bool contains(std::string v,const std::string& n){ std::transform(v.begin(),v.end(),v.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));}); return v.find(n)!=std::string::npos; } }
ClinicalBoundaryDecision ClinicalBuildBoundary::validateModule(const std::string& m,bool h) const { if(!h) return {}; const char* denied[]={"researchfulldiveorchestrator","fulldiveorchestrator","quantum","memory","time","emotion","subconscious","distributedconsciousness"}; for(auto d:denied) if(contains(m,d)) return {false,"clinical_boundary_rejects_"+std::string(d)}; return {}; }
ClinicalBoundaryDecision ClinicalBuildBoundary::validateAssetTag(const std::string& a,bool h) const { if(!h) return {}; const char* denied[]={"combat","mmorpg","memory","time","emotion","subconscious"}; for(auto d:denied) if(contains(a,d)) return {false,"clinical_asset_or_manipulation_rejected"}; return {}; }}
