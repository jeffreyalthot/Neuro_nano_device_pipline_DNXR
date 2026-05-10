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

void XRRenderer::renderSafetyOverlay(const VirtualWorldState& world) const {
    const std::string quickMenuVisual = world.quickMenuButtonPressed
                                            ? world.uiAssets.quickMenuButtonPressPng
                                            : world.uiAssets.quickMenuButtonNormalPng;
    Logger::info("Rendering in-field quick menu button [" + quickMenuVisual + "] size=" +
                 std::to_string(world.uiAssets.quickMenuButtonWidth) + "x" +
                 std::to_string(world.uiAssets.quickMenuButtonHeight) + ".");

    if (world.quickMenuVisible) {
        const std::string quitVisual = world.quitButtonPressed
                                           ? world.uiAssets.quitButtonPressPng
                                           : world.uiAssets.quitButtonNormalPng;
        Logger::info("Rendering quick menu quit button [" + quitVisual + "] size=" +
                     std::to_string(world.uiAssets.quitButtonWidth) + "x" +
                     std::to_string(world.uiAssets.quitButtonHeight) + ".");

        if (world.safeExitRequested) {
            Logger::info("Quit button activated: progressive neural-safe exit procedure is running.");
        }
    }
}

} // namespace dnxr
