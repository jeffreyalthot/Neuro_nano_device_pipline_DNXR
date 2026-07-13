#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string read(const char* path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void assertAbsent(const std::string& text, const std::string& needle) {
    if (text.find(needle) != std::string::npos) {
        assert(false && "forbidden runtime convergence token present");
    }
}
}

int main() {
    const std::string runnerHeader = read("src/runtime/RuntimePipelineRunner.h");
    const std::string runnerImpl = read("src/runtime/RuntimePipelineRunner.cpp");
    assertAbsent(runnerHeader, "LegacyRunInput");
    assertAbsent(runnerHeader, "runLegacy");
    assertAbsent(runnerImpl, "runLegacy");
    assertAbsent(runnerImpl, "finishLegacyCycle");

    const std::string canonicalHeader = read("src/runtime/CanonicalNeuralBciPipeline.h");
    assert(canonicalHeader.find("RuntimePathKind::SecondarySimulation") == std::string::npos);
    assert(canonicalHeader.find("RuntimePathKind::PrimaryReal") == std::string::npos);
    assert(canonicalHeader.find("CanonicalNeuralBciPipeline") != std::string::npos);

    const std::string cmake = read("CMakeLists.txt");
    assert(cmake.find("DNXR_OBSERVER_RUNTIME_REGEX") != std::string::npos);
    assert(cmake.find("devices/XRHeadsetInterface") != std::string::npos);
    assert(cmake.find("rendering/XRRenderer") != std::string::npos);
    assert(cmake.find("Enable simulation adapters for tests/tools only") != std::string::npos);

    const std::vector<const char*> productionScopes = {
        "src/app/RuntimeApplication.cpp",
        "src/main.cpp",
        "src/runtime/CanonicalNeuralBciPipeline.cpp",
        "src/core/RuntimeController.cpp"};
    for (const char* path : productionScopes) {
        const std::string text = read(path);
        assertAbsent(text, "SecondarySimulation");
        assertAbsent(text, "PrimaryReal");
        assertAbsent(text, "runLegacy");
        assertAbsent(text, "LegacyRunInput");
    }

    return 0;
}
