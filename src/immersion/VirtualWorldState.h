#pragma once

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
    float intensity{0.5F};
    bool quickMenuVisible{false};
    bool safeExitRequested{false};
    bool safeExitCompleted{false};
    float safeExitComfortFade{1.0F};
};

} // namespace dnxr
