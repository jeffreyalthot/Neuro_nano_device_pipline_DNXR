#include "JsonParser.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <utility>

namespace dnxr {

JsonValue::JsonValue(bool value) : type_(Type::Bool), boolValue_(value) {}
JsonValue::JsonValue(double value) : type_(Type::Number), numberValue_(value) {}
JsonValue::JsonValue(std::string value) : type_(Type::String), stringValue_(std::move(value)) {}
JsonValue::JsonValue(Array value) : type_(Type::Array), arrayValue_(std::move(value)) {}
JsonValue::JsonValue(Object value) : type_(Type::Object), objectValue_(std::move(value)) {}

JsonValue::Type JsonValue::type() const { return type_; }
bool JsonValue::isObject() const { return type_ == Type::Object; }
bool JsonValue::isArray() const { return type_ == Type::Array; }

bool JsonValue::has(const std::string& key) const { return find(key) != nullptr; }

const JsonValue* JsonValue::find(const std::string& key) const {
    if (!isObject()) {
        return nullptr;
    }
    const auto it = objectValue_.find(key);
    return it == objectValue_.end() ? nullptr : &it->second;
}

std::string JsonValue::getString(const std::string& key, const std::string& fallback) const {
    const JsonValue* value = find(key);
    return value != nullptr && value->type_ == Type::String ? value->stringValue_ : fallback;
}

int JsonValue::getInt(const std::string& key, int fallback) const {
    const JsonValue* value = find(key);
    if (value == nullptr || value->type_ != Type::Number || !std::isfinite(value->numberValue_)) {
        return fallback;
    }
    return static_cast<int>(value->numberValue_);
}

std::uint64_t JsonValue::getUInt64(const std::string& key, std::uint64_t fallback) const {
    const JsonValue* value = find(key);
    if (value == nullptr || value->type_ != Type::Number || !std::isfinite(value->numberValue_) || value->numberValue_ < 0.0) {
        return fallback;
    }
    const auto max = static_cast<double>(std::numeric_limits<std::uint64_t>::max());
    return static_cast<std::uint64_t>(std::min(value->numberValue_, max));
}

float JsonValue::getFloat(const std::string& key, float fallback) const {
    return static_cast<float>(getDouble(key, fallback));
}

double JsonValue::getDouble(const std::string& key, double fallback) const {
    const JsonValue* value = find(key);
    return value != nullptr && value->type_ == Type::Number && std::isfinite(value->numberValue_) ? value->numberValue_ : fallback;
}

bool JsonValue::getBool(const std::string& key, bool fallback) const {
    const JsonValue* value = find(key);
    return value != nullptr && value->type_ == Type::Bool ? value->boolValue_ : fallback;
}

std::vector<float> JsonValue::getFloatArray(const std::string& key, const std::vector<float>& fallback) const {
    const JsonValue* value = find(key);
    if (value == nullptr || value->type_ != Type::Array) {
        return fallback;
    }
    std::vector<float> out;
    out.reserve(value->arrayValue_.size());
    for (const JsonValue& item : value->arrayValue_) {
        if (item.type_ != Type::Number || !std::isfinite(item.numberValue_)) {
            return fallback;
        }
        out.push_back(static_cast<float>(item.numberValue_));
    }
    return out.empty() ? fallback : out;
}

const std::string& JsonValue::asString() const { return stringValue_; }
double JsonValue::asNumber(double fallback) const { return type_ == Type::Number ? numberValue_ : fallback; }
bool JsonValue::asBool(bool fallback) const { return type_ == Type::Bool ? boolValue_ : fallback; }
const JsonValue::Array& JsonValue::asArray() const { return arrayValue_; }
const JsonValue::Object& JsonValue::asObject() const { return objectValue_; }

namespace {

class Parser {
public:
    explicit Parser(const std::string& json) : json_(json) {}

    JsonValue parseDocument() {
        skipWhitespace();
        JsonValue value = parseValue();
        skipWhitespace();
        if (!eof()) {
            fail("unexpected trailing data");
        }
        return value;
    }

private:
    bool eof() const { return pos_ >= json_.size(); }
    char peek() const { return eof() ? '\0' : json_[pos_]; }
    char take() { return eof() ? '\0' : json_[pos_++]; }

    void fail(const std::string& message) const {
        throw std::runtime_error("json parse error at byte " + std::to_string(pos_) + ": " + message);
    }

    void skipWhitespace() {
        while (!eof() && std::isspace(static_cast<unsigned char>(peek()))) {
            ++pos_;
        }
    }

    bool consume(char expected) {
        skipWhitespace();
        if (peek() != expected) {
            return false;
        }
        ++pos_;
        return true;
    }

    void expectLiteral(const std::string& literal) {
        if (json_.compare(pos_, literal.size(), literal) != 0) {
            fail("expected " + literal);
        }
        pos_ += literal.size();
    }

    JsonValue parseValue() {
        skipWhitespace();
        switch (peek()) {
            case '{': return parseObject();
            case '[': return parseArray();
            case '"': return JsonValue(parseString());
            case 't': expectLiteral("true"); return JsonValue(true);
            case 'f': expectLiteral("false"); return JsonValue(false);
            case 'n': expectLiteral("null"); return JsonValue();
            default:
                if (peek() == '-' || std::isdigit(static_cast<unsigned char>(peek()))) {
                    return JsonValue(parseNumber());
                }
                fail("expected value");
        }
        return JsonValue();
    }

    JsonValue parseObject() {
        consume('{');
        JsonValue::Object object;
        skipWhitespace();
        if (consume('}')) {
            return JsonValue(std::move(object));
        }
        while (true) {
            skipWhitespace();
            if (peek() != '"') {
                fail("expected object key");
            }
            std::string key = parseString();
            if (!consume(':')) {
                fail("expected ':' after object key");
            }
            object[std::move(key)] = parseValue();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                fail("expected ',' between object entries");
            }
        }
        return JsonValue(std::move(object));
    }

    JsonValue parseArray() {
        consume('[');
        JsonValue::Array array;
        skipWhitespace();
        if (consume(']')) {
            return JsonValue(std::move(array));
        }
        while (true) {
            array.push_back(parseValue());
            if (consume(']')) {
                break;
            }
            if (!consume(',')) {
                fail("expected ',' between array entries");
            }
        }
        return JsonValue(std::move(array));
    }

    std::string parseString() {
        if (take() != '"') {
            fail("expected string");
        }
        std::string out;
        while (!eof()) {
            const char ch = take();
            if (ch == '"') {
                return out;
            }
            if (ch != '\\') {
                out.push_back(ch);
                continue;
            }
            if (eof()) {
                fail("unterminated escape");
            }
            const char escaped = take();
            switch (escaped) {
                case '"': out.push_back('"'); break;
                case '\\': out.push_back('\\'); break;
                case '/': out.push_back('/'); break;
                case 'b': out.push_back('\b'); break;
                case 'f': out.push_back('\f'); break;
                case 'n': out.push_back('\n'); break;
                case 'r': out.push_back('\r'); break;
                case 't': out.push_back('\t'); break;
                case 'u':
                    for (int i = 0; i < 4; ++i) {
                        if (eof() || !std::isxdigit(static_cast<unsigned char>(take()))) {
                            fail("invalid unicode escape");
                        }
                    }
                    out.push_back('?');
                    break;
                default:
                    fail("invalid escape sequence");
            }
        }
        fail("unterminated string");
        return out;
    }

    double parseNumber() {
        const std::size_t start = pos_;
        if (peek() == '-') {
            ++pos_;
        }
        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            ++pos_;
        }
        if (peek() == '.') {
            ++pos_;
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++pos_;
            }
        }
        if (peek() == 'e' || peek() == 'E') {
            ++pos_;
            if (peek() == '+' || peek() == '-') {
                ++pos_;
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++pos_;
            }
        }
        char* end = nullptr;
        const std::string token = json_.substr(start, pos_ - start);
        const double value = std::strtod(token.c_str(), &end);
        if (end == token.c_str() || *end != '\0') {
            fail("invalid number");
        }
        return value;
    }

private:
    const std::string& json_;
    std::size_t pos_ = 0;
};

} // namespace

JsonValue JsonParser::parse(const std::string& json) {
    return Parser(json).parseDocument();
}

bool JsonParser::tryParse(const std::string& json, JsonValue& out, std::string* error) {
    try {
        out = parse(json);
        if (error) {
            error->clear();
        }
        return true;
    } catch (const std::exception& ex) {
        if (error) {
            *error = ex.what();
        }
        out = JsonValue{};
        return false;
    }
}

} // namespace dnxr
