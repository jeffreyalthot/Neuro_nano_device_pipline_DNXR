#pragma once

#include <map>
#include <string>
#include <vector>

namespace dnxr {

class RegulatoryEvidenceIndex {
public:
    void addArtifact(const std::string& category, const std::string& artifact);
    bool hasArtifact(const std::string& category, const std::string& artifact) const;
    std::vector<std::string> artifactsFor(const std::string& category) const;
    std::size_t artifactCount() const;
private:
    std::map<std::string, std::vector<std::string>> artifacts_;
};

} // namespace dnxr
