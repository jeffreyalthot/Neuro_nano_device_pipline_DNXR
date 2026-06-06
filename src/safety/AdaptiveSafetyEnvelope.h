#pragma once
namespace dnxr { struct AdaptiveSafetyInputs { float stress=0,fatigue=0,signalQuality=1,calibrationDrift=0,latency=0; bool patientStopUncertain=false; }; class AdaptiveSafetyEnvelope { public: float intensityScale(const AdaptiveSafetyInputs& i) const; }; }
