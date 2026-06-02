cmake_minimum_required(VERSION 3.16)
project(DistributedNeuralXR LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

option(DNXR_BUILD_TESTS "Build DNXR tests" ON)
option(DNXR_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(DNXR_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(DNXR_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(DNXR_SIMULATION_ONLY "Force simulation-only runtime guardrails" ON)
option(DNXR_ENABLE_LONG_SIMULATED_TESTS "Enable long simulated tests" ON)
option(DNXR_ENABLE_LONG_REAL_TESTS "Enable long real soak tests" OFF)

if(DNXR_ENABLE_TSAN AND DNXR_ENABLE_ASAN)
    message(FATAL_ERROR "DNXR_ENABLE_TSAN and DNXR_ENABLE_ASAN cannot be enabled together")
endif()

# Runtime portability for Windows builds.
option(DNXR_WINDOWS_PORTABLE_RUNTIME "Statically link C/C++ runtime on Windows to avoid missing DLL entry points" ON)
if(WIN32 AND DNXR_WINDOWS_PORTABLE_RUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_link_options(-static-libgcc -static-libstdc++)
    endif()
endif()

file(GLOB_RECURSE DNXR_CORE_SRC CONFIGURE_DEPENDS src/*.cpp)
list(FILTER DNXR_CORE_SRC EXCLUDE REGEX "/main\\.cpp$")

function(dnxr_apply_sanitizers target)
    if(DNXR_ENABLE_ASAN)
        if(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
            target_link_options(${target} PRIVATE /fsanitize=address)
        else()
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address)
        endif()
    endif()

    if(DNXR_ENABLE_UBSAN)
        if(NOT MSVC)
            target_compile_options(${target} PRIVATE -fsanitize=undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=undefined)
        endif()
    endif()

    if(DNXR_ENABLE_TSAN)
        if(NOT MSVC)
            target_compile_options(${target} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=thread)
        endif()
    endif()
endfunction()

add_library(dnxr_core STATIC ${DNXR_CORE_SRC})
target_include_directories(dnxr_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(DNXR_SIMULATION_ONLY)
    target_compile_definitions(dnxr_core PUBLIC DNXR_SIMULATION_ONLY=1)
else()
    target_compile_definitions(dnxr_core PUBLIC DNXR_SIMULATION_ONLY=0)
endif()

dnxr_apply_sanitizers(dnxr_core)

# Cross-platform compile tags for Windows/Linux/macOS integration layers.
if(WIN32)
    target_compile_definitions(dnxr_core PUBLIC DNXR_TARGET_OS_WINDOWS=1)
elseif(APPLE)
    target_compile_definitions(dnxr_core PUBLIC DNXR_TARGET_OS_MACOS=1)
elseif(UNIX)
    target_compile_definitions(dnxr_core PUBLIC DNXR_TARGET_OS_LINUX=1)
endif()

add_executable(distributed_neural_xr src/main.cpp)
target_link_libraries(distributed_neural_xr PRIVATE dnxr_core)
dnxr_apply_sanitizers(distributed_neural_xr)

if(DNXR_BUILD_TESTS)
    enable_testing()

    function(dnxr_add_test test_name)
        add_executable(${test_name} tests/${test_name}.cpp)
        target_link_libraries(${test_name} PRIVATE dnxr_core)
        dnxr_apply_sanitizers(${test_name})
        add_test(NAME ${test_name} COMMAND ${test_name})
    endfunction()

    set(DNXR_TEST_TARGETS
        dnxr_state_smoke_test
        dnxr_config_loader_test
        dnxr_json_parser_test
        dnxr_calibration_drift_monitor_test
        dnxr_save_asset_manager_test
        dnxr_plugin_registry_test
        dnxr_signal_unit_test
        dnxr_file_replay_signal_provider_test
        dnxr_signal_dropout_test
        dnxr_cognition_unit_test
        dnxr_safety_unit_test
        dnxr_safety_state_machine_test
        dnxr_watchdog_failure_test
        dnxr_safety_profile_loader_test
        dnxr_simulation_only_guard_test
        dnxr_secret_provider_test
        dnxr_safety_pipeline_test
        dnxr_reality_anchor_system_test
        dnxr_multisensory_clock_test
        dnxr_intent_risk_classifier_test
        dnxr_neurorights_policy_test
        dnxr_safety_fuzz_test
        dnxr_renderer_qos_stress_test
        dnxr_pipeline_integration_test
        dnxr_neurotech_unit_test
        dnxr_neuro_sdk_bridge_test
        dnxr_runtime_realtime_test
        dnxr_runtime_10k_cycles_test
        dnxr_runtime_types_test
        dnxr_runtime_trace_recorder_test
        dnxr_logger_sink_test
        dnxr_replay_determinism_test
        dnxr_security_foundation_test
        dnxr_network_replay_attack_test
        dnxr_neural_chip_hal_test
        dnxr_vestibular_budget_boundary_test
        dnxr_sensory_frame_schema_test
        dnxr_vestibular_exposure_budget_test
        dnxr_full_dive_bridge_intervention_test
        dnxr_full_dive_intervention_window_test
        dnxr_stimulus_envelope_concurrency_test
        dnxr_vestibular_exposure_budget_concurrency_test
        dnxr_safety_state_machine_concurrency_test
        dnxr_open_interop_protocol_test
        dnxr_neural_intent_protocol_test
        dnxr_runtime_acquisition_stage_test
        dnxr_runtime_inference_stage_test
        dnxr_runtime_calibration_stage_test
        dnxr_runtime_immersion_frame_stage_test
        dnxr_runtime_cycle_finalizer_test
        dnxr_runtime_pipeline_runner_test
        dnxr_runtime_safety_stage_test
        dnxr_runtime_full_dive_stage_test
        dnxr_runtime_render_stage_test
        dnxr_runtime_stimulus_stage_test
        dnxr_runtime_memory_growth_test
        dnxr_runtime_pipeline_100k_cycles_test
        dnxr_runtime_pipeline_memory_growth_test
        dnxr_runtime_stage_jitter_simulation_test
        dnxr_runtime_1h_stability_test
        dnxr_vestibular_exposure_budget_long_test
        dnxr_full_dive_intervention_window_long_test
        dnxr_watchdog_stale_subsystem_long_test
        dnxr_full_dive_orchestrator_integration_test
        dnxr_neural_chip_immersion_integration_test
    )

    foreach(test_name IN LISTS DNXR_TEST_TARGETS)
        dnxr_add_test(${test_name})
    endforeach()

    set_tests_properties(dnxr_safety_state_machine_concurrency_test PROPERTIES LABELS "concurrency;safety")
    set_tests_properties(dnxr_vestibular_budget_boundary_test PROPERTIES LABELS "unit;safety")
    set_tests_properties(dnxr_sensory_frame_schema_test PROPERTIES LABELS "unit;safety")
    set_tests_properties(dnxr_vestibular_exposure_budget_test PROPERTIES LABELS "unit;safety")
    set_tests_properties(dnxr_full_dive_bridge_intervention_test PROPERTIES LABELS "unit;safety;full_dive")
    set_tests_properties(dnxr_full_dive_intervention_window_test PROPERTIES LABELS "unit;safety;full_dive")
    set_tests_properties(dnxr_stimulus_envelope_concurrency_test PROPERTIES LABELS "concurrency;safety")
    set_tests_properties(dnxr_vestibular_exposure_budget_concurrency_test PROPERTIES LABELS "concurrency;safety")
    set_tests_properties(dnxr_runtime_10k_cycles_test PROPERTIES LABELS "long_fast")
    set_tests_properties(dnxr_runtime_acquisition_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_inference_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_calibration_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_immersion_frame_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_cycle_finalizer_test PROPERTIES LABELS "unit;runtime")
    set_tests_properties(dnxr_runtime_pipeline_runner_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_safety_stage_test PROPERTIES LABELS "unit;runtime;stage;safety")
    set_tests_properties(dnxr_runtime_full_dive_stage_test PROPERTIES LABELS "unit;runtime;stage;full_dive")
    set_tests_properties(dnxr_runtime_render_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_stimulus_stage_test PROPERTIES LABELS "unit;runtime;stage;safety")
    set(DNXR_LONG_SIMULATED_TEST_TARGETS
        dnxr_runtime_memory_growth_test
        dnxr_runtime_pipeline_100k_cycles_test
        dnxr_runtime_pipeline_memory_growth_test
        dnxr_runtime_stage_jitter_simulation_test
        dnxr_runtime_1h_stability_test
        dnxr_watchdog_stale_subsystem_long_test
    )
    set(DNXR_LONG_REAL_TEST_TARGETS
        dnxr_vestibular_exposure_budget_long_test
        dnxr_full_dive_intervention_window_long_test
    )
    set_tests_properties(dnxr_runtime_memory_growth_test PROPERTIES LABELS "long_simulated;soak;memory")
    set_tests_properties(dnxr_runtime_pipeline_100k_cycles_test PROPERTIES LABELS "long_simulated;soak;runtime")
    set_tests_properties(dnxr_runtime_pipeline_memory_growth_test PROPERTIES LABELS "long_simulated;soak;memory;runtime")
    set_tests_properties(dnxr_runtime_stage_jitter_simulation_test PROPERTIES LABELS "long_simulated;runtime;stage")
    set_tests_properties(dnxr_runtime_1h_stability_test PROPERTIES LABELS "long_simulated;soak")
    set_tests_properties(dnxr_vestibular_exposure_budget_long_test PROPERTIES LABELS "long_real;soak;safety")
    set_tests_properties(dnxr_full_dive_intervention_window_long_test PROPERTIES LABELS "long_real;soak;full_dive;safety")
    set_tests_properties(dnxr_watchdog_stale_subsystem_long_test PROPERTIES LABELS "long_simulated;soak;safety")
    if(NOT DNXR_ENABLE_LONG_SIMULATED_TESTS)
        set_tests_properties(${DNXR_LONG_SIMULATED_TEST_TARGETS} PROPERTIES DISABLED TRUE)
    endif()
    if(NOT DNXR_ENABLE_LONG_REAL_TESTS)
        set_tests_properties(${DNXR_LONG_REAL_TEST_TARGETS} PROPERTIES DISABLED TRUE)
    endif()
endif()
