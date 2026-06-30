#include "EvidenceRevocationList.h"
#include <algorithm>
namespace dnxr { bool EvidenceRevocationList::isRevoked(const std::string& s) const { return std::find(revokedSignatures.begin(), revokedSignatures.end(), s)!=revokedSignatures.end(); } }
