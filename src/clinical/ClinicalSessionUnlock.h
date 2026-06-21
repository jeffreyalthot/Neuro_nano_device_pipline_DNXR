#pragma once
#include <cstdint>
#include <string>
namespace dnxr { struct ClinicalSessionUnlock { std::string sessionId; std::string protocolId; std::string siteId; std::string subjectId; std::int64_t expiresAtUnixSeconds = 0; bool valid = false; }; }
