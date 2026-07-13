#include "app/RuntimeApplication.h"
#include "app/RuntimeCli.h"

#include <iostream>

int main(int argc, char** argv) {
    const dnxr::RuntimeCliResult cli = dnxr::RuntimeCli::parse(argc, argv);
    if (cli.action == dnxr::RuntimeCliAction::Help ||
        cli.action == dnxr::RuntimeCliAction::Version ||
        cli.action == dnxr::RuntimeCliAction::ValidateConfig) {
        std::cout << cli.message << '\n';
        return cli.exitCode;
    }
    if (cli.action == dnxr::RuntimeCliAction::Error) {
        std::cerr << cli.message << "\n\n" << dnxr::RuntimeCli::helpText();
        return cli.exitCode;
    }

    dnxr::RuntimeApplication app(cli.config);
    return app.run();
}
