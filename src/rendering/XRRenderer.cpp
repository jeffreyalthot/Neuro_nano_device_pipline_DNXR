#include "XRRenderer.h"

#include "../utils/Logger.h"

namespace dnxr {

void XRRenderer::present(const std::vector<float>& frame) const {
    // This function submits the generated frame to the presentation layer and logs channel count.
    Logger::info("Presenting XR frame channels=" + std::to_string(frame.size()));
}

void XRRenderer::renderSafetyOverlay(bool quickMenuVisible) const {
    Logger::info("Rendering top-left circular safety button with three dots for quick menu access.");
    if (quickMenuVisible) {
        Logger::info("Rendering quick menu option: Quitter (safe progressive simulation exit).");
    }
}

} // namespace dnxr
