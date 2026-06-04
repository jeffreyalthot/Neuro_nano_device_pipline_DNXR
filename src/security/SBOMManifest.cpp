#include "SBOMManifest.h"
namespace dnxr { void SBOMManifest::add(SBOMComponent c){components_.push_back(c);} std::vector<SBOMComponent> SBOMManifest::components() const{return components_;} bool SBOMManifest::empty() const{return components_.empty();} }
