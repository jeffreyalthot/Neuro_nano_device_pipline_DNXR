#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct SaveProfile {
    int saveVersion = 2;
    std::string profileId = "slot_001";
    std::string characterId = "base_sphere_skin";
    std::string lastMapId = "default_blender_map";
    std::string checksum;
};

class SaveManager {
public:
    explicit SaveManager(std::string saveDirectory = "assets/save");

    bool saveProfile(const SaveProfile& profile, const std::string& slotId) const;
    bool loadProfile(const std::string& slotId, SaveProfile& profile) const;
    bool validateFile(const std::string& path) const;
    std::vector<std::string> listSlots() const;
    std::string pathForSlot(const std::string& slotId) const;

    static std::string computeChecksum(const SaveProfile& profile);
    static std::string serialize(const SaveProfile& profile);
    static bool parse(const std::string& json, SaveProfile& profile);

private:
    std::string saveDirectory_;
};

} // namespace dnxr
