#pragma once
namespace dnxr { struct SafeImpactResult { bool painSimulationBlocked=true, impactMappedToPressureOnly=true; float pressure=0, rampDownMs=50; }; class SafeImpactModel { public: SafeImpactResult mapImpact(float impactIntensity) const; }; }
