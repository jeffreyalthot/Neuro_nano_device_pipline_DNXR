#pragma once
#include <string>
namespace dnxr { struct CalibrationRollbackDecision { bool rollbackRequired=false; bool emergencyStopRequired=false; std::string targetProfileId; std::string reason; }; class CalibrationRollbackGuard { public: CalibrationRollbackDecision evaluate(double driftScore, bool currentProfileValid, const std::string& lastKnownSafeProfileId) const; }; }
