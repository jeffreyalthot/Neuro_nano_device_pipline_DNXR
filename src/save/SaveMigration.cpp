#include "SaveMigration.h"

#include <regex>

namespace dnxr {
namespace {

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

} // namespace

int SaveMigration::detectVersion(const std::string& json) {
    int version = 0;
    if (findInt(json, "save_version", version)) {
        return version;
    }
    return 1;
}

std::string SaveMigration::migrateToLatest(const std::string& json) {
    if (detectVersion(json) >= latestVersion()) {
        return json;
    }

    std::string profileId;
    if (!findString(json, "profile_id", profileId)) {
        findString(json, "save_slot", profileId);
    }
    if (profileId.empty()) {
        profileId = "slot_001";
    }

    std::string characterId;
    if (!findString(json, "character_id", characterId)) {
        characterId = "unknown_character";
    }

    std::string mapId;
    if (!findString(json, "last_map_id", mapId)) {
        mapId = "unknown_map";
    }

    return "{\n"
           "  \"save_version\": 2,\n"
           "  \"profile_id\": \"" + escapeJsonString(profileId) + "\",\n"
           "  \"character_id\": \"" + escapeJsonString(characterId) + "\",\n"
           "  \"last_map_id\": \"" + escapeJsonString(mapId) + "\",\n"
           "  \"checksum\": \"pending\"\n"
           "}\n";
}

int SaveMigration::latestVersion() {
    return 2;
}

} // namespace dnxr
