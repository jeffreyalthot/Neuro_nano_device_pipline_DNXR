#include "XRRenderer.h"

#include "../utils/Logger.h"

namespace dnxr {

void XRRenderer::present(const std::vector<float>& frame, const VirtualWorldState& world) const {
    // This function submits the generated frame and active Blender-compatible map metadata.
    Logger::info("Presenting XR frame channels=" + std::to_string(frame.size()) +
                 " map=" + world.activeMap.displayName +
                 " format=" + world.activeMap.sourceFormat +
                 " asset=" + world.activeMap.sourcePath);
}

void XRRenderer::renderSafetyOverlay(bool quickMenuVisible) const {
    Logger::info("Rendering top-left circular safety button with three dots for quick menu access.");
    if (quickMenuVisible) {
        Logger::info("Rendering quick menu option: Quitter (safe progressive simulation exit).");
    }
}

} // namespace dnxr
