#pragma once
#include "EmergencyLatch.h"
namespace dnxr { class IndependentCutoff { public: bool cutoff(EmergencyLatch& latch, const std::string& reason) const { latch.latch(reason); return true; } }; }
