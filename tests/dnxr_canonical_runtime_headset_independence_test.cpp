#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

namespace {
std::string read(const char* path) {
    std::ifstream in(path);
    if (!in && path[0] == '.') {
        in.open(path + 3);
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
}

int main() {
    const std::string controller = read("../src/core/RuntimeController.cpp");
    const std::string services = read("../src/runtime/RuntimeStageServices.h");
    const std::string acquisition = read("../src/runtime/RuntimeAcquisitionStage.cpp");
    const std::string render = read("../src/runtime/RuntimeRenderStage.cpp");

    assert(controller.find("std::make_unique<XRHeadsetInterface>") == std::string::npos);
    assert(controller.find("pullBioTelemetry") == std::string::npos);
    assert(controller.find("pushSensoryFrame") == std::string::npos);
    assert(services.find("XRHeadsetInterface") == std::string::npos);
    assert(acquisition.find("pullBioTelemetry") == std::string::npos);
    assert(render.find("pushSensoryFrame") == std::string::npos);
    assert(render.find("ObserverMirror") != std::string::npos);
    return 0;
}
