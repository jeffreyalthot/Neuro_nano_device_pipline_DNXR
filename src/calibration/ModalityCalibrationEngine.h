#pragma once
#include <map>
#include <string>
namespace dnxr { struct ModalityCalibrationResult { bool valid=false; float confidence=0; }; class ModalityCalibrationEngine { public: void setCalibration(const std::string& modality,float confidence); ModalityCalibrationResult verify(const std::string& modality) const; private: std::map<std::string,float> confidence_; }; }
