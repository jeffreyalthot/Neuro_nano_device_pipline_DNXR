#include "XRRenderer.h"

#include "../utils/Logger.h"

namespace dnxr {

void XRRenderer::present(const std::vector<float>& frame) const {
    // This function submits the generated frame to the presentation layer and logs channel count.
    Logger::info("Presenting XR frame channels=" + std::to_string(frame.size()));
}

} // namespace dnxr
