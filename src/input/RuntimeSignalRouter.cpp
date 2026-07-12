#include "RuntimeSignalRouter.h"
#include "FileReplaySignalProvider.h"
#if DNXR_SIMULATION_ONLY
#include "SimulatedSignalProvider.h"
#endif
#include "../utils/Logger.h"
#include <stdexcept>
#include "../runtime/RuntimeTypes.h"

namespace dnxr {

RuntimeSignalRouter::RuntimeSignalRouter(const RuntimeConfig& config) {
    const auto source = parseSignalSource(config.signalSource);
    if (source == SignalSource::FileReplay) {
        provider_ = std::make_unique<FileReplaySignalProvider>(config.replaySignalPath);
    } else if (source == SignalSource::Simulated) {
#if DNXR_SIMULATION_ONLY
        NeuralSignalSimulatorConfig simulatorConfig;
        simulatorConfig.scenario = parseSimulationScenario(config.simulationScenario);
        simulatorConfig.channelCount = config.simulationChannelCount;
        simulatorConfig.noiseAmplitude = config.simulationNoiseAmplitude;
        simulatorConfig.dropoutEveryCycles = config.simulationDropoutEveryCycles;
        provider_ = std::make_unique<SimulatedSignalProvider>(simulatorConfig);
#else
        Logger::error("[RuntimeSignalRouter] SOURCE_CONFIGURATION_INVALID: simulated source not compiled in this build");
        throw std::invalid_argument("SOURCE_CONFIGURATION_INVALID");
#endif
    } else {
        Logger::error("[RuntimeSignalRouter] SOURCE_CONFIGURATION_INVALID: unsupported signal source '" + config.signalSource + "'");
        throw std::invalid_argument("SOURCE_CONFIGURATION_INVALID");
    }
}

std::vector<float> RuntimeSignalRouter::nextSignal(int cycle) {
    return provider_->nextSignal(cycle);
}

std::string RuntimeSignalRouter::activeSource() const {
    return provider_->name();
}

} // namespace dnxr
