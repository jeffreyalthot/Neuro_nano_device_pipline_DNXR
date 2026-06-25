#include "qms/QmsRevocationList.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
namespace dnxr { QmsRevocationList::QmsRevocationList(std::string root): root_(std::move(root)) {} bool QmsRevocationList::isRevoked(const std::string& keyId) const { if (keyId.empty()) return false; std::ifstream in(std::filesystem::path(root_) / "qms/trust/revoked_keys.json"); for(auto probe=std::filesystem::current_path(); !in && probe.has_parent_path(); probe=probe.parent_path()){ in.open(probe / "qms/trust/revoked_keys.json"); if(probe==probe.parent_path()) break; } if (!in) return false; std::ostringstream ss; ss << in.rdbuf(); return ss.str().find(keyId) != std::string::npos; } }
