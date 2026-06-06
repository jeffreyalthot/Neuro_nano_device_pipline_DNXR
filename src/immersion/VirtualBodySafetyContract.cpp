#include "VirtualBodySafetyContract.h"
#include <cmath>
namespace dnxr { bool VirtualBodySafetyContract::clinicalSafe() const { return limbOwnershipRequired&&prohibitAbruptChange&&prohibitExtraLimbs&&prohibitLimbLoss&&prohibitBodyDistortion&&prohibitNonHumanScale&&std::fabs(scale-1.0f)<0.15f&&movementLimit<=1.0f; } }
