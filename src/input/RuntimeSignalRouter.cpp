#include "RuntimeSignalRouter.h"
#include "FileReplaySignalProvider.h"
#include "SimulatedSignalProvider.h"
#include "../utils/Logger.h"
#include "../runtime/RuntimeTypes.h"

namespace dnxr {

RuntimeSignalRouter::RuntimeSignalRouter(const RuntimeConfig& config) {
    const auto source = parseSignalSource(config.signalSource);
    if (source == SignalSource::FileReplay) {
        provider_ = std::make_unique<FileReplaySignalProvider>(config.replaySignalPath);
    } else {
        if (source != SignalSource::Simulated) {
            Logger::warn("[RuntimeSignalRouter] Unsupported signal source '" + config.signalSource + "'; using simulated provider.");
        }
        NeuralSignalSimulatorConfig simulatorConfig;
        simulatorConfig.scenario = parseSimulationScenario(config.simulationScenario);
        simulatorConfig.channelCount = config.simulationChannelCount;
        simulatorConfig.noiseAmplitude = config.simulationNoiseAmplitude;
        simulatorConfig.dropoutEveryCycles = config.simulationDropoutEveryCycles;
        provider_ = std::make_unique<SimulatedSignalProvider>(simulatorConfig);
    }
}

std::vector<float> RuntimeSignalRouter::nextSignal(int cycle) {
    return provider_->nextSignal(cycle);
}

std::string RuntimeSignalRouter::activeSource() const {
    return provider_->name();
}

} // namespace dnxr
