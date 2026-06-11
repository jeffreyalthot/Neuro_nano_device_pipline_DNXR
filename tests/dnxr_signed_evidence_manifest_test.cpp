#include "qms/SignedEvidenceManifest.h"
#include <cassert>
#include <fstream>

int main() {
    const char* path = "manifest_test_artifact.txt";
    {
        std::ofstream out(path);
        out << "abc";
    }

    dnxr::SignedEvidenceManifestBuilder builder;
    const auto digest = dnxr::SignedEvidenceManifestBuilder::sha256File(path);
    assert(digest == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    auto manifest = builder.build({{path, "test_artifact"}});
    assert(manifest.files.size() == 1U);
    assert(manifest.files[0].sha256 == digest);
    assert(manifest.toJson().find("0.6.5") != std::string::npos);
    return 0;
}
