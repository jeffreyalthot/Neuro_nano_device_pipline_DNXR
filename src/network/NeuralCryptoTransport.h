#pragma once

#include "SimulationCryptoTransport.h"

namespace dnxr {

using NeuralCryptoTransport [[deprecated("Use SimulationCryptoTransport in simulation builds")]] =
    SimulationCryptoTransport;

} // namespace dnxr
