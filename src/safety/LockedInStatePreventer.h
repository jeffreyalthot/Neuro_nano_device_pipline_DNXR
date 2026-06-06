#pragma once
#include <string>
namespace dnxr { struct ExitChannelState { bool patientStop=true,operatorStop=true,physicianStop=true,hardwareCutoff=true,realityAnchor=true,communication=true; }; struct LockedInStatePreventerResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class LockedInStatePreventer { public: LockedInStatePreventerResult evaluate(const ExitChannelState& i) const; }; }
