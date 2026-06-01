#include "PluginRegistry.h"

#include <algorithm>
#include <utility>

namespace dnxr {
namespace {

bool containsCapability(const PluginDescriptor& descriptor, const std::string& capability) {
    return std::find(descriptor.capabilities.begin(), descriptor.capabilities.end(), capability) != descriptor.capabilities.end();
}

} // namespace

std::string toString(PluginType type) {
    switch (type) {
    case PluginType::SignalProvider:
        return "signal_provider";
    case PluginType::Renderer:
        return "renderer";
    case PluginType::SafetyLayer:
        return "safety_layer";
    case PluginType::NetworkTransport:
        return "network_transport";
    case PluginType::SimulationScenario:
        return "simulation_scenario";
    case PluginType::Utility:
        return "utility";
    case PluginType::Unknown:
    default:
        return "unknown";
    }
}

PluginType parsePluginType(const std::string& value) {
    if (value == "signal_provider") {
        return PluginType::SignalProvider;
    }
    if (value == "renderer") {
        return PluginType::Renderer;
    }
    if (value == "safety_layer") {
        return PluginType::SafetyLayer;
    }
    if (value == "network_transport") {
        return PluginType::NetworkTransport;
    }
    if (value == "simulation_scenario") {
        return PluginType::SimulationScenario;
    }
    if (value == "utility") {
        return PluginType::Utility;
    }
    return PluginType::Unknown;
}

PluginRegistrationResult PluginRegistry::registerPlugin(std::unique_ptr<IPlugin> plugin) {
    if (!plugin) {
        return {false, "plugin pointer is null"};
    }

    PluginDescriptor descriptor = plugin->descriptor();
    if (descriptor.id.empty()) {
        return {false, "plugin id is empty"};
    }
    if (descriptor.type == PluginType::Unknown) {
        return {false, "plugin type is unknown"};
    }
    if (findEntry(descriptor.id) != nullptr) {
        return {false, "plugin id already registered"};
    }

    Entry entry;
    entry.plugin = std::move(plugin);
    entry.descriptor = std::move(descriptor);
    entry.enabled = entry.descriptor.enabledByDefault;
    entries_.push_back(std::move(entry));
    return {true, "registered"};
}

bool PluginRegistry::enable(const std::string& id) {
    Entry* entry = findEntry(id);
    if (entry == nullptr) {
        return false;
    }
    entry->enabled = true;
    return true;
}

bool PluginRegistry::disable(const std::string& id) {
    Entry* entry = findEntry(id);
    if (entry == nullptr) {
        return false;
    }
    if (entry->initialized) {
        entry->plugin->shutdown();
        entry->initialized = false;
    }
    entry->enabled = false;
    return true;
}

bool PluginRegistry::initializePlugin(const std::string& id) {
    Entry* entry = findEntry(id);
    if (entry == nullptr || !entry->enabled) {
        return false;
    }
    if (entry->initialized) {
        return true;
    }
    if (!dependenciesSatisfied(*entry)) {
        return false;
    }
    entry->initialized = entry->plugin->initialize();
    return entry->initialized;
}

void PluginRegistry::initializeEnabled() {
    bool progressed = true;
    while (progressed) {
        progressed = false;
        for (Entry& entry : entries_) {
            if (entry.enabled && !entry.initialized && dependenciesSatisfied(entry)) {
                const bool initialized = entry.plugin->initialize();
                entry.initialized = initialized;
                progressed = progressed || initialized;
            }
        }
    }
}

void PluginRegistry::shutdownAll() {
    for (auto it = entries_.rbegin(); it != entries_.rend(); ++it) {
        if (it->initialized) {
            it->plugin->shutdown();
            it->initialized = false;
        }
    }
}

IPlugin* PluginRegistry::find(const std::string& id) {
    Entry* entry = findEntry(id);
    return entry == nullptr ? nullptr : entry->plugin.get();
}

const IPlugin* PluginRegistry::find(const std::string& id) const {
    const Entry* entry = findEntry(id);
    return entry == nullptr ? nullptr : entry->plugin.get();
}

std::vector<PluginRuntimeState> PluginRegistry::plugins() const {
    std::vector<PluginRuntimeState> result;
    result.reserve(entries_.size());
    for (const Entry& entry : entries_) {
        result.push_back({entry.descriptor, entry.enabled, entry.initialized});
    }
    return result;
}

std::vector<PluginRuntimeState> PluginRegistry::pluginsByType(PluginType type) const {
    std::vector<PluginRuntimeState> result;
    for (const Entry& entry : entries_) {
        if (entry.descriptor.type == type) {
            result.push_back({entry.descriptor, entry.enabled, entry.initialized});
        }
    }
    return result;
}

std::vector<PluginRuntimeState> PluginRegistry::pluginsWithCapability(const std::string& capability) const {
    std::vector<PluginRuntimeState> result;
    for (const Entry& entry : entries_) {
        if (containsCapability(entry.descriptor, capability)) {
            result.push_back({entry.descriptor, entry.enabled, entry.initialized});
        }
    }
    return result;
}

PluginDependencyReport PluginRegistry::dependencyReport(const std::string& id) const {
    PluginDependencyReport report;
    const Entry* entry = findEntry(id);
    if (entry == nullptr) {
        report.satisfied = false;
        report.missingPluginIds.push_back(id);
        return report;
    }

    for (const std::string& dependencyId : entry->descriptor.dependencies) {
        const Entry* dependency = findEntry(dependencyId);
        if (dependency == nullptr || !dependency->enabled || !dependency->initialized) {
            report.satisfied = false;
            report.missingPluginIds.push_back(dependencyId);
        }
    }
    return report;
}

bool PluginRegistry::isInitialized(const std::string& id) const {
    const Entry* entry = findEntry(id);
    return entry != nullptr && entry->initialized;
}

bool PluginRegistry::isEnabled(const std::string& id) const {
    const Entry* entry = findEntry(id);
    return entry != nullptr && entry->enabled;
}

std::size_t PluginRegistry::size() const {
    return entries_.size();
}

void PluginRegistry::clear() {
    shutdownAll();
    entries_.clear();
}

PluginRegistry::Entry* PluginRegistry::findEntry(const std::string& id) {
    auto it = std::find_if(entries_.begin(), entries_.end(), [&id](const Entry& entry) {
        return entry.descriptor.id == id;
    });
    return it == entries_.end() ? nullptr : &(*it);
}

const PluginRegistry::Entry* PluginRegistry::findEntry(const std::string& id) const {
    auto it = std::find_if(entries_.begin(), entries_.end(), [&id](const Entry& entry) {
        return entry.descriptor.id == id;
    });
    return it == entries_.end() ? nullptr : &(*it);
}

bool PluginRegistry::dependenciesSatisfied(const Entry& entry) const {
    for (const std::string& dependencyId : entry.descriptor.dependencies) {
        const Entry* dependency = findEntry(dependencyId);
        if (dependency == nullptr || !dependency->enabled || !dependency->initialized) {
            return false;
        }
    }
    return true;
}

} // namespace dnxr
