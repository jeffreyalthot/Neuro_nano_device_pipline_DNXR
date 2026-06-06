#pragma once
#include <string>
namespace dnxr { struct DissociationSignals { bool identityConfusion=false,realityConfusion=false,timeConfusion=false,panicSpike=false,refusalToExit=false,compulsiveImmersion=false,memoryDiscontinuity=false; }; struct DerealizationDissociationGuardResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class DerealizationDissociationGuard { public: DerealizationDissociationGuardResult evaluate(const DissociationSignals& i) const; }; }
