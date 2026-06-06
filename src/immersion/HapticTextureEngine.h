#pragma once
namespace dnxr { struct HapticSurfaceInput { float roughness=0,pressure=0,vibration=0,slip=0,impact=0,contactDurationMs=0; }; struct HapticTextureLayer { float pressure=0,vibration=0,texture=0; }; class HapticTextureEngine { public: HapticTextureLayer synthesize(const HapticSurfaceInput& i) const; }; }
