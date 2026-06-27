#include "SafetyDecision.h"
#include <sstream>
namespace dnxr { std::string SafetyDecision::explain() const { std::ostringstream out; out << (accepted?"ACCEPT":"REJECT") << " module=" << module << " rule=" << rule << " expected=" << expected << " received=" << received << " action=" << action << " reproduce=" << reproduce << " correction=" << correction; return out.str(); } }
