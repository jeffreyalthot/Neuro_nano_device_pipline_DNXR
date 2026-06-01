#pragma once

#include <string>

namespace dnxr {

class SaveMigration {
public:
    static int detectVersion(const std::string& json);
    static std::string migrateToLatest(const std::string& json);
    static int latestVersion();
};

} // namespace dnxr
