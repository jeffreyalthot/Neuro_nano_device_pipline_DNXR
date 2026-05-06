#include "XRRenderer.h"
#include "../utils/Logger.h"
namespace dnxr { void XRRenderer::present(const std::vector<float>& frame) const { Logger::info("Presenting XR frame channels="+std::to_string(frame.size())); } }
