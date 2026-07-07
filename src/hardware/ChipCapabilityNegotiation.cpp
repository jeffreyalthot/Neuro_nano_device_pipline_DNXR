#include "ChipCapabilityNegotiation.h"

#include <algorithm>

namespace dnxr {

ChipCapabilities
ChipCapabilityNegotiation::clampForClinical(
    const ChipCapabilities &advertised) const {
  ChipCapabilities capabilities = advertised;
  capabilities.maxSafeRateHz = capabilities.maxSafeRateHz > 0.0f
                                   ? std::min(capabilities.maxSafeRateHz,
                                              DNXR_NEURAL_CHIP_TARGET_FRAME_RATE_HZ)
                                   : DNXR_NEURAL_CHIP_TARGET_FRAME_RATE_HZ;
  capabilities.maxPacketSize = capabilities.maxPacketSize > 0
                                   ? std::min(capabilities.maxPacketSize, 4096)
                                   : 4096;

  const std::size_t wireLimitedChannels = capabilities.wireCount > 0
                                              ? capabilities.wireCount *
                                                    DNXR_NEURAL_CHIP_CHANNELS_PER_WIRE
                                              : capabilities.electrodeCount;
  std::size_t topologyLimit = capabilities.electrodeCount;
  if (wireLimitedChannels > 0) {
    topologyLimit = topologyLimit > 0 ? std::min(topologyLimit, wireLimitedChannels)
                                      : wireLimitedChannels;
  }
  if (capabilities.maxStimulusChannels > 0) {
    topologyLimit = topologyLimit > 0
                        ? std::min(topologyLimit, capabilities.maxStimulusChannels)
                        : capabilities.maxStimulusChannels;
  }
  capabilities.maxStimulusChannels = topologyLimit;
  return capabilities;
}

bool ChipCapabilityNegotiation::supportsFiveSenseClosedLoop(
    const ChipCapabilities &capabilities) const {
  return capabilities.canVisual && capabilities.canAuditory &&
         capabilities.canHaptic && capabilities.canVestibular &&
         capabilities.canProprioceptive && capabilities.canOlfactory &&
         capabilities.canGustatory && capabilities.supportsHardwareCutoff &&
         capabilities.supportsClosedLoopFeedback;
}

} // namespace dnxr
