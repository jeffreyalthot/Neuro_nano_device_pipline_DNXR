#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    assert(input.good());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool sourceFile(const std::filesystem::path& path) {
    const std::string ext = path.extension().string();
    return ext == ".h" || ext == ".hpp" || ext == ".cpp" || ext == ".cc" || ext == ".cxx";
}

std::string genericPath(const std::filesystem::path& path) {
    return path.generic_string();
}

std::vector<std::string> publicTypes(const std::string& content) {
    std::vector<std::string> out;
    const std::regex typePattern(R"(\b(class|struct|enum(?:\s+class)?)\s+([A-Za-z_][A-Za-z0-9_]*))");
    for (std::sregex_iterator it(content.begin(), content.end(), typePattern), end; it != end; ++it) {
        const std::string name = (*it)[2].str();
        if (name != "public" && name != "private" && name != "protected") {
            out.push_back(name);
        }
    }
    return out;
}

} // namespace

int main() {
    const std::filesystem::path root = std::filesystem::current_path();
    const std::string fileManifest = readFile(root / "docs/runtime_unification/DNXR_FILE_DISPOSITION_MANIFEST.csv");
    const std::string symbolManifest = readFile(root / "docs/runtime_unification/DNXR_SYMBOL_PRESERVATION_MANIFEST.csv");

    std::size_t sourceFiles = 0U;
    std::size_t headers = 0U;
    std::size_t publicTypeCount = 0U;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root / "src")) {
        if (!entry.is_regular_file() || !sourceFile(entry.path())) {
            continue;
        }
        const auto relative = genericPath(std::filesystem::relative(entry.path(), root));
        ++sourceFiles;
        if (fileManifest.find(relative) == std::string::npos) {
            std::cerr << "Missing file disposition for " << relative << '\n';
            return 2;
        }
        if (entry.path().extension() == ".h" || entry.path().extension() == ".hpp") {
            ++headers;
            for (const auto& type : publicTypes(readFile(entry.path()))) {
                ++publicTypeCount;
                if (symbolManifest.find(type) == std::string::npos || symbolManifest.find(relative) == std::string::npos) {
                    std::cerr << "Missing public type preservation entry for " << type << " in " << relative << '\n';
                    return 3;
                }
            }
        }
    }

    assert(sourceFiles > 300U);
    assert(headers > 100U);
    assert(publicTypeCount > 100U);

    const std::vector<std::string> requiredSubsystems = {
        "src/affective/", "src/app/", "src/assets/", "src/bench/", "src/calibration/",
        "src/clinical/", "src/cognition/", "src/compat/", "src/config/", "src/core/", "src/devices/",
        "src/emotion/",
        "src/evidence/", "src/full_dive/", "src/hardware/", "src/hardware_bench/", "src/hil/",
        "src/immersion/", "src/input/", "src/interface/", "src/memory/", "src/mmorpg/",
        "src/network/", "src/neural/", "src/neural_chip/", "src/neuro/", "src/neurotech/",
        "src/perception/", "src/performance/", "src/physics/", "src/plugin/", "src/qms/", "src/readiness/",
        "src/real/", "src/reality/", "src/rendering/", "src/replay/", "src/runtime/", "src/safety/",
        "src/safety_kernel/", "src/save/", "src/security/", "src/signal/", "src/simulation/",
        "src/social/", "src/subconscious/", "src/time/", "src/utils/"};
    for (const auto& subsystem : requiredSubsystems) {
        if (fileManifest.find(subsystem) == std::string::npos) {
            std::cerr << "Missing subsystem disposition coverage for " << subsystem << '\n';
            return 4;
        }
    }

    std::cout << "Disposition manifest covers " << sourceFiles << " source files, "
              << headers << " headers, and " << publicTypeCount << " public type declarations.\n";
    return 0;
}
