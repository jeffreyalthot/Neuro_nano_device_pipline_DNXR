#include "ModalityCalibrationEngine.h"
namespace dnxr { void ModalityCalibrationEngine::setCalibration(const std::string& m,float c){confidence_[m]=c;} ModalityCalibrationResult ModalityCalibrationEngine::verify(const std::string& m) const{ auto it=confidence_.find(m); return {it!=confidence_.end()&&it->second>=0.75f,it==confidence_.end()?0.0f:it->second}; } }
