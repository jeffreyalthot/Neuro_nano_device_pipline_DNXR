#pragma once
namespace dnxr { struct PatientSpecificSafetyEnvelope { float visualSensitivity=1,audioSensitivity=1,vestibularSensitivity=1,hapticSensitivity=1,dissociationRisk=0,cybersicknessRisk=0,seizureRisk=0,panicRisk=0,calibrationConfidence=0; float riskMultiplier() const; }; }
