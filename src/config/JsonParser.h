#pragma once

#include "JsonValue.h"

#include <string>

namespace dnxr {

class JsonParser {
public:
    static JsonValue parse(const std::string& json);
    static bool tryParse(const std::string& json, JsonValue& out, std::string* error = nullptr);
};

} // namespace dnxr
