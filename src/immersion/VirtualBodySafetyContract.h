#pragma once
#include <string>
#include <vector>
namespace dnxr { struct VirtualBodySafetyContract { float height=1.7f,armLength=0.7f,legLength=0.9f,scale=1.0f; std::string handMapping="human_bilateral"; std::vector<float> jointLimits; float movementLimit=1.0f; bool limbOwnershipRequired=true,prohibitAbruptChange=true,prohibitExtraLimbs=true,prohibitLimbLoss=true,prohibitBodyDistortion=true,prohibitNonHumanScale=true; bool clinicalSafe() const; }; }
