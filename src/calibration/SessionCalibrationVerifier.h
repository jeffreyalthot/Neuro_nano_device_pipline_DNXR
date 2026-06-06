#pragma once
#include <string>
namespace dnxr { struct SessionCalibrationInputs { bool patientIdentity=false,deviceIdentity=false,calibrationFresh=false,baselineStable=false,stopSignal=false,realityAnchor=false,physicianConfirmation=false; }; struct SessionCalibrationResult { bool allowed=false; std::string reason="not_checked"; }; class SessionCalibrationVerifier { public: SessionCalibrationResult verify(const SessionCalibrationInputs& i) const; }; }
