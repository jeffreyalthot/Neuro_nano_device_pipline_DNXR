#pragma once
#include <string>
namespace dnxr {
enum class RuntimeDeploymentMode { SimulationOnly, HardwareBenchNoHuman, PreclinicalPhantom, AuthorizedResearchOnly, HumanClinicalTrialAuthorized };
RuntimeDeploymentMode runtimeDeploymentModeFromString(const std::string& value);
std::string toString(RuntimeDeploymentMode mode);
}
