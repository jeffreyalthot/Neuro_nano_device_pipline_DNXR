#pragma once
#include <string>
namespace dnxr { struct BenchOnlyHardwareRequest { bool humanSubjectAttached=false; bool allowStimulusDispatch=false; bool missingPhysicalCutoff=false; bool missingAckLine=false; }; struct BenchOnlyHardwareDecision { bool accepted=false; bool benchOnly=true; bool phantomOnly=true; bool allowStimulusDispatch=false; std::string reason; }; class BenchOnlyNeuralHardwareAdapter { public: BenchOnlyHardwareDecision validate(const BenchOnlyHardwareRequest& request) const; }; }
