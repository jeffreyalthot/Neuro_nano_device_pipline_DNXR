#include "XRHeadsetInterface.h"
#include "../utils/Logger.h"
namespace dnxr { void XRHeadsetInterface::pushSensoryFrame(const std::vector<float>& frame) const { Logger::info("XR frame pushed="+std::to_string(frame.size())); } std::vector<float> XRHeadsetInterface::pullBioTelemetry() const { return {72.0F,16.0F,98.0F}; } }
