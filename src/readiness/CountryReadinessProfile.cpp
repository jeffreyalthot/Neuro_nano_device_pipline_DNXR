#include "readiness/CountryReadinessProfile.h"

#include <utility>

namespace dnxr {

CountryReadinessProfile::CountryReadinessProfile(std::string country) : country_(std::move(country)) {}
const std::string& CountryReadinessProfile::country() const { return country_; }

void CountryReadinessProfile::addItem(std::string id, std::string description, bool complete) {
    items_.push_back({std::move(id), std::move(description), complete});
}

void CountryReadinessProfile::setItemComplete(const std::string& id, bool complete) {
    for (auto& item : items_) {
        if (item.id == id) {
            item.complete = complete;
        }
    }
}

std::vector<CountryReadinessItem> CountryReadinessProfile::items() const { return items_; }

std::vector<CountryReadinessItem> CountryReadinessProfile::missingItems() const {
    std::vector<CountryReadinessItem> missing;
    for (const auto& item : items_) {
        if (!item.complete) {
            missing.push_back(item);
        }
    }
    return missing;
}

CountryReadinessStatus CountryReadinessProfile::status() const {
    if (items_.empty() || !missingItems().empty()) {
        return CountryReadinessStatus::Blocked;
    }
    return CountryReadinessStatus::ReadyForQualifiedReview;
}

bool CountryReadinessProfile::blocksClinicalSubmission() const { return status() != CountryReadinessStatus::ReadyForQualifiedReview; }

std::string toString(CountryReadinessStatus status) {
    switch (status) {
    case CountryReadinessStatus::Draft:
        return "Draft";
    case CountryReadinessStatus::Blocked:
        return "Blocked";
    case CountryReadinessStatus::ReadyForQualifiedReview:
        return "ReadyForQualifiedReview";
    }
    return "Blocked";
}

} // namespace dnxr
