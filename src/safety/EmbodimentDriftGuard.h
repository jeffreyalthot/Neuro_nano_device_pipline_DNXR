#pragma once
#include <string>
namespace dnxr { struct EmbodimentSignals { bool avatarMismatch=false,limbInstability=false,wrongScale=false,asymmetricFeedback=false,delayedTouch=false; }; struct EmbodimentDriftGuardResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class EmbodimentDriftGuard { public: EmbodimentDriftGuardResult evaluate(const EmbodimentSignals& i) const; }; }
