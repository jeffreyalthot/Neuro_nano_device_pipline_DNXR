#include "../src/assets/AssetRegistry.h"
#include "../src/save/SaveManager.h"
#include "../src/save/SaveMigration.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>

int main() {
    dnxr::AssetRegistry registry;
    bool discoveredAssets = registry.discoverFromDirectory("assets");
    if (registry.assetsByType(dnxr::AssetType::Map).empty()) {
        registry.clear();
        discoveredAssets = registry.discoverFromDirectory("../assets");
    }
    assert(discoveredAssets);
    assert(registry.size() > 0U);
    assert(!registry.assetsByType(dnxr::AssetType::Map).empty());
    const auto discoveredMaps = registry.assetsByType(dnxr::AssetType::Map);
    assert(registry.findById(discoveredMaps.front().id) != nullptr);
    assert(dnxr::toString(dnxr::AssetType::Map) == "map");

    registry.registerAsset({"required_missing", dnxr::AssetType::Map, "assets/maps/missing_required.blendmap", true});
    assert(registry.missingRequiredAssets().size() == 1U);

    const std::string saveDir = "build/save_manager_test";
    dnxr::SaveManager manager(saveDir);
    dnxr::SaveProfile profile;
    profile.characterId = "avatar_test";
    profile.lastMapId = "starter_town_hub";
    assert(manager.saveProfile(profile, "slot_test"));

    dnxr::SaveProfile loaded;
    assert(manager.loadProfile("slot_test", loaded));
    assert(loaded.saveVersion == dnxr::SaveMigration::latestVersion());
    assert(loaded.profileId == "slot_test");
    assert(loaded.characterId == "avatar_test");
    assert(loaded.lastMapId == "starter_town_hub");
    assert(loaded.checksum == dnxr::SaveManager::computeChecksum(loaded));
    assert(manager.validateFile(manager.pathForSlot("slot_test")));
    assert(!manager.listSlots().empty());

    const std::string legacyPath = manager.pathForSlot("legacy_slot");
    {
        std::ofstream legacy(legacyPath);
        legacy << "{\n"
               << "  \"save_slot\": \"legacy_slot\",\n"
               << "  \"character_id\": \"legacy_avatar\",\n"
               << "  \"world\": { \"last_map_id\": \"legacy_map\" }\n"
               << "}\n";
    }

    dnxr::SaveProfile legacyLoaded;
    assert(manager.loadProfile("legacy_slot", legacyLoaded));
    assert(legacyLoaded.saveVersion == 2);
    assert(legacyLoaded.profileId == "legacy_slot");
    assert(legacyLoaded.characterId == "legacy_avatar");
    assert(legacyLoaded.lastMapId == "legacy_map");

    {
        std::ofstream corrupt(manager.pathForSlot("corrupt_slot"));
        dnxr::SaveProfile tampered = loaded;
        tampered.characterId = "tampered_avatar";
        tampered.checksum = loaded.checksum;
        corrupt << dnxr::SaveManager::serialize(tampered);
    }
    dnxr::SaveProfile corruptLoaded;
    assert(!manager.loadProfile("corrupt_slot", corruptLoaded));

    std::remove(manager.pathForSlot("slot_test").c_str());
    std::remove(manager.pathForSlot("legacy_slot").c_str());
    std::remove(manager.pathForSlot("corrupt_slot").c_str());
    return 0;
}
