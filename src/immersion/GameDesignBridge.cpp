#include "GameDesignBridge.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

namespace dnxr {

float GameDesignBridge::clamp01(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

void GameDesignBridge::integrateBaseGameplaySystems(VirtualWorldState& world) const {
    // Le modèle VirtualWorldState actuel expose une API compacte (scène + intensité).
    // On initialise donc une base gameplay cohérente via cette API stable.
    world.setScene("AvatarSelectionLounge");
    world.setIntensity(0.45F);

    const std::filesystem::path baseMap = "assets/maps/base_generated_hub.blendmap";
    const std::filesystem::path baseSkin = "assets/avatars/skins/default_avatar.skin";
    std::filesystem::create_directories(baseMap.parent_path());
    std::filesystem::create_directories(baseSkin.parent_path());

    if (!std::filesystem::exists(baseMap)) {
        std::ofstream out(baseMap);
        out << "# Auto-generated base map\n";
        out << "map_id=base_generated_hub\n";
        out << "spawn=0,0,0\n";
    }
    if (!std::filesystem::exists(baseSkin)) {
        std::ofstream out(baseSkin);
        out << "# Auto-generated default skin\n";
        out << "skin_id=default_avatar\n";
        out << "palette=neutral\n";
    }
}

void GameDesignBridge::applyHealthState(VirtualWorldState& world, float hp, float hpMax) const {
    const float safeHpMax = std::max(1.0F, hpMax);
    const float hpNormalized = clamp01(std::clamp(hp, 0.0F, safeHpMax) / safeHpMax);
    // HP faible => baisse d'intensité immersive pour signaler la criticité.
    world.setIntensity(0.2F + (0.8F * hpNormalized));
    if (hpNormalized <= 0.0F) {
        world.setScene("RecoveryChamber");
    }
}

void GameDesignBridge::processRespawn(VirtualWorldState& world, bool respawnSelected, std::size_t spawnIndex) const {
    if (!respawnSelected) {
        return;
    }
    const std::size_t safeIndex = std::min<std::size_t>(spawnIndex, 2U);
    const std::string scene = (safeIndex == 0U) ? "AtriumNorth"
                              : (safeIndex == 1U) ? "AtriumSouth"
                                                  : "HubCenter";
    world.setScene(scene);
    world.setIntensity(1.0F);
}

} // namespace dnxr
