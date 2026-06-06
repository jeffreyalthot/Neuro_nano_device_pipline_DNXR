#pragma once
#include <vector>
namespace dnxr { struct PatientBaseline { float resting=0,attention=0,stress=0,fatigue=0,motor=0,sensoryResponse=0; }; class PatientBaselineTracker { public: PatientBaseline update(const std::vector<float>& features, float learningRate=0.01f); PatientBaseline baseline() const; private: PatientBaseline b_; }; }
