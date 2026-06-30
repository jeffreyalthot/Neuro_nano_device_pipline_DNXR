#pragma once
#include "../evidence/EvidenceBundleVerifier.h"
namespace dnxr { struct ClinicalEvidenceBundle { std::vector<EvidenceBundleDocument> documents; EvidenceRevocationList revocationList; std::string trustAnchor; std::string patientId; std::string sessionId; std::string protocolId; }; }
