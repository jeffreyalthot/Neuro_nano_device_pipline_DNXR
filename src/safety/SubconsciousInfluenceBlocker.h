#pragma once
#include <string>
namespace dnxr { struct InfluenceSignals { bool memoryManipulation=false,emotionForcing=false,fearInduction=false,rewardAddictionLoops=false,identityOverwrite=false,suggestionLoops=false,unapprovedNarrativePressure=false; }; struct SubconsciousInfluenceBlockerResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class SubconsciousInfluenceBlocker { public: SubconsciousInfluenceBlockerResult evaluate(const InfluenceSignals& i) const; }; }
