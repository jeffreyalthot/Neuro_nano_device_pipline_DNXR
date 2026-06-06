#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr { struct ClinicalLogEntry { std::uint64_t sequence=0; std::string utcTimestamp; std::string eventType; std::string payloadHash; std::string previousHash; std::string entryHash; }; class TamperEvidentClinicalLog { public: ClinicalLogEntry append(const std::string& eventType, const std::string& payloadHash, const std::string& utcTimestamp); bool verify() const; const std::vector<ClinicalLogEntry>& entries() const; private: std::vector<ClinicalLogEntry> entries_; }; }
