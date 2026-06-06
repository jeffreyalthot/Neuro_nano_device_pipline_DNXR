#pragma once
namespace dnxr { struct AdaptiveCalibrationInput { bool emergency=false,poorSignal=false,patientDistress=false; float requestedDelta=0; }; class SafeAdaptiveCalibration { public: float boundedDelta(const AdaptiveCalibrationInput& i, double minutes) const; }; }
