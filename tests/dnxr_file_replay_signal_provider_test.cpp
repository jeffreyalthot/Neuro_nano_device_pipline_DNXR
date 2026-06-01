#include "../src/input/FileReplaySignalProvider.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace {

void writeReplayFile(const std::string& path) {
    std::ofstream output(path);
    output << "# replay fixture\n";
    output << " 0.10, 0.20, 0.30 \n";
    output << "invalid,line\n";
    output << "0.40,0.50,0.60\n";
    output << "nan,0.2,0.3\n";
    output << "0.70, 0.80, 0.90\n";
}

} // namespace

int main() {
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    const std::string replayPath = "build/dnxr_file_replay_signal_provider_test.csv";
    writeReplayFile(replayPath);

    dnxr::FileReplaySignalProvider provider(replayPath);
    assert(provider.name() == "file_replay");
    assert(!provider.usingFallback());
    assert(provider.frameCount() == 3U);

    const std::vector<float> first = provider.nextSignal(0);
    assert(first.size() == 3U);
    assert(first[0] == 0.10f);
    assert(first[2] == 0.30f);

    const std::vector<float> second = provider.nextSignal(1);
    assert(second[0] == 0.40f);
    assert(second[2] == 0.60f);

    const std::vector<float> wrapped = provider.nextSignal(4);
    assert(wrapped == second);

    const std::vector<float> negativeCycle = provider.nextSignal(-10);
    assert(negativeCycle == first);

    dnxr::FileReplaySignalProvider missing("build/does_not_exist_replay_signal.csv");
    assert(missing.usingFallback());
    assert(missing.frameCount() == 1U);
    assert(missing.nextSignal(100).size() == 5U);

    const std::string invalidPath = "build/dnxr_file_replay_signal_provider_invalid.csv";
    {
        std::ofstream invalid(invalidPath);
        invalid << "# comments only\n";
        invalid << "bad\n";
        invalid << "1.0,,2.0\n";
    }
    dnxr::FileReplaySignalProvider invalidProvider(invalidPath);
    assert(invalidProvider.usingFallback());
    assert(invalidProvider.frameCount() == 1U);

    std::remove(replayPath.c_str());
    std::remove(invalidPath.c_str());
    dnxr::Logger::resetToDefaultSinks();
    return 0;
}
