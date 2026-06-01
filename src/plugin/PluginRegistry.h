#pragma once

#include "IPlugin.h"

#include <memory>
#include <string>
#include <vector>

namespace dnxr {

struct PluginRegistrationResult {
    bool accepted = false;
    std::string message;
};

struct PluginDependencyReport {
    bool satisfied = true;
    std::vector<std::string> missingPluginIds;
};

struct PluginRuntimeState {
    PluginDescriptor descriptor;
    bool enabled = false;
    bool initialized = false;
};

class PluginRegistry {
public:
    PluginRegistrationResult registerPlugin(std::unique_ptr<IPlugin> plugin);

    bool enable(const std::string& id);
    bool disable(const std::string& id);
    bool initializePlugin(const std::string& id);
    void initializeEnabled();
    void shutdownAll();

    IPlugin* find(const std::string& id);
    const IPlugin* find(const std::string& id) const;
    std::vector<PluginRuntimeState> plugins() const;
    std::vector<PluginRuntimeState> pluginsByType(PluginType type) const;
    std::vector<PluginRuntimeState> pluginsWithCapability(const std::string& capability) const;
    PluginDependencyReport dependencyReport(const std::string& id) const;
    bool isInitialized(const std::string& id) const;
    bool isEnabled(const std::string& id) const;
    std::size_t size() const;
    void clear();

private:
    struct Entry {
        std::unique_ptr<IPlugin> plugin;
        PluginDescriptor descriptor;
        bool enabled = false;
        bool initialized = false;
    };

    Entry* findEntry(const std::string& id);
    const Entry* findEntry(const std::string& id) const;
    bool dependenciesSatisfied(const Entry& entry) const;

    std::vector<Entry> entries_;
};

} // namespace dnxr
