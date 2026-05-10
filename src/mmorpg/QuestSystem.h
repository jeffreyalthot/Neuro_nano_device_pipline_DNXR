#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct QuestObjective {
    std::string id;
    std::string description;
    int targetCount{1};
    int progress{0};
    bool completed{false};
};

struct QuestReward {
    int xp{0};
    int gold{0};
    std::vector<std::string> items{};
};

struct Quest {
    std::string id;
    std::string title;
    std::string narrative;
    std::vector<QuestObjective> objectives;
    QuestReward reward;
    bool accepted{false};
    bool completed{false};
    bool rewarded{false};
};

struct PlayerProgression {
    int level{1};
    int gold{0};
    int xp{0};
    std::vector<std::string> inventory{};
};

class QuestSystem {
public:
    std::vector<Quest> seedStarterQuests() const;
    void acceptQuest(std::vector<Quest>& quests, const std::string& questId) const;
    void advanceObjective(std::vector<Quest>& quests, const std::string& questId, const std::string& objectiveId, int delta) const;
    void claimRewards(std::vector<Quest>& quests, PlayerProgression& progression, const std::string& questId) const;

private:
    static int levelXpThreshold(int level);
    static void applyLevelUp(PlayerProgression& progression);
};

} // namespace dnxr
