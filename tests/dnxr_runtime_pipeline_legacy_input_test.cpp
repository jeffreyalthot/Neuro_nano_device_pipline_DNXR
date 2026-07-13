#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

namespace {
std::string read(const char* path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}
}

int main() {
    const std::string header = read("src/runtime/RuntimePipelineRunner.h");
    const std::string impl = read("src/runtime/RuntimePipelineRunner.cpp");
    assert(header.find("LegacyRunInput") == std::string::npos);
    assert(header.find("runLegacy") == std::string::npos);
    assert(impl.find("runLegacy") == std::string::npos);
    assert(impl.find("finishLegacyCycle") == std::string::npos);
    return 0;
}
