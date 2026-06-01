#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class PluginType {
    SignalProvider,
    Renderer,
    SafetyLayer,
    NetworkTransport,
    SimulationScenario,
    Utility,
    Unknown
};

struct PluginDescriptor {
    std::string id;
    std::string name;
    std::string version;
    PluginType type = PluginType::Unknown;
    std::vector<std::string> capabilities;
    std::vector<std::string> dependencies;
    bool enabledByDefault = true;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual PluginDescriptor descriptor() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
};

std::string toString(PluginType type);
PluginType parsePluginType(const std::string& value);

} // namespace dnxr
