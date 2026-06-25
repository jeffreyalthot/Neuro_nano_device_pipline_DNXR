#include "HardwareBenchSession.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
namespace dnxr {
static bool has(const std::string& s,const std::string& k,const std::string& v){ return s.find("\""+k+"\"")!=std::string::npos && s.find(v)!=std::string::npos; }
HardwareBenchSessionManifest HardwareBenchSession::dryRunManifest(){ HardwareBenchSessionManifest m; m.sessionId="HB-0.6.6-DRY-RUN"; m.safetyKernelEnabled=true; m.emergencyStopVerified=true; m.traceRecordingEnabled=true; return m; }
HardwareBenchSessionManifest HardwareBenchSession::loadManifest(const std::string& path){ std::ifstream in(path); if(!in) throw std::runtime_error("manifest not readable"); std::ostringstream ss; ss<<in.rdbuf(); auto s=ss.str(); auto m=dryRunManifest(); m.humanSubjectPresent=has(s,"human_subject_present","true"); m.activeStimulationEnabled=has(s,"active_stimulation_enabled","true"); if(s.find("PHANTOM")!=std::string::npos) m.targetType="PHANTOM"; if(s.find("HUMAN_CLINICAL_TRIAL_AUTHORIZED")!=std::string::npos) m.deploymentMode=RuntimeDeploymentMode::HumanClinicalTrialAuthorized; return m; }
bool HardwareBenchSession::isAccepted(const HardwareBenchSessionManifest& m,std::string* r){ if(m.humanSubjectPresent){ if(r)*r="human_subject_present_forbidden_in_hardware_bench"; return false;} if(m.targetType!="DUMMY_LOAD"&&m.targetType!="PHANTOM"){ if(r)*r="target_must_be_dummy_load_or_phantom"; return false;} if(!m.emergencyStopVerified){ if(r)*r="emergency_stop_required"; return false;} if(!m.traceRecordingEnabled){ if(r)*r="trace_recorder_required"; return false;} return true; }
}
