#include "../src/app/RuntimeCli.h"

#include <cassert>
#include <string>
#include <vector>

namespace {
int argcOf(const std::vector<const char*>& args) { return static_cast<int>(args.size()); }
char** argvOf(std::vector<const char*>& args) { return const_cast<char**>(args.data()); }
}

int main() {
    {
        std::vector<const char*> args{"distributed_neural_xr", "--help"};
        const auto parsed = dnxr::RuntimeCli::parse(argcOf(args), argvOf(args));
        assert(parsed.action == dnxr::RuntimeCliAction::Help);
        assert(parsed.exitCode == 0);
        assert(parsed.message.find("No command starts a neural cycle") != std::string::npos);
    }
    {
        std::vector<const char*> args{"distributed_neural_xr", "--version"};
        const auto parsed = dnxr::RuntimeCli::parse(argcOf(args), argvOf(args));
        assert(parsed.action == dnxr::RuntimeCliAction::Version);
        assert(parsed.exitCode == 0);
    }
    {
        std::vector<const char*> args{"distributed_neural_xr"};
        const auto parsed = dnxr::RuntimeCli::parse(argcOf(args), argvOf(args));
        assert(parsed.action == dnxr::RuntimeCliAction::Error);
        assert(parsed.message.find("missing command") != std::string::npos);
    }
    {
        std::vector<const char*> args{"distributed_neural_xr", "run", "--config", "config/runtime_config.json"};
        const auto parsed = dnxr::RuntimeCli::parse(argcOf(args), argvOf(args));
        assert(parsed.action == dnxr::RuntimeCliAction::Error);
        assert(parsed.message.find("run requires --profile") != std::string::npos);
    }
    return 0;
}
