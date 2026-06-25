#include "qms/QmsTrustStore.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>
namespace dnxr { QmsTrustStore::QmsTrustStore(std::string root): root_(std::move(root)) {} bool QmsTrustStore::isTrustedKey(const std::string& keyId) const { if (keyId.empty()) return false; std::ifstream in(std::filesystem::path(root_) / "qms/trust/qms_trust_store.json"); for(auto probe=std::filesystem::current_path(); !in && probe.has_parent_path(); probe=probe.parent_path()){ in.open(probe / "qms/trust/qms_trust_store.json"); if(probe==probe.parent_path()) break; } if (!in) return false; std::ostringstream ss; ss << in.rdbuf(); return ss.str().find(keyId) != std::string::npos; } }
