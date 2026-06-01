#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class AssetType {
    Map,
    CharacterPlaceholder,
    PropPlaceholder,
    Unknown
};

struct AssetDescriptor {
    std::string id;
    AssetType type = AssetType::Unknown;
    std::string path;
    bool required = false;
};

class AssetRegistry {
public:
    void registerAsset(const AssetDescriptor& asset);
    bool discoverFromDirectory(const std::string& rootPath);

    const AssetDescriptor* findById(const std::string& id) const;
    std::vector<AssetDescriptor> assetsByType(AssetType type) const;
    std::vector<AssetDescriptor> missingRequiredAssets() const;
    const std::vector<AssetDescriptor>& assets() const;
    std::size_t size() const;
    void clear();

private:
    static AssetType inferType(const std::string& path);
    static std::string makeAssetId(const std::string& path);

    std::vector<AssetDescriptor> assets_;
};

std::string toString(AssetType type);

} // namespace dnxr
