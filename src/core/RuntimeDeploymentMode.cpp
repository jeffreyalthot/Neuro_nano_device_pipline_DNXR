#include "RuntimeDeploymentMode.h"
#include <stdexcept>
namespace dnxr {
RuntimeDeploymentMode runtimeDeploymentModeFromString(const std::string& v) {
    if (v == "SIMULATION_ONLY" || v == "SimulationOnly") return RuntimeDeploymentMode::SimulationOnly;
    if (v == "HARDWARE_BENCH_NO_HUMAN" || v == "HardwareBenchNoHuman") return RuntimeDeploymentMode::HardwareBenchNoHuman;
    if (v == "PRECLINICAL_PHANTOM" || v == "PreclinicalPhantom") return RuntimeDeploymentMode::PreclinicalPhantom;
    if (v == "ANIMAL_OR_TISSUE_RESEARCH_AUTHORIZED" || v == "AUTHORIZED_RESEARCH_ONLY" || v == "AuthorizedResearchOnly") return RuntimeDeploymentMode::AuthorizedResearchOnly;
    if (v == "HUMAN_CLINICAL_TRIAL_AUTHORIZED" || v == "HumanClinicalTrialAuthorized") return RuntimeDeploymentMode::HumanClinicalTrialAuthorized;
    throw std::invalid_argument("unknown runtime deployment mode: " + v);
}
std::string toString(RuntimeDeploymentMode mode) {
    switch (mode) {
    case RuntimeDeploymentMode::SimulationOnly: return "SIMULATION_ONLY";
    case RuntimeDeploymentMode::HardwareBenchNoHuman: return "HARDWARE_BENCH_NO_HUMAN";
    case RuntimeDeploymentMode::PreclinicalPhantom: return "PRECLINICAL_PHANTOM";
    case RuntimeDeploymentMode::AuthorizedResearchOnly: return "AUTHORIZED_RESEARCH_ONLY";
    case RuntimeDeploymentMode::HumanClinicalTrialAuthorized: return "HUMAN_CLINICAL_TRIAL_AUTHORIZED";
    }
    return "UNKNOWN";
}
}
