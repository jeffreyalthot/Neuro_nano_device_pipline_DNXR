#pragma once
#include <string>
namespace dnxr { struct BuildFingerprint { std::string buildHash; std::string sourceManifestHash; static BuildFingerprint current(); }; }
