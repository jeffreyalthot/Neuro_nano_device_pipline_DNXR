#pragma once

#include "SpatialAudioEngine.h"

#include <string>
#include <vector>

namespace dnxr {

struct VirtualMapAsset {
    std::string mapId;
    std::string displayName;
    std::string sourceFormat;
    std::string sourcePath;
};

struct SpawnPoint {
    std::string id;
    std::string zoneName;
    float x;
    float y;
    float z;
};

struct CameraRig {
    std::string id;
    std::string mode;
    bool isActive;
    float fovDegrees;
    float nearClip;
    float farClip;
};

struct AudioCaptureZone {
    std::string id;
    float x;
    float y;
    float z;
    float radiusMeters;
};

struct UIAssetSpec {
    std::string hpBarPng{"assets/ui/hud_hp_bar_1024x128.png"};
    std::string quickMenuPng{"assets/ui/quick_menu_panel_1024x1024.png"};
    std::string respawnMenuPng{"assets/ui/respawn_menu_960x540.png"};
    int hpBarWidth{1024};
    int hpBarHeight{128};
    int quickMenuWidth{1024};
    int quickMenuHeight{1024};
    int respawnMenuWidth{960};
    int respawnMenuHeight{540};
};

struct VirtualWorldState {
    std::string sceneName{"BootChamber"};
    VirtualMapAsset activeMap{
        "default_blender_map",
        "Default Nexus Atrium",
        "blender-map-v1",
        "assets/maps/default_nexus_atrium.blendmap"};
    std::vector<VirtualMapAsset> availableMaps{
        activeMap
    };
    std::vector<SpatialAudioSource> audioSources{
        {"object-reactor", "environment", 4.0F, -25.0F, 0.45F},
        {"npc-guide", "character", 2.5F, 35.0F, 0.40F},
        {"player-proximity", "player", 3.5F, 150.0F, 0.35F}
    };

    UIAssetSpec uiAssets{};
    std::vector<SpawnPoint> spawnPoints{};
    std::string activeSpawnPointId{"spawn_alpha"};
    std::vector<CameraRig> cameras{};
    std::vector<AudioCaptureZone> audioSensors{};

    float hpCurrent{100.0F};
    float hpMax{100.0F};
    float hpNormalized{1.0F};
    bool avatarTranslucent{false};
    float avatarTransparency{1.0F};
    bool respawnMenuVisible{false};
    bool gameBridgeEnabled{false};

    float intensity{0.5F};
    float environmentSyncPhase{0.0F};
    bool quickMenuVisible{false};
    bool safeExitRequested{false};
    bool safeExitCompleted{false};
    float safeExitComfortFade{1.0F};
};

} // namespace dnxr
