#include "GameDesignBridge.h"

#include <algorithm>

namespace dnxr {

float GameDesignBridge::clamp01(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

void GameDesignBridge::integrateBaseGameplaySystems(VirtualWorldState& world) const {
    world.gameBridgeEnabled = true;
    world.uiAssets = UIAssetSpec{};
    world.spawnPoints = {
        {"spawn_alpha", "Atrium_North", 0.0F, 0.0F, 2.0F},
        {"spawn_beta", "Atrium_South", 18.0F, 0.0F, -7.5F},
        {"spawn_gamma", "Hub_Center", -9.0F, 0.0F, 9.0F}
    };
    world.activeSpawnPointId = world.spawnPoints.front().id;

    world.cameras = {
        {"cam_fp", "first_person", true, 110.0F, 0.02F, 600.0F},
        {"cam_tp", "third_person", false, 90.0F, 0.10F, 900.0F}
    };

    world.audioSensors = {
        {"ear_left", -0.10F, 0.0F, 0.05F, 8.0F},
        {"ear_right", 0.10F, 0.0F, 0.05F, 8.0F},
        {"torso_zone", 0.0F, 0.0F, 0.0F, 4.0F}
    };

    world.characterSelection.selectionMenuEnabled = true;
    world.characterSelection.selectionMapId = "avatar_selection_lounge";
    world.characterSelection.selectedCharacterId = "base_sphere_skin";
    world.saveState.lastKnownMapId = world.activeMap.mapId;
    world.saveState.lastKnownSpawnPointId = world.activeSpawnPointId;
    world.saveState.hp = world.hpCurrent;
}

void GameDesignBridge::applyHealthState(VirtualWorldState& world, float hp, float hpMax) const {
    const float safeHpMax = std::max(1.0F, hpMax);
    world.hpNormalized = clamp01(hp / safeHpMax);
    world.hpCurrent = std::clamp(hp, 0.0F, safeHpMax);
    world.hpMax = safeHpMax;

    world.avatarTranslucent = world.hpCurrent <= 0.0F;
    world.avatarTransparency = world.avatarTranslucent ? 0.20F : 1.0F;
    world.respawnMenuVisible = world.avatarTranslucent;
}

void GameDesignBridge::processRespawn(VirtualWorldState& world, bool respawnSelected, std::size_t spawnIndex) const {
    if (!world.respawnMenuVisible || !respawnSelected || world.spawnPoints.empty()) {
        return;
    }

    const std::size_t safeIndex = std::min(spawnIndex, world.spawnPoints.size() - 1);
    world.activeSpawnPointId = world.spawnPoints[safeIndex].id;
    world.hpCurrent = world.hpMax;
    world.hpNormalized = 1.0F;
    world.avatarTranslucent = false;
    world.avatarTransparency = 1.0F;
    world.respawnMenuVisible = false;
}

} // namespace dnxr
