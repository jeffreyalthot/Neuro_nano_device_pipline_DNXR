#pragma once
#include <string>
namespace dnxr { struct OlfactoryRequest { float intensity=0; bool contextValid=false; }; struct OlfactoryPersistenceState { float emittedIntensity=0, adaptation=0, crossContamination=0, exposureMs=0; bool allowed=true; std::string reason="ok"; }; class OlfactoryPersistenceEngine { public: OlfactoryPersistenceState update(const OlfactoryRequest& request, double dtMs); private: OlfactoryPersistenceState state_; }; }
