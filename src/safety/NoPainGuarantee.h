#pragma once
#include <string>
namespace dnxr { struct PainSafetyInput { bool pain=false,burning=false,suffocation=false,crushing=false,fallPanic=false,forcedParalysis=false,lockedIn=false,fearInduction=false; }; struct NoPainGuaranteeResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class NoPainGuarantee { public: NoPainGuaranteeResult evaluate(const PainSafetyInput& i) const; }; }
