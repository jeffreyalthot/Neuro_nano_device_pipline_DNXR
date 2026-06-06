#pragma once
#include <cstdint>
namespace dnxr { struct PatientSessionClockState { std::uint64_t sessionElapsedMs=0, virtualSceneElapsedMs=0, realWorldTimeAnchorMs=0, decompressionTimeMs=0, exposureTimeMs=0, lastPatientConfirmationMs=0, lastPhysicianConfirmationMs=0; }; class PatientSessionClock { public: void start(std::uint64_t nowMs); void tick(std::uint64_t nowMs, bool inDecompression); void confirmPatient(std::uint64_t nowMs); void confirmPhysician(std::uint64_t nowMs); PatientSessionClockState state() const; private: std::uint64_t startMs_=0; PatientSessionClockState state_; }; }
