cmake_minimum_required(VERSION 3.16)
project(DistributedNeuralXR LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Runtime portability for Windows builds.
option(DNXR_WINDOWS_PORTABLE_RUNTIME "Statically link C/C++ runtime on Windows to avoid missing DLL entry points" ON)
if(WIN32 AND DNXR_WINDOWS_PORTABLE_RUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_link_options(-static-libgcc -static-libstdc++)
    endif()
endif()

file(GLOB_RECURSE DNXR_SRC CONFIGURE_DEPENDS src/*.cpp)
add_executable(distributed_neural_xr ${DNXR_SRC})

enable_testing()

add_executable(dnxr_state_smoke_test
    tests/dnxr_state_smoke_test.cpp
    src/immersion/VirtualWorldState.cpp
    src/immersion/GameDesignBridge.cpp
    src/utils/Logger.cpp
)
add_test(NAME dnxr_state_smoke_test COMMAND dnxr_state_smoke_test)

add_executable(dnxr_signal_unit_test
    tests/dnxr_signal_unit_test.cpp
    src/signal/SignalStreamProcessor.cpp
    src/signal/FeatureVectorizer.cpp
)
add_test(NAME dnxr_signal_unit_test COMMAND dnxr_signal_unit_test)

add_executable(dnxr_cognition_unit_test
    tests/dnxr_cognition_unit_test.cpp
    src/cognition/StateInferenceEngine.cpp
    src/cognition/IntentModel.cpp
)
add_test(NAME dnxr_cognition_unit_test COMMAND dnxr_cognition_unit_test)

add_executable(dnxr_safety_unit_test
    tests/dnxr_safety_unit_test.cpp
    src/safety/PhysiologicalGuard.cpp
    src/safety/StabilityMonitor.cpp
    src/utils/Logger.cpp
)
add_test(NAME dnxr_safety_unit_test COMMAND dnxr_safety_unit_test)

add_executable(dnxr_pipeline_integration_test
    tests/dnxr_pipeline_integration_test.cpp
    src/signal/SignalStreamProcessor.cpp
    src/signal/FeatureVectorizer.cpp
    src/cognition/StateInferenceEngine.cpp
    src/cognition/IntentModel.cpp
    src/calibration/NeuralCalibrationEngine.cpp
    src/calibration/AdaptationLayer.cpp
    src/immersion/ImmersionCore.cpp
    src/immersion/GameDesignBridge.cpp
    src/immersion/VirtualWorldState.cpp
    src/immersion/SensoryRenderer.cpp
    src/safety/PhysiologicalGuard.cpp
    src/safety/StabilityMonitor.cpp
    src/safety/EmergencyShutdown.cpp
    src/utils/Logger.cpp
)
add_test(NAME dnxr_pipeline_integration_test COMMAND dnxr_pipeline_integration_test)


add_executable(dnxr_neurotech_unit_test
    tests/dnxr_neurotech_unit_test.cpp
    src/neurotech/DeepImmersionPlatform.cpp
    src/network/NeuralIntentProtocol.cpp
    src/safety/NeuroConsentManager.cpp
    src/cognition/InducedVectorInfluenceEngine.cpp
    src/safety/AddictionRiskGuard.cpp
)
add_test(NAME dnxr_neurotech_unit_test COMMAND dnxr_neurotech_unit_test)


add_executable(dnxr_neuro_sdk_bridge_test
    tests/dnxr_neuro_sdk_bridge_test.cpp
    src/neurotech/NeuroSDKBridge.cpp
    src/neurotech/DeepImmersionPlatform.cpp
    src/network/NeuralIntentProtocol.cpp
    src/safety/NeuroConsentManager.cpp
    src/cognition/InducedVectorInfluenceEngine.cpp
    src/safety/AddictionRiskGuard.cpp
)
add_test(NAME dnxr_neuro_sdk_bridge_test COMMAND dnxr_neuro_sdk_bridge_test)


# Cross-platform compile tags for Windows/Linux/macOS integration layers.
if(WIN32)
    target_compile_definitions(distributed_neural_xr PRIVATE DNXR_TARGET_OS_WINDOWS=1)
elseif(APPLE)
    target_compile_definitions(distributed_neural_xr PRIVATE DNXR_TARGET_OS_MACOS=1)
elseif(UNIX)
    target_compile_definitions(distributed_neural_xr PRIVATE DNXR_TARGET_OS_LINUX=1)
endif()


add_executable(dnxr_runtime_realtime_test
    tests/dnxr_runtime_realtime_test.cpp
    src/core/NeuroRealtimeScheduler.cpp
    src/utils/DeterministicSessionReplay.cpp
)
add_test(NAME dnxr_runtime_realtime_test COMMAND dnxr_runtime_realtime_test)


add_executable(dnxr_security_foundation_test
    tests/dnxr_security_foundation_test.cpp
    src/network/NeuralCryptoTransport.cpp
    src/safety/NeuralIdentityVault.cpp
)
add_test(NAME dnxr_security_foundation_test COMMAND dnxr_security_foundation_test)


add_executable(dnxr_open_interop_protocol_test
    tests/dnxr_open_interop_protocol_test.cpp
    src/network/OpenNeuralInteropProtocol.cpp
)
add_test(NAME dnxr_open_interop_protocol_test COMMAND dnxr_open_interop_protocol_test)
