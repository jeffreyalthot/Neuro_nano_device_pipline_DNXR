#include "RealModePolicy.h"
namespace dnxr {
RealModePermissions RealModePolicy::permissionsFor(RuntimeDeploymentMode mode) const {
    switch (mode) {
    case RuntimeDeploymentMode::SimulationOnly: return {true,true,false,false,false,false};
    case RuntimeDeploymentMode::HardwareBenchNoHuman: return {true,true,true,true,false,false};
    case RuntimeDeploymentMode::PreclinicalPhantom: return {true,true,true,true,false,true};
    case RuntimeDeploymentMode::AuthorizedResearchOnly: return {true,true,true,true,false,true};
    case RuntimeDeploymentMode::HumanClinicalTrialAuthorized: return {true,true,true,true,true,true};
    }
    return {};
}
}
