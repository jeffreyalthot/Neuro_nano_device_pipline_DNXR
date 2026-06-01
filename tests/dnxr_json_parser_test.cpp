#include "../src/config/JsonParser.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    const std::string json = R"json({
        "name": "dnxr",
        "version": 26,
        "enabled": true,
        "gain": 0.125,
        "values": [0.1, -0.2, 3.5e-1],
        "nested": {"ignored": false}
    })json";

    dnxr::JsonValue root;
    std::string error;
    assert(dnxr::JsonParser::tryParse(json, root, &error));
    assert(root.isObject());
    assert(root.has("nested"));
    assert(root.getString("name", "") == "dnxr");
    assert(root.getInt("version", 0) == 26);
    assert(root.getBool("enabled", false));
    assert(root.getFloat("gain", 0.0f) == 0.125f);

    const std::vector<float> values = root.getFloatArray("values", {});
    assert(values.size() == 3U);
    assert(values[1] == -0.2f);

    assert(!dnxr::JsonParser::tryParse("{bad json", root, &error));
    assert(!error.empty());
    return 0;
}
