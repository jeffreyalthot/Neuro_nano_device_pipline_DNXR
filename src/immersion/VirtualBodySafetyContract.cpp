#include "VirtualBodySafetyContract.h"
#include <cmath>
namespace dnxr { bool VirtualBodySafetyContract::clinicalSafe() const { return !prohibitedTransformationDetected&&limbOwnershipRequired&&prohibitAbruptChange&&prohibitExtraLimbs&&prohibitLimbLoss&&prohibitBodyDistortion&&prohibitNonHumanScale&&std::fabs(scale-1.0f)<0.15f&&std::fabs(scaleDelta)<0.15f&&std::fabs(limbLengthDelta)<0.15f&&std::fabs(jointLimitDelta)<0.20f&&ownershipRisk<0.2f&&proprioceptiveMismatchRisk<0.2f&&hapticMismatchRisk<0.2f&&vestibularMismatchRisk<0.2f&&movementLimit<=1.0f; } }
