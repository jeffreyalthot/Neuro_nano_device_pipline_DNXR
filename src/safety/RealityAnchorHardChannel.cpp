#include "RealityAnchorHardChannel.h"
namespace dnxr { RealityAnchorHardMessage RealityAnchorHardChannel::compose(const std::string& p,const std::string& m) const { RealityAnchorHardMessage r; r.patientName=p; r.supervisionMessage=m; r.realityReturnSignal="independent_reality_anchor_active"; r.operatorVoice="available"; return r; } }
