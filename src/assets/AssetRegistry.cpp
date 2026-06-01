#include "AssetRegistry.h"

#include <algorithm>
#include <filesystem>
#include <vector>

namespace dnxr {
namespace {

bool hasSuffix(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace

void AssetRegistry::registerAsset(const AssetDescriptor& asset) {
    if (asset.id.empty() || asset.path.empty()) {
        return;
    }

    const auto existing = std::find_if(
        assets_.begin(),
        assets_.end(),
        [&](const AssetDescriptor& candidate) { return candidate.id == asset.id; });

    if (existing != assets_.end()) {
        *existing = asset;
        return;
    }

    assets_.push_back(asset);
}

bool AssetRegistry::discoverFromDirectory(const std::string& rootPath) {
    const std::filesystem::path root(rootPath);
    if (!std::filesystem::exists(root)) {
        return false;
    }

    std::vector<std::string> discoveredPaths;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            discoveredPaths.push_back(entry.path().generic_string());
        }
    }
    std::sort(discoveredPaths.begin(), discoveredPaths.end());

    for (const auto& path : discoveredPaths) {
        const AssetType type = inferType(path);
        if (type == AssetType::Unknown) {
            continue;
        }

        registerAsset({makeAssetId(path), type, path, false});
    }

    return true;
}

const AssetDescriptor* AssetRegistry::findById(const std::string& id) const {
    const auto found = std::find_if(
        assets_.begin(),
        assets_.end(),
        [&](const AssetDescriptor& asset) { return asset.id == id; });
    return found == assets_.end() ? nullptr : &(*found);
}

std::vector<AssetDescriptor> AssetRegistry::assetsByType(AssetType type) const {
    std::vector<AssetDescriptor> out;
    for (const auto& asset : assets_) {
        if (asset.type == type) {
            out.push_back(asset);
        }
    }
    return out;
}

std::vector<AssetDescriptor> AssetRegistry::missingRequiredAssets() const {
    std::vector<AssetDescriptor> missing;
    for (const auto& asset : assets_) {
        if (asset.required && !std::filesystem::exists(asset.path)) {
            missing.push_back(asset);
        }
    }
    return missing;
}

const std::vector<AssetDescriptor>& AssetRegistry::assets() const {
    return assets_;
}

std::size_t AssetRegistry::size() const {
    return assets_.size();
}

void AssetRegistry::clear() {
    assets_.clear();
}

AssetType AssetRegistry::inferType(const std::string& path) {
    if (hasSuffix(path, ".blendmap")) {
        return AssetType::Map;
    }
    if (path.find("characters") != std::string::npos) {
        return AssetType::CharacterPlaceholder;
    }
    if (path.find("props") != std::string::npos) {
        return AssetType::PropPlaceholder;
    }
    return AssetType::Unknown;
}

std::string AssetRegistry::makeAssetId(const std::string& path) {
    const auto filename = std::filesystem::path(path).stem().generic_string();
    return filename.empty() ? path : filename;
}

std::string toString(AssetType type) {
    switch (type) {
        case AssetType::Map: return "map";
        case AssetType::CharacterPlaceholder: return "character_placeholder";
        case AssetType::PropPlaceholder: return "prop_placeholder";
        case AssetType::Unknown: return "unknown";
    }
    return "unknown";
}

} // namespace dnxr
