#include "qms/ReviewRecord.h"

namespace dnxr::qms {

bool ReviewRecord::isComplete() const {
    return !id.empty() && !artifactId.empty() && !reviewer.empty() && !role.empty() && !date.empty() && accepted;
}

} // namespace dnxr::qms
