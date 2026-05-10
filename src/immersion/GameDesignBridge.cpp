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
        {"cam_fp", "first_person", false, 110.0F, 0.02F, 600.0F, 0.0F, 1.67F, 0.02F},
        {"cam_tp", "third_person", true, 90.0F, 0.10F, 900.0F, -0.35F, 1.70F, -2.25F},
        {"cam_dialog", "dialogue", false, 58.0F, 0.02F, 60.0F, 0.20F, 1.63F, 1.10F},
        {"cam_cine_a", "cinematic", false, 42.0F, 0.10F, 1200.0F, 1.60F, 2.10F, -3.75F}
    };

    world.audioSensors = {
        {"ear_left", -0.10F, 0.0F, 0.05F, 8.0F},
        {"ear_right", 0.10F, 0.0F, 0.05F, 8.0F},
        {"torso_zone", 0.0F, 0.0F, 0.0F, 4.0F},
        {"combat_front", 0.0F, 0.0F, 1.8F, 12.0F},
        {"ambience_far", 0.0F, 0.0F, 0.0F, 24.0F}
    };

    world.menuScreens = {
        {"title_screen", "Title", {"start", "options", "quit"}},
        {"login_create", "Connexion / Creation", {"login", "create_profile", "back"}},
        {"character_select", "Selection Personnage", {"avatars", "appearance", "confirm"}},
        {"inventory", "Inventaire", {"bag", "consumables", "materials"}},
        {"equipment", "Equipement", {"head", "torso", "legs", "feet", "accessory", "weapon"}},
        {"skills", "Competences", {"active", "passive", "mastery"}},
        {"map_quests", "Carte / Quetes", {"world_map", "active_quests", "pins"}},
        {"options", "Options", {"audio", "video", "controls", "accessibility"}},
        {"combat_hud", "Combat HUD", {"hp", "stamina", "skills_bar", "target"}}
    };

    world.characterSelection.wardrobeCatalog = {
        {"hair_short_black", "head", "Cheveux courts noirs", "assets/avatars/wearables/hair_short_black.mesh", "hair_matte"},
        {"hair_long_brown", "head", "Cheveux longs brun", "assets/avatars/wearables/hair_long_brown.mesh", "hair_satin"},
        {"cloth_tunic_blue", "torso", "Tunique bleue", "assets/avatars/wearables/cloth_tunic_blue.mesh", "cloth_soft"},
        {"jacket_leather_black", "torso", "Veste cuir noire", "assets/avatars/wearables/jacket_leather_black.mesh", "leather_standard"},
        {"pants_travel_gray", "legs", "Pantalon voyage gris", "assets/avatars/wearables/pants_travel_gray.mesh", "fabric_dense"},
        {"boots_starter_brown", "feet", "Bottes de base", "assets/avatars/wearables/boots_starter_brown.mesh", "leather_rough"},
        {"cape_windrunner", "accessory", "Cape Windrunner", "assets/avatars/wearables/cape_windrunner.mesh", "cloth_dynamic"}
    };

    world.inputBindings = {
        {"move_forward", "W", "none", "LeftStickUp"},
        {"move_left", "A", "none", "LeftStickLeft"},
        {"move_back", "S", "none", "LeftStickDown"},
        {"move_right", "D", "none", "LeftStickRight"},
        {"interact", "E", "MouseLeft", "X"},
        {"inventory", "I", "none", "DPadUp"},
        {"character", "C", "none", "Y"},
        {"skills", "K", "none", "RB"},
        {"map", "M", "none", "View"},
        {"system_menu", "Esc", "none", "Menu"}
    };

    world.avatarRigProfiles = {
        {"avatar_male_adult_a", "locomotion_humanoid_v1", "combat_blade_v1", "idle_relaxed_v1", {"neutral", "focused", "smile", "concerned"}},
        {"avatar_female_adult_a", "locomotion_humanoid_v1", "combat_blade_v1", "idle_relaxed_v1", {"neutral", "focused", "smile", "concerned"}}
    };

    world.environment.dayNightTimeHours = 9.0F;
    world.environment.weatherMode = "clear";
    world.environment.windIntensity = 0.30F;

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
