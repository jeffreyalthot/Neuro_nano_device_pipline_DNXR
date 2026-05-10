#include "QuestSystem.h"

#include <algorithm>

namespace dnxr {

std::vector<Quest> QuestSystem::seedStarterQuests() const {
    return {
        {"q_intro_sync", "Synchronisation initiale", "Calibrer le lien neural et parler au guide du hub.", {{"obj_calibrate", "Completer la calibration cognitive", 1, 0, false}, {"obj_talk_guide", "Parler au guide de la place centrale", 1, 0, false}}, {350, 85, {"starter_focus_rune", "nano_bandage"}}, false, false, false},
        {"q_field_relay", "Relais des champs", "Reparer des relais energetiques dans Nature Shard Fields.", {{"obj_relay_scan", "Scanner les relais corrompus", 3, 0, false}, {"obj_relay_patch", "Appliquer des patchs de stabilisation", 3, 0, false}}, {900, 230, {"relay_core_fragment", "field_explorer_boots"}}, false, false, false}
    };
}

void QuestSystem::acceptQuest(std::vector<Quest>& quests, const std::string& questId) const {
    for (auto& q : quests) {
        if (q.id == questId) {
            q.accepted = true;
            return;
        }
    }
}

void QuestSystem::advanceObjective(std::vector<Quest>& quests, const std::string& questId, const std::string& objectiveId, int delta) const {
    for (auto& q : quests) {
        if (!q.accepted || q.completed || q.id != questId) {
            continue;
        }

        for (auto& objective : q.objectives) {
            if (objective.id != objectiveId || objective.completed) {
                continue;
            }
            objective.progress = std::min(objective.targetCount, objective.progress + std::max(0, delta));
            objective.completed = objective.progress >= objective.targetCount;
        }

        q.completed = std::all_of(q.objectives.begin(), q.objectives.end(), [](const QuestObjective& o) { return o.completed; });
    }
}

void QuestSystem::claimRewards(std::vector<Quest>& quests, PlayerProgression& progression, const std::string& questId) const {
    for (auto& q : quests) {
        if (q.id != questId || !q.completed || q.rewarded) {
            continue;
        }

        progression.xp += q.reward.xp;
        progression.gold += q.reward.gold;
        progression.inventory.insert(progression.inventory.end(), q.reward.items.begin(), q.reward.items.end());
        applyLevelUp(progression);
        q.rewarded = true;
        return;
    }
}

int QuestSystem::levelXpThreshold(int level) {
    return std::max(100, level * level * 120);
}

void QuestSystem::applyLevelUp(PlayerProgression& progression) {
    while (progression.xp >= levelXpThreshold(progression.level)) {
        progression.xp -= levelXpThreshold(progression.level);
        progression.level += 1;
    }
}

} // namespace dnxr
