#include "../src/plugin/PluginRegistry.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace {

class TestPlugin final : public dnxr::IPlugin {
public:
    explicit TestPlugin(dnxr::PluginDescriptor descriptor, bool initializeResult = true)
        : descriptor_(std::move(descriptor)), initializeResult_(initializeResult) {}

    dnxr::PluginDescriptor descriptor() const override {
        return descriptor_;
    }

    bool initialize() override {
        ++initializeCalls;
        initialized = initializeResult_;
        return initializeResult_;
    }

    void shutdown() override {
        ++shutdownCalls;
        initialized = false;
    }

    int initializeCalls = 0;
    int shutdownCalls = 0;
    bool initialized = false;

private:
    dnxr::PluginDescriptor descriptor_;
    bool initializeResult_ = true;
};

dnxr::PluginDescriptor descriptor(const std::string& id,
                                  dnxr::PluginType type,
                                  std::vector<std::string> capabilities = {},
                                  std::vector<std::string> dependencies = {},
                                  bool enabledByDefault = true) {
    dnxr::PluginDescriptor value;
    value.id = id;
    value.name = id + " plugin";
    value.version = "1.0.0";
    value.type = type;
    value.capabilities = std::move(capabilities);
    value.dependencies = std::move(dependencies);
    value.enabledByDefault = enabledByDefault;
    return value;
}

} // namespace

int main() {
    assert(dnxr::toString(dnxr::PluginType::SignalProvider) == "signal_provider");
    assert(dnxr::parsePluginType("renderer") == dnxr::PluginType::Renderer);
    assert(dnxr::parsePluginType("not_real") == dnxr::PluginType::Unknown);

    dnxr::PluginRegistry registry;
    auto nullResult = registry.registerPlugin(nullptr);
    assert(!nullResult.accepted);

    auto emptyId = registry.registerPlugin(std::make_unique<TestPlugin>(descriptor("", dnxr::PluginType::Utility)));
    assert(!emptyId.accepted);

    auto unknownType = registry.registerPlugin(std::make_unique<TestPlugin>(descriptor("unknown", dnxr::PluginType::Unknown)));
    assert(!unknownType.accepted);

    auto signal = std::make_unique<TestPlugin>(
        descriptor("sim_signal", dnxr::PluginType::SignalProvider, {"signal.provider", "simulation.safe"}));
    TestPlugin* signalPtr = signal.get();
    assert(registry.registerPlugin(std::move(signal)).accepted);
    assert(!registry.registerPlugin(std::make_unique<TestPlugin>(descriptor("sim_signal", dnxr::PluginType::Renderer))).accepted);

    assert(registry.size() == 1U);
    assert(registry.isEnabled("sim_signal"));
    assert(registry.pluginsByType(dnxr::PluginType::SignalProvider).size() == 1U);
    assert(registry.pluginsWithCapability("simulation.safe").size() == 1U);
    assert(registry.pluginsWithCapability("missing.capability").empty());

    auto dependent = std::make_unique<TestPlugin>(
        descriptor("renderer", dnxr::PluginType::Renderer, {"renderer.xr"}, {"sim_signal"}));
    TestPlugin* dependentPtr = dependent.get();
    assert(registry.registerPlugin(std::move(dependent)).accepted);

    auto blocked = std::make_unique<TestPlugin>(
        descriptor("blocked_safety", dnxr::PluginType::SafetyLayer, {"safety.layer"}, {"missing_dependency"}));
    assert(registry.registerPlugin(std::move(blocked)).accepted);

    auto disabled = std::make_unique<TestPlugin>(
        descriptor("manual_network", dnxr::PluginType::NetworkTransport, {"network.transport"}, {}, false));
    assert(registry.registerPlugin(std::move(disabled)).accepted);
    assert(!registry.isEnabled("manual_network"));

    assert(!registry.initializePlugin("renderer"));
    assert(registry.initializePlugin("sim_signal"));
    assert(signalPtr->initializeCalls == 1);
    assert(registry.initializePlugin("renderer"));
    assert(dependentPtr->initializeCalls == 1);

    const auto blockedReport = registry.dependencyReport("blocked_safety");
    assert(!blockedReport.satisfied);
    assert(blockedReport.missingPluginIds.size() == 1U);
    assert(blockedReport.missingPluginIds.front() == "missing_dependency");

    registry.initializeEnabled();
    assert(registry.isInitialized("sim_signal"));
    assert(registry.isInitialized("renderer"));
    assert(!registry.isInitialized("blocked_safety"));
    assert(!registry.isInitialized("manual_network"));

    assert(registry.enable("manual_network"));
    registry.initializeEnabled();
    assert(registry.isInitialized("manual_network"));

    assert(registry.disable("renderer"));
    assert(!registry.isEnabled("renderer"));
    assert(!registry.isInitialized("renderer"));
    assert(dependentPtr->shutdownCalls == 1);

    registry.shutdownAll();
    assert(!registry.isInitialized("sim_signal"));
    assert(signalPtr->shutdownCalls == 1);

    registry.clear();
    assert(registry.size() == 0U);
    return 0;
}
