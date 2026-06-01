#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace dnxr {

class JsonValue {
public:
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };

    using Array = std::vector<JsonValue>;
    using Object = std::map<std::string, JsonValue>;

    JsonValue() = default;
    explicit JsonValue(bool value);
    explicit JsonValue(double value);
    explicit JsonValue(std::string value);
    explicit JsonValue(Array value);
    explicit JsonValue(Object value);

    Type type() const;
    bool isObject() const;
    bool isArray() const;
    bool has(const std::string& key) const;
    const JsonValue* find(const std::string& key) const;

    std::string getString(const std::string& key, const std::string& fallback) const;
    int getInt(const std::string& key, int fallback) const;
    std::uint64_t getUInt64(const std::string& key, std::uint64_t fallback) const;
    float getFloat(const std::string& key, float fallback) const;
    double getDouble(const std::string& key, double fallback) const;
    bool getBool(const std::string& key, bool fallback) const;
    std::vector<float> getFloatArray(const std::string& key, const std::vector<float>& fallback) const;

    const std::string& asString() const;
    double asNumber(double fallback = 0.0) const;
    bool asBool(bool fallback = false) const;
    const Array& asArray() const;
    const Object& asObject() const;

private:
    Type type_ = Type::Null;
    bool boolValue_ = false;
    double numberValue_ = 0.0;
    std::string stringValue_;
    Array arrayValue_;
    Object objectValue_;
};

} // namespace dnxr
