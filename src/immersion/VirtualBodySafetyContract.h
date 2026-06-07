#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr { struct VirtualBodySafetyContract { float height=1.7f,armLength=0.7f,legLength=0.9f,scale=1.0f; std::string handMapping="human_bilateral"; std::vector<float> jointLimits; float movementLimit=1.0f; std::uint64_t patientBodyProfileHash=0,avatarBodyProfileHash=0; float scaleDelta=0,limbLengthDelta=0,jointLimitDelta=0,ownershipRisk=0,proprioceptiveMismatchRisk=0,hapticMismatchRisk=0,vestibularMismatchRisk=0; bool prohibitedTransformationDetected=false; bool limbOwnershipRequired=true,prohibitAbruptChange=true,prohibitExtraLimbs=true,prohibitLimbLoss=true,prohibitBodyDistortion=true,prohibitNonHumanScale=true; bool clinicalSafe() const; }; }
