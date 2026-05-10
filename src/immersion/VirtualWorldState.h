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
    float intensity{0.5F};
    float environmentSyncPhase{0.0F};
    bool quickMenuVisible{false};
    bool safeExitRequested{false};
    bool safeExitCompleted{false};
    float safeExitComfortFade{1.0F};
};

} // namespace dnxr
