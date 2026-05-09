#pragma once

#include <string>

namespace dnxr {

struct VirtualWorldState {
    std::string sceneName{"BootChamber"};
    float intensity{0.5F};
    bool quickMenuVisible{false};
    bool safeExitRequested{false};
    bool safeExitCompleted{false};
    float safeExitComfortFade{1.0F};
};

} // namespace dnxr
