#pragma once
#include <string>
#include <vector>
namespace dnxr { struct MultiWindowIntentResult { std::string intentClass="none"; float intentStrength=0,intentDirection=0,voluntaryControlConfidence=0,artifactProbability=0,ambiguityScore=1; bool safeToAct=false; }; class MultiWindowIntentDecoder { public: MultiWindowIntentResult decode(const std::vector<float>& instantaneous,const std::vector<float>& baseline,float fatigue,float artifactProbability,bool patientStopConflict) const; }; }
