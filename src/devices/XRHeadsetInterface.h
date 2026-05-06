#pragma once
#include <vector>
namespace dnxr { class XRHeadsetInterface { public: void pushSensoryFrame(const std::vector<float>& frame) const; std::vector<float> pullBioTelemetry() const; }; }
