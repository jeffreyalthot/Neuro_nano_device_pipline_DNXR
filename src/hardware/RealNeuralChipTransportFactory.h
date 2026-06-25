#pragma once
#include "RealNeuralChipTransport.h"
#include "real/RealTransportUnlockContext.h"
#include <memory>
namespace dnxr { class RealNeuralChipTransportFactory { public: static std::unique_ptr<INeuralChipTransport> create(const RealTransportUnlockContext& context); }; }
