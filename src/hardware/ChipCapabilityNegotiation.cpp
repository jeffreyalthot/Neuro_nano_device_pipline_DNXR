#include "ChipCapabilityNegotiation.h"
#include <algorithm>
namespace dnxr { ChipCapabilities ChipCapabilityNegotiation::clampForClinical(const ChipCapabilities& a) const { ChipCapabilities c=a; c.maxSafeRateHz=std::min(c.maxSafeRateHz,90.0f); c.maxPacketSize=std::min(c.maxPacketSize,4096); return c; } bool ChipCapabilityNegotiation::supportsFiveSenseClosedLoop(const ChipCapabilities& c) const { return c.canVisual&&c.canAuditory&&c.canHaptic&&c.canVestibular&&c.canProprioceptive&&c.canOlfactory&&c.canGustatory&&c.supportsHardwareCutoff&&c.supportsClosedLoopFeedback; } }
