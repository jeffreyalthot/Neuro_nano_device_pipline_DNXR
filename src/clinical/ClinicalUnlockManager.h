#pragma once

#include "ClinicalUnlockCapabilities.h"

namespace dnxr {

class ClinicalUnlockManager {
public:
    ClinicalUnlockCapabilities evaluate(const ClinicalUnlockContext& context) const;
};

} // namespace dnxr
