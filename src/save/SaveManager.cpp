#include "SaveManager.h"

#include "SaveMigration.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>
#include <utility>

namespace dnxr {
namespace {

std::string readFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return {};
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool findString(const std::string& json, const std::string& key, std::string& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = match[1].str();
        return true;
    }
    return false;
}

bool findInt(const std::string& json, const std::string& key, int& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = std::stoi(match[1].str());
        return true;
    }
    return false;
}

std::string escapeJsonString(const std::string& value) {
    std::string escaped;
    for (char ch : value) {
        switch (ch) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += ch; break;
        }
    }
    return escaped;
}

std::string checksumPayload(const SaveProfile& profile) {
    std::ostringstream payload;
    payload << profile.saveVersion << '|'
            << profile.profileId << '|'
            << profile.characterId << '|'
            << profile.lastMapId;
    return payload.str();
}

std::uint64_t fnv1a64(const std::string& payload) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

SaveManager::SaveManager(std::string saveDirectory)
    : saveDirectory_(std::move(saveDirectory)) {}

bool SaveManager::saveProfile(const SaveProfile& profile, const std::string& slotId) const {
    std::filesystem::create_directories(saveDirectory_);
    std::ofstream output(pathForSlot(slotId), std::ios::trunc);
    if (!output) {
        return false;
    }

    SaveProfile copy = profile;
    copy.saveVersion = SaveMigration::latestVersion();
    copy.profileId = slotId;
    copy.checksum = computeChecksum(copy);
    output << serialize(copy);
    return static_cast<bool>(output);
}

bool SaveManager::loadProfile(const std::string& slotId, SaveProfile& profile) const {
    const std::string path = pathForSlot(slotId);
    std::string json = readFile(path);
    if (json.empty()) {
        return false;
    }

    json = SaveMigration::migrateToLatest(json);
    SaveProfile loaded;
    if (!parse(json, loaded)) {
        return false;
    }

    if (loaded.checksum == "pending") {
        loaded.checksum = computeChecksum(loaded);
    }

    if (loaded.checksum != computeChecksum(loaded)) {
        return false;
    }

    profile = loaded;
    return true;
}

bool SaveManager::validateFile(const std::string& path) const {
    std::string json = readFile(path);
    if (json.empty()) {
        return false;
    }

    json = SaveMigration::migrateToLatest(json);
    SaveProfile profile;
    if (!parse(json, profile)) {
        return false;
    }
    return profile.checksum == "pending" || profile.checksum == computeChecksum(profile);
}

std::vector<std::string> SaveManager::listSlots() const {
    std::vector<std::string> slots;
    if (!std::filesystem::exists(saveDirectory_)) {
        return slots;
    }

    for (const auto& entry : std::filesystem::directory_iterator(saveDirectory_)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto filename = entry.path().filename().generic_string();
        const std::string prefix = "player_profile_";
        const std::string suffix = ".json";
        if (filename.rfind(prefix, 0U) == 0U && filename.size() > prefix.size() + suffix.size()) {
            slots.push_back(filename.substr(prefix.size(), filename.size() - prefix.size() - suffix.size()));
        }
    }
    std::sort(slots.begin(), slots.end());
    return slots;
}

std::string SaveManager::pathForSlot(const std::string& slotId) const {
    return (std::filesystem::path(saveDirectory_) / ("player_profile_" + slotId + ".json")).generic_string();
}

std::string SaveManager::computeChecksum(const SaveProfile& profile) {
    std::ostringstream out;
    out << "save_" << std::hex << std::setw(16) << std::setfill('0') << fnv1a64(checksumPayload(profile));
    return out.str();
}

std::string SaveManager::serialize(const SaveProfile& profile) {
    SaveProfile copy = profile;
    if (copy.checksum.empty()) {
        copy.checksum = computeChecksum(copy);
    }

    std::ostringstream out;
    out << "{\n"
        << "  \"save_version\": " << copy.saveVersion << ",\n"
        << "  \"profile_id\": \"" << escapeJsonString(copy.profileId) << "\",\n"
        << "  \"character_id\": \"" << escapeJsonString(copy.characterId) << "\",\n"
        << "  \"last_map_id\": \"" << escapeJsonString(copy.lastMapId) << "\",\n"
        << "  \"checksum\": \"" << escapeJsonString(copy.checksum) << "\"\n"
        << "}\n";
    return out.str();
}

bool SaveManager::parse(const std::string& json, SaveProfile& profile) {
    SaveProfile parsed;
    if (!findInt(json, "save_version", parsed.saveVersion)) {
        return false;
    }
    if (!findString(json, "profile_id", parsed.profileId)) {
        return false;
    }
    if (!findString(json, "character_id", parsed.characterId)) {
        return false;
    }
    if (!findString(json, "last_map_id", parsed.lastMapId)) {
        return false;
    }
    if (!findString(json, "checksum", parsed.checksum)) {
        return false;
    }

    profile = parsed;
    return true;
}

} // namespace dnxr
