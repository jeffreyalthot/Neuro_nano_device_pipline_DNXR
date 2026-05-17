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
