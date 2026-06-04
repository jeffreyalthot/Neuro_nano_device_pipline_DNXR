#pragma once

#include "readiness/EvidenceRepository.h"

#include <string>
#include <vector>

namespace dnxr {

enum class CountryReadinessStatus {
    Draft,
    Blocked,
    ReadyForQualifiedReview
};

struct CountryReadinessItem {
    std::string id;
    std::string description;
    bool complete{false};
};

class CountryReadinessProfile {
public:
    explicit CountryReadinessProfile(std::string country);
    virtual ~CountryReadinessProfile() = default;

    const std::string& country() const;
    void setItemComplete(const std::string& id, bool complete);
    std::vector<CountryReadinessItem> items() const;
    std::vector<CountryReadinessItem> missingItems() const;
    CountryReadinessStatus status() const;
    CountryReadinessStatus status(const EvidenceRepository& evidence, const std::string& artifactTypePrefix) const;
    bool blocksClinicalSubmission() const;
    bool blocksClinicalSubmission(const EvidenceRepository& evidence, const std::string& artifactTypePrefix) const;

protected:
    void addItem(std::string id, std::string description, bool complete = false);

private:
    std::string country_;
    std::vector<CountryReadinessItem> items_;
};

std::string toString(CountryReadinessStatus status);

} // namespace dnxr
