#include "RuntimeDiagnostics.h"
#include <sstream>
namespace dnxr { void RuntimeDiagnostics::record(PipelineStepResult result) { steps_.push_back(std::move(result)); } std::string RuntimeDiagnostics::report() const { std::ostringstream out; out << "diagnostic_steps=" << steps_.size(); for (const auto& s: steps_) out << "\n- " << s.step << ": " << (s.accepted?"accepted":"rejected") << " (" << s.reason << ")"; return out.str(); } }
