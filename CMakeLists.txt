cmake_minimum_required(VERSION 3.16)
project(DistributedNeuralXR LANGUAGES CXX)

find_package(OpenSSL REQUIRED COMPONENTS Crypto)
set(CMAKE_CXX_STANDARD 17)

option(DNXR_BUILD_TESTS "Build DNXR tests" ON)
option(DNXR_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(DNXR_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(DNXR_ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(DNXR_SIMULATION_ONLY "Force simulation-only runtime guardrails" ON)
option(DNXR_ENABLE_SIMULATION "Enable simulation adapters for tests/tools only; does not change the canonical business runtime graph" ON)
option(DNXR_ENABLE_HARDWARE_BENCH "Enable real hardware bench runtime without human use" OFF)
option(DNXR_ENABLE_PRECLINICAL_PHANTOM "Enable preclinical phantom mode" OFF)
option(DNXR_ENABLE_HUMAN_CLINICAL_TRIAL "Enable authorized human clinical trial mode" OFF)
option(DNXR_HUMAN_USE_BUILD "Build clinical human-use runtime; requires a local signed evidence gate file" OFF)
option(DNXR_CLINICAL_TEST_BUILD "Enable controlled clinical investigation unlock path" OFF)
option(DNXR_ENABLE_LONG_SIMULATED_TESTS "Enable long simulated tests" ON)
option(DNXR_ENABLE_LONG_REAL_TESTS "Enable long real soak tests" OFF)
option(DNXR_ENABLE_CONTROLLER_10K_TEST "Enable 10k deterministic controller pipeline test" OFF)
option(DNXR_ENABLE_CONTROLLER_100K_TEST "Enable 100k deterministic controller readiness test" OFF)
option(DNXR_ENABLE_EXTENDED_VV_TESTS "Enable extended clinical-readiness verification tests" ON)
option(DNXR_ENABLE_NO_ALLOCATION_GUARD "Enable no-allocation guard readiness tests" ON)
option(DNXR_ENABLE_CLINICAL_READINESS_VALIDATION "Enable all clinical-readiness validation lanes, including long simulated/real and controller soak tests" OFF)
option(DNXR_ENABLE_RESEARCH_SPECULATION "Build speculative research-only Full Dive modules" OFF)

if(DNXR_ENABLE_CLINICAL_READINESS_VALIDATION)
    set(DNXR_ENABLE_LONG_SIMULATED_TESTS ON CACHE BOOL "Enable long simulated tests" FORCE)
    set(DNXR_ENABLE_LONG_REAL_TESTS ON CACHE BOOL "Enable long real soak tests" FORCE)
    set(DNXR_ENABLE_CONTROLLER_10K_TEST ON CACHE BOOL "Enable 10k deterministic controller pipeline test" FORCE)
    set(DNXR_ENABLE_CONTROLLER_100K_TEST ON CACHE BOOL "Enable 100k deterministic controller readiness test" FORCE)
    set(DNXR_ENABLE_EXTENDED_VV_TESTS ON CACHE BOOL "Enable extended clinical-readiness verification tests" FORCE)
    set(DNXR_ENABLE_NO_ALLOCATION_GUARD ON CACHE BOOL "Enable no-allocation guard readiness tests" FORCE)
endif()

if(DNXR_ENABLE_HARDWARE_BENCH AND DNXR_SIMULATION_ONLY)
    message(FATAL_ERROR "DNXR_ENABLE_HARDWARE_BENCH requires DNXR_SIMULATION_ONLY=OFF")
endif()

if(DNXR_ENABLE_HUMAN_CLINICAL_TRIAL AND NOT DNXR_HUMAN_USE_BUILD)
    message(FATAL_ERROR "DNXR_ENABLE_HUMAN_CLINICAL_TRIAL requires DNXR_HUMAN_USE_BUILD=ON and signed evidence gate")
endif()

if(DNXR_ENABLE_HUMAN_CLINICAL_TRIAL AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/clinical_evidence/HUMAN_USE_BUILD_GATE.signed.json")
    message(FATAL_ERROR "Human clinical trial build requires signed HUMAN_USE_BUILD_GATE.signed.json")
endif()

if(DNXR_HUMAN_USE_BUILD AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/clinical_evidence/HUMAN_USE_BUILD_GATE.signed.json")
    message(FATAL_ERROR "DNXR_HUMAN_USE_BUILD requires clinical_evidence/HUMAN_USE_BUILD_GATE.signed.json and cannot be enabled by CMake flag alone")
endif()

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

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dnxr_sources.cmake)

option(DNXR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors for DNXR targets" ON)
option(DNXR_ENABLE_STATIC_ANALYSIS "Run clang-tidy when available" OFF)
option(DNXR_ENABLE_IWYU "Run include-what-you-use when available" OFF)

function(dnxr_configure_target target)
    target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(${target} PUBLIC OpenSSL::Crypto)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(DNXR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
        target_compile_definitions(${target} PUBLIC _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wswitch-enum)
        if(DNXR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
    if(DNXR_ENABLE_STATIC_ANALYSIS)
        find_program(DNXR_CLANG_TIDY_EXE NAMES clang-tidy)
        if(DNXR_CLANG_TIDY_EXE)
            set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${DNXR_CLANG_TIDY_EXE}")
        else()
            message(WARNING "DNXR_ENABLE_STATIC_ANALYSIS=ON but clang-tidy was not found")
        endif()
    endif()
    if(DNXR_ENABLE_IWYU)
        find_program(DNXR_IWYU_EXE NAMES include-what-you-use iwyu)
        if(DNXR_IWYU_EXE)
            set_target_properties(${target} PROPERTIES CXX_INCLUDE_WHAT_YOU_USE "${DNXR_IWYU_EXE}")
        else()
            message(WARNING "DNXR_ENABLE_IWYU=ON but include-what-you-use was not found")
        endif()
    endif()
endfunction()

function(dnxr_apply_sanitizers target)
    if(DNXR_ENABLE_ASAN)
        if(MSVC)
            target_compile_options(${target} PRIVATE /fsanitize=address)
            target_link_options(${target} PRIVATE /fsanitize=address)
        else()
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address)
            target_compile_definitions(${target} PRIVATE DNXR_ASAN_ENABLED=1)
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
            target_compile_definitions(${target} PRIVATE DNXR_TSAN_ENABLED=1)
        endif()
    endif()
endfunction()

add_library(dnxr_runtime_core STATIC ${DNXR_RUNTIME_CORE_SRC})
if(CMAKE_UNITY_BUILD)
    set_target_properties(dnxr_runtime_core PROPERTIES UNITY_BUILD_BATCH_SIZE 1)
endif()
dnxr_configure_target(dnxr_runtime_core)
target_compile_definitions(dnxr_runtime_core PUBLIC
    DNXR_HUMAN_USE_BUILD=$<BOOL:${DNXR_HUMAN_USE_BUILD}>
    DNXR_CLINICAL_TEST_BUILD=$<BOOL:${DNXR_CLINICAL_TEST_BUILD}>
    DNXR_ENABLE_RESEARCH_SPECULATION=$<BOOL:${DNXR_ENABLE_RESEARCH_SPECULATION}>
    DNXR_SIMULATION_ONLY=$<BOOL:${DNXR_SIMULATION_ONLY}>
    DNXR_ENABLE_HARDWARE_BENCH=$<BOOL:${DNXR_ENABLE_HARDWARE_BENCH}>
    DNXR_ENABLE_PRECLINICAL_PHANTOM=$<BOOL:${DNXR_ENABLE_PRECLINICAL_PHANTOM}>
    DNXR_ENABLE_HUMAN_CLINICAL_TRIAL=$<BOOL:${DNXR_ENABLE_HUMAN_CLINICAL_TRIAL}>
)
dnxr_apply_sanitizers(dnxr_runtime_core)

add_library(dnxr_real_adapters STATIC ${DNXR_REAL_ADAPTER_SRC})
dnxr_configure_target(dnxr_real_adapters)
target_link_libraries(dnxr_real_adapters PUBLIC dnxr_runtime_core)
dnxr_apply_sanitizers(dnxr_real_adapters)

add_library(dnxr_observer STATIC ${DNXR_OBSERVER_SRC})
dnxr_configure_target(dnxr_observer)
target_link_libraries(dnxr_observer PUBLIC dnxr_runtime_core)
dnxr_apply_sanitizers(dnxr_observer)

add_library(dnxr_test_adapters STATIC ${DNXR_TEST_ADAPTER_SRC})
dnxr_configure_target(dnxr_test_adapters)
target_link_libraries(dnxr_test_adapters PUBLIC dnxr_runtime_core)
dnxr_apply_sanitizers(dnxr_test_adapters)

add_library(dnxr_hil STATIC ${DNXR_HIL_SRC})
dnxr_configure_target(dnxr_hil)
target_link_libraries(dnxr_hil PUBLIC dnxr_runtime_core dnxr_real_adapters dnxr_test_adapters)
dnxr_apply_sanitizers(dnxr_hil)

add_library(dnxr_tools STATIC ${DNXR_TOOL_SRC})
dnxr_configure_target(dnxr_tools)
target_link_libraries(dnxr_tools PUBLIC dnxr_runtime_core)
dnxr_apply_sanitizers(dnxr_tools)

if(DNXR_ENABLE_RESEARCH_SPECULATION AND DNXR_RESEARCH_SPECULATION_SRC)
    add_library(dnxr_research_speculation STATIC ${DNXR_RESEARCH_SPECULATION_SRC})
    dnxr_configure_target(dnxr_research_speculation)
    target_link_libraries(dnxr_research_speculation PUBLIC dnxr_runtime_core)
endif()

# Observer quarantine source markers: devices/XRHeadsetInterface and rendering/XRRenderer remain observer/test-only, not production acquisition.
# DNXR_OBSERVER_RUNTIME_REGEX documents observer-only legacy display source quarantine.
# Compatibility aggregate for tests and legacy tools only. The production executable
# below deliberately links the canonical runtime core plus selected real adapters,
# not this broad aggregate.
add_library(dnxr_core INTERFACE)
target_link_libraries(dnxr_core INTERFACE "$<LINK_GROUP:RESCAN,dnxr_runtime_core,dnxr_real_adapters,dnxr_observer,dnxr_test_adapters,dnxr_hil,dnxr_tools>")
if(DNXR_ENABLE_RESEARCH_SPECULATION)
    target_link_libraries(dnxr_core INTERFACE dnxr_research_speculation)
endif()

# Cross-platform compile tags for Windows/Linux/macOS integration layers.
if(WIN32)
    target_compile_definitions(dnxr_runtime_core PUBLIC DNXR_TARGET_OS_WINDOWS=1)
elseif(APPLE)
    target_compile_definitions(dnxr_runtime_core PUBLIC DNXR_TARGET_OS_MACOS=1)
elseif(UNIX)
    target_compile_definitions(dnxr_runtime_core PUBLIC DNXR_TARGET_OS_LINUX=1)
endif()

add_executable(distributed_neural_xr src/main.cpp)
if(DNXR_SIMULATION_ONLY)
    target_link_libraries(distributed_neural_xr PRIVATE dnxr_runtime_core dnxr_real_adapters dnxr_test_adapters dnxr_observer)
else()
    target_link_libraries(distributed_neural_xr PRIVATE dnxr_runtime_core dnxr_real_adapters dnxr_observer)
endif()
dnxr_apply_sanitizers(distributed_neural_xr)

if(DNXR_ENABLE_HARDWARE_BENCH)
    add_executable(dnxr_hardware_bench src/hardware_bench/main.cpp src/hardware_bench/HardwareBenchRuntime.cpp src/hardware_bench/HardwareBenchSession.cpp src/hardware_bench/HardwareBenchSelfTest.cpp src/hardware_bench/HardwareBenchEmergencyStop.cpp src/hardware_bench/HardwareBenchTelemetry.cpp src/bench/DummyLoadAdapter.cpp src/bench/SignalSinkProbe.cpp src/bench/NeuralChipPhantomInterface.cpp src/real/RealReadinessGate.cpp src/real/RealModePolicy.cpp src/real/HardwareBenchAuthorization.cpp src/core/RuntimeDeploymentMode.cpp src/hardware/RealNeuralChipHAL.cpp src/hardware/NeuralChipHAL.cpp)
    target_include_directories(dnxr_hardware_bench PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(dnxr_hardware_bench PRIVATE dnxr_core)
    dnxr_apply_sanitizers(dnxr_hardware_bench)
endif()

if(DNXR_BUILD_TESTS)
    enable_testing()

    function(dnxr_add_test test_name)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_name}.cpp")
            set(test_source "tests/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/full_dive/${test_name}.cpp")
            set(test_source "tests/full_dive/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/hardware/${test_name}.cpp")
            set(test_source "tests/hardware/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime/${test_name}.cpp")
            set(test_source "tests/runtime/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/performance/${test_name}.cpp")
            set(test_source "tests/performance/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/qms/${test_name}.cpp")
            set(test_source "tests/qms/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/replay/${test_name}.cpp")
            set(test_source "tests/replay/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/assets/${test_name}.cpp")
            set(test_source "tests/assets/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/safety/${test_name}.cpp")
            set(test_source "tests/safety/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/real/${test_name}.cpp")
            set(test_source "tests/real/${test_name}.cpp")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/hardware_bench/${test_name}.cpp")
            set(test_source "tests/hardware_bench/${test_name}.cpp")
        else()
            message(FATAL_ERROR "Missing test source for ${test_name}")
        endif()
        add_executable(${test_name} ${test_source})
        if(test_name STREQUAL "dnxr_runtime_realtime_60hz_telemetry_test" OR test_name STREQUAL "dnxr_session_bootstrap_validator_test")
            target_link_libraries(${test_name} PRIVATE dnxr_runtime_core)
        else()
            target_link_libraries(${test_name} PRIVATE dnxr_core)
        endif()
        dnxr_apply_sanitizers(${test_name})
        add_test(NAME ${test_name} COMMAND ${test_name})
        set_tests_properties(${test_name} PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endfunction()

    set(DNXR_TEST_TARGETS
        dnxr_vector_to_spike_normalizer_test
        dnxr_vector_temporal_alignment_test
        dnxr_patient_target_binding_test
        dnxr_cross_modal_coherence_binder_test
        dnxr_engine_spike_core_test
        dnxr_spike_to_stimulus_translator_test
        dnxr_stimulus_by_spike_runtime_test
        dnxr_ppp_grammar_serialization_test
        dnxr_runtime_path_lock_test
        dnxr_canonical_runtime_architecture_test
        dnxr_canonical_runtime_single_owner_test
        dnxr_canonical_runtime_headset_independence_test
        dnxr_canonical_runtime_architecture_guard_test
        dnxr_canonical_stage_graph_transaction_test
        dnxr_bci_acquisition_source_validation_test
        dnxr_authenticated_virtual_world_ingress_test
        dnxr_runtime_motor_intent_stage_test
        dnxr_canonical_transaction_contract_test
        dnxr_session_bootstrap_validator_test
        dnxr_spike_runtime_real_evidence_gate_test
        dnxr_openssl_packet_protector_test
        dnxr_virtual_world_data_ingress_test
        dnxr_canonical_virtual_world_ingress_test
        dnxr_cli_side_effect_contract_test
        dnxr_full_dive_environment_sync_graph_test
        dnxr_spike_bridge_replay_coverage_test
        runtime_mode_test
        full_unlocked_runtime_test
        pipeline_state_machine_test
        replay_consistency_test
        performance_smoke_test
        safety_decision_test
        timebase_sync_test
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
        dnxr_integrated_safety_lifecycle_decision_test
        dnxr_canonical_closed_loop_motor_world_feedback_test
        dnxr_runtime_disposition_manifest_audit_test
        dnxr_runtime_single_pipeline_structure_test
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
        dnxr_runtime_callback_ref_test
        dnxr_runtime_pipeline_runner_test
        dnxr_runtime_pipeline_services_only_test
        dnxr_runtime_pipeline_sink_refs_test
        dnxr_runtime_pipeline_run_input_ref_test
        dnxr_runtime_pipeline_legacy_input_test
        dnxr_runtime_controller_pipeline_smoke_test
        dnxr_runtime_controller_no_allocation_smoke_test
        dnxr_runtime_controller_pipeline_10k_cycles_test
        dnxr_runtime_controller_pipeline_memory_growth_test
        dnxr_runtime_controller_jitter_budget_test
        dnxr_runtime_safety_stage_test
        dnxr_runtime_full_dive_stage_test
        dnxr_runtime_render_stage_test
        dnxr_runtime_stimulus_stage_test
        dnxr_runtime_allocation_guard_test
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

        dnxr_clinical_readiness_assessor_test
        dnxr_preclinical_readiness_assessor_test
        dnxr_readiness_cannot_skip_hil_test
        dnxr_readiness_requires_traceability_test
        dnxr_readiness_evidence_repository_test
        dnxr_senior_medical_scorecard_test
        dnxr_clinical_readiness_blocks_unsigned_evidence_test
        dnxr_regulatory_claim_guard_test
        dnxr_clinical_test_acceptance_review_gate_test
        dnxr_preclinical_readiness_requires_hil_reports_test
        dnxr_country_readiness_requires_signed_artifacts_test
        dnxr_qms_review_record_test
        dnxr_qms_signoff_record_test
        dnxr_qms_review_gate_blocks_unsigned_high_risk_test
        dnxr_full_dive_capability_matrix_test
        dnxr_full_dive_readiness_assessor_test
        dnxr_full_dive_session_validator_test
        dnxr_hil_fault_injection_test
        dnxr_hil_latency_budget_test
        dnxr_hil_emergency_stop_test
        dnxr_hil_phantom_disconnect_test
        dnxr_hil_recovery_sequence_test
        dnxr_hil_readiness_assessor_test
        dnxr_hil_scenario_runner_test
        dnxr_hil_session_report_test
        dnxr_hil_risk_coverage_matrix_test
        dnxr_hil_multi_fault_recovery_test
        dnxr_hil_latency_p99_test
        dnxr_canada_readiness_profile_test
        dnxr_us_readiness_profile_test
        dnxr_country_readiness_blocks_without_protocol_test
        dnxr_security_audit_trail_test
        dnxr_threat_event_test
        dnxr_replay_attack_blocked_test
        dnxr_packet_tamper_detection_test
        dnxr_secure_update_policy_test
        dnxr_runtime_controller_100k_cycles_test
        dnxr_runtime_no_allocation_zone_test
        dnxr_runtime_latency_profiler_test
        dnxr_runtime_realtime_60hz_telemetry_test
        dnxr_runtime_jitter_analyzer_test
        dnxr_runtime_wcet_recorder_test
        dnxr_human_use_blocked_without_authorization_test
        dnxr_stimulus_zeroed_on_missing_monitoring_test
        dnxr_deadline_violation_triggers_emergency_test
        dnxr_patient_stop_signal_latches_shutdown_test
        dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test
        dnxr_evidence_signature_rejects_tampered_bundle_test
        dnxr_human_unlock_rejects_wrong_build_hash_test
        dnxr_human_unlock_rejects_revoked_signature_test
        dnxr_human_unlock_rejects_expired_protocol_test
        dnxr_human_unlock_rejects_unsigned_patient_session_test
        dnxr_runtime_five_sense_coherence_stage_test
        dnxr_runtime_body_schema_stage_test
        dnxr_runtime_closed_loop_feedback_stage_test
        dnxr_runtime_perception_correction_stage_test
        dnxr_neural_stimulus_translator_path_test
        dnxr_full_dive_neural_frame_no_allocation_test
        dnxr_neural_chip_only_interface_test
        dnxr_modality_phase_lock_engine_test
        dnxr_perception_divergence_detector_test
        dnxr_device_capability_limiter_test
        dnxr_neural_chip_ack_required_test
        dnxr_neural_chip_transport_lifecycle_test
        dnxr_hal_accepts_seven_modality_full_dive_command_test
        dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test
        dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test
        dnxr_signal_quality_gate_blocks_stimulus_test
        dnxr_neural_stimulus_translator_fixed_command_no_allocation_test
        dnxr_clinical_boundary_rejects_research_orchestrator_test
        dnxr_clinical_asset_gate_blocks_mmorpg_combat_test
        dnxr_five_sense_exposure_ledger_session_limit_test
        dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test
        dnxr_neural_vector_validator_test
        dnxr_patient_specific_neural_map_test
        dnxr_modality_exposure_ledger_test
        dnxr_per_modality_isolation_broker_test
        dnxr_neural_chip_only_pipeline_test
        dnxr_visual_cortical_world_encoder_test
        dnxr_canonical_visual_bci_spike_path_test
        dnxr_canonical_multimodal_bci_spike_path_test
        dnxr_virtual_epidermis_contact_codec_test
        dnxr_avatar_kinematic_rig_controller_test
        dnxr_chemical_channels_hold_test
        dnxr_reality_anchor_hard_channel_test
        dnxr_neural_chip_bus_direction_split_test
        dnxr_motor_intent_decoder_pipeline_test
        dnxr_frame_lifecycle_guard_test
        dnxr_patient_modality_policy_gate_test
        dnxr_safe_chip_frame_hold_buffer_test
        dnxr_neural_chip_latency_budget_test
        dnxr_closed_loop_perception_error_controller_test
        dnxr_chip_capability_contract_test
        dnxr_neural_chip_adversarial_frame_test
        dnxr_neural_chip_frame_committer_test
        dnxr_closed_loop_pipeline_integration_test
        dnxr_neural_vector_slew_rate_limiter_test
        dnxr_experimental_chemical_channel_gate_test
        dnxr_neural_chip_pipeline_config_test
        dnxr_neural_chip_decision_trace_test
        dnxr_full_dive_chip_frame_diff_test
        dnxr_neural_chip_session_state_test
        dnxr_neural_chip_emergency_latch_test
        dnxr_neural_chip_only_long_session_stability_test
        dnxr_neural_chip_adversarial_sequence_test
        dnxr_neural_chip_hold_commit_policy_test
        dnxr_neural_chip_replay_recorder_integration_test
        dnxr_full_dive_chip_frame_diff_coverage_test
        dnxr_per_modality_rate_and_dose_policy_integration_test
        dnxr_neural_chip_session_supervisor_test
        dnxr_neural_chip_emergency_latch_authorized_reset_test
        dnxr_neural_chip_ack_handshake_test
        dnxr_lifecycle_two_phase_commit_test
        dnxr_hold_fallback_chain_does_not_poison_lifecycle_test
        dnxr_ack_pending_state_test
        dnxr_hold_reality_anchor_safe_test
        dnxr_world_frame_lifecycle_reject_test
        dnxr_disabled_modality_full_neutrality_test
        dnxr_sensor_encoder_contract_full_coverage_test
        dnxr_pipeline_resume_after_latch_reset_test
        dnxr_replay_recorder_bounded_memory_test
        dnxr_session_digest_test
        dnxr_motor_intent_confidence_gate_test
        dnxr_avatar_safety_pose_controller_test
        dnxr_neural_chip_pipeline_config_validator_test
        dnxr_neural_chip_pipeline_fuzz_session_test
        dnxr_neural_chip_ack_external_injection_test
        dnxr_hold_frame_sequencer_test
        dnxr_neural_chip_hold_lifecycle_test
        dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test
        dnxr_pipeline_resume_controller_test
        dnxr_emergency_reset_audit_trail_test
        dnxr_neural_chip_replay_recorder_jsonl_test
        dnxr_neural_chip_replay_player_test
        dnxr_decision_trace_modality_fieldpath_test
        dnxr_decision_trace_coverage_validator_test
        dnxr_disabled_modality_neutrality_validator_test
        dnxr_reality_anchor_degradation_model_test
        dnxr_world_frame_sanitizer_test
        dnxr_sensor_encoder_output_contract_test
        dnxr_neural_chip_pipeline_state_machine_test
        dnxr_pipeline_state_machine_ack_timeout_sequence_test
        dnxr_pipeline_config_loader_parser_result_test
        dnxr_decision_trace_path_aware_coverage_test
        dnxr_emergency_latch_controller_audit_test
        dnxr_pipeline_session_digest_integration_test
        dnxr_safety_invariant_engine_test
        dnxr_sanitize_failure_frame_factory_test
        dnxr_neural_chip_timebase_lifecycle_ack_test
        dnxr_neural_vector_field_registry_single_source_test
        dnxr_pipeline_state_gate_test
        dnxr_ack_early_gate_before_encoders_test
        dnxr_reject_with_hold_path_kind_test
        dnxr_pipeline_path_classifier_test
        dnxr_decision_trace_coverage_pipeline_integration_test
        dnxr_real_readiness_gate_allows_hardware_bench_no_human_test
        dnxr_real_readiness_gate_blocks_human_without_authorization_test
        dnxr_hardware_bench_requires_dummy_load_test
        dnxr_hardware_bench_requires_emergency_stop_test
        dnxr_hardware_bench_requires_trace_recorder_test
        dnxr_real_hal_rejects_raw_sensory_payload_compile_test
        dnxr_real_hal_accepts_bounded_stimulus_command_test
        dnxr_hardware_bench_dry_run_sends_no_active_output_test
        dnxr_emergency_latch_controller_pipeline_integration_test
        dnxr_hold_stability_tracker_test
        dnxr_pipeline_resume_state_bridge_test
        dnxr_replay_schema_version_test
        dnxr_replay_consistency_verifier_test
        dnxr_pipeline_safety_case_report_test
        dnxr_pipeline_state_machine_runtime_invariant_test
        dnxr_pipeline_no_auto_ack_recovery_test
        dnxr_timebase_advances_on_repeated_world_timestamp_test
        dnxr_pipeline_shutdown_no_normal_frames_test
        dnxr_state_machine_transition_matrix_test
        dnxr_strict_trace_coverage_blocks_incomplete_trace_test
        dnxr_frame_commit_result_bus_submitted_test
        dnxr_emergency_transition_requires_audited_reset_test
        dnxr_step_result_consistency_verifier_test
        dnxr_pipeline_reject_code_classifier_test
        dnxr_neural_chip_stateful_long_session_test
        dnxr_human_subject_use_gate_test
        dnxr_release_mode_lock_test
        dnxr_medical_review_gate_test
        dnxr_regulatory_authorization_evidence_gate_test
        dnxr_hardware_bench_readiness_gate_test
        dnxr_p95_p99_acceptance_gate_test
        dnxr_performance_telemetry_recorder_test
        dnxr_hardware_bench_session_runner_test
        dnxr_bench_only_neural_hardware_adapter_test
        dnxr_traceability_coverage_gate_test
        dnxr_release_readiness_gate_test
        dnxr_signed_evidence_manifest_test
        dnxr_gustatory_discrete_stimulation_test
        dnxr_haptic_proprioception_mismatch_test
        dnxr_no_pain_mapping_test
        dnxr_olfactory_persistence_budget_test
        dnxr_thermal_burn_lockout_test
        dnxr_visual_audio_phase_lock_test
        dnxr_visual_vestibular_conflict_blocks_motion_test
        dnxr_neural_chip_ack_timeout_test
        dnxr_neural_chip_capability_mismatch_test
        dnxr_neural_chip_feedback_required_test
        dnxr_neural_chip_firmware_mismatch_test
        dnxr_neural_chip_packet_hash_chain_test
        dnxr_neural_chip_temperature_cutoff_test
        dnxr_decompression_state_machine_test
        dnxr_dissociation_reality_anchor_test
        dnxr_flash_pattern_visual_block_test
        dnxr_locked_in_channel_loss_test
        dnxr_pain_perception_emergency_stop_test
        dnxr_vestibular_hard_limit_test
    )

    if(NOT DNXR_ENABLE_RESEARCH_SPECULATION)
        list(REMOVE_ITEM DNXR_TEST_TARGETS dnxr_full_dive_orchestrator_integration_test)
    endif()

    foreach(test_name IN LISTS DNXR_TEST_TARGETS)
        dnxr_add_test(${test_name})
    endforeach()

    if(TARGET dnxr_runtime_render_stage_test)
        target_sources(dnxr_runtime_render_stage_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime/RuntimeRenderStage.cpp)
    endif()

    foreach(immersion_compat_test IN ITEMS dnxr_pipeline_integration_test dnxr_neural_chip_immersion_integration_test)
        if(TARGET ${immersion_compat_test})
            target_sources(${immersion_compat_test} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/immersion/ImmersionCore.cpp)
        endif()
    endforeach()

    set_tests_properties(dnxr_canonical_runtime_architecture_guard_test PROPERTIES LABELS "architecture;canonical;runtime")
    set_tests_properties(dnxr_canonical_stage_graph_transaction_test PROPERTIES LABELS "architecture;canonical;runtime;transaction")
    set_tests_properties(dnxr_bci_acquisition_source_validation_test PROPERTIES LABELS "unit;runtime;acquisition;bci")
    set_tests_properties(dnxr_authenticated_virtual_world_ingress_test PROPERTIES LABELS "unit;runtime;world-ingress;canonical")
    set_tests_properties(dnxr_runtime_motor_intent_stage_test PROPERTIES LABELS "unit;runtime;motor-intent;canonical")
    set_tests_properties(dnxr_safety_state_machine_concurrency_test PROPERTIES LABELS "concurrency;safety")
    set_tests_properties(dnxr_real_readiness_gate_allows_hardware_bench_no_human_test dnxr_real_readiness_gate_blocks_human_without_authorization_test PROPERTIES LABELS "real;clinical_blocker")
    set_tests_properties(dnxr_hardware_bench_requires_dummy_load_test dnxr_hardware_bench_requires_emergency_stop_test dnxr_hardware_bench_requires_trace_recorder_test dnxr_hardware_bench_dry_run_sends_no_active_output_test PROPERTIES LABELS "hardware_bench;hardware;real")
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
    set_tests_properties(dnxr_runtime_callback_ref_test PROPERTIES LABELS "unit;runtime;callback")
    set_tests_properties(dnxr_runtime_pipeline_runner_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_pipeline_services_only_test PROPERTIES LABELS "unit;runtime;services")
    set_tests_properties(dnxr_runtime_pipeline_sink_refs_test PROPERTIES LABELS "unit;runtime;sinks")
    set_tests_properties(dnxr_runtime_pipeline_run_input_ref_test PROPERTIES LABELS "unit;runtime;services")
    set_tests_properties(dnxr_runtime_pipeline_legacy_input_test PROPERTIES LABELS "unit;runtime;legacy")
    set_tests_properties(dnxr_runtime_controller_pipeline_smoke_test PROPERTIES LABELS "unit;runtime;controller")
    set_tests_properties(dnxr_runtime_controller_no_allocation_smoke_test PROPERTIES LABELS "unit;runtime;controller;memory")
    set_tests_properties(dnxr_runtime_controller_pipeline_10k_cycles_test PROPERTIES LABELS "long_simulated;runtime;controller" TIMEOUT 60)
    if(NOT DNXR_ENABLE_CONTROLLER_10K_TEST)
        set_tests_properties(dnxr_runtime_controller_pipeline_10k_cycles_test PROPERTIES DISABLED TRUE)
    endif()
    set_tests_properties(dnxr_runtime_controller_pipeline_memory_growth_test PROPERTIES LABELS "long_simulated;runtime;controller;memory")
    set_tests_properties(dnxr_runtime_controller_jitter_budget_test PROPERTIES LABELS "long_fast;runtime;controller")
    set_tests_properties(dnxr_runtime_safety_stage_test PROPERTIES LABELS "unit;runtime;stage;safety")
    set_tests_properties(dnxr_runtime_full_dive_stage_test PROPERTIES LABELS "unit;runtime;stage;full_dive")
    set_tests_properties(dnxr_runtime_render_stage_test PROPERTIES LABELS "unit;runtime;stage")
    set_tests_properties(dnxr_runtime_stimulus_stage_test PROPERTIES LABELS "unit;runtime;stage;safety")
    set_tests_properties(dnxr_runtime_allocation_guard_test PROPERTIES LABELS "unit;runtime;memory")
    set(DNXR_LONG_SIMULATED_TEST_TARGETS
        dnxr_runtime_controller_pipeline_10k_cycles_test
        dnxr_runtime_memory_growth_test
        dnxr_runtime_pipeline_100k_cycles_test
        dnxr_runtime_pipeline_memory_growth_test
        dnxr_runtime_stage_jitter_simulation_test
        dnxr_runtime_1h_stability_test
        dnxr_watchdog_stale_subsystem_long_test
        dnxr_runtime_controller_100k_cycles_test
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

    set_tests_properties(dnxr_hil_scenario_runner_test PROPERTIES LABELS "unit;hil;evidence")
    set_tests_properties(dnxr_hil_session_report_test PROPERTIES LABELS "unit;hil;evidence")
    set_tests_properties(dnxr_hil_risk_coverage_matrix_test PROPERTIES LABELS "unit;hil;risk")
    set_tests_properties(dnxr_hil_multi_fault_recovery_test PROPERTIES LABELS "unit;hil;safety")
    set_tests_properties(dnxr_hil_latency_p99_test PROPERTIES LABELS "unit;hil;latency")
    set_tests_properties(dnxr_canada_readiness_profile_test PROPERTIES LABELS "unit;readiness;canada")
    set_tests_properties(dnxr_us_readiness_profile_test PROPERTIES LABELS "unit;readiness;us")
    set_tests_properties(dnxr_country_readiness_blocks_without_protocol_test PROPERTIES LABELS "unit;readiness;regulatory")

    set_tests_properties(dnxr_runtime_controller_100k_cycles_test PROPERTIES LABELS "long_simulated;runtime;controller")
    if(NOT DNXR_ENABLE_CONTROLLER_100K_TEST)
        set_tests_properties(dnxr_runtime_controller_100k_cycles_test PROPERTIES DISABLED TRUE)
    endif()
    if(NOT DNXR_ENABLE_NO_ALLOCATION_GUARD)
        set_tests_properties(dnxr_runtime_no_allocation_zone_test PROPERTIES DISABLED TRUE)
    endif()
    if(NOT DNXR_ENABLE_EXTENDED_VV_TESTS)
        set_tests_properties(
            dnxr_clinical_readiness_assessor_test
            dnxr_preclinical_readiness_assessor_test
            dnxr_readiness_cannot_skip_hil_test
            dnxr_readiness_requires_traceability_test
            dnxr_readiness_evidence_repository_test
            dnxr_clinical_readiness_blocks_unsigned_evidence_test
            dnxr_regulatory_claim_guard_test
            dnxr_clinical_test_acceptance_review_gate_test
            dnxr_preclinical_readiness_requires_hil_reports_test
            dnxr_country_readiness_requires_signed_artifacts_test
            dnxr_qms_review_record_test
            dnxr_qms_signoff_record_test
            dnxr_qms_review_gate_blocks_unsigned_high_risk_test
            dnxr_hil_fault_injection_test
            dnxr_hil_latency_budget_test
            dnxr_hil_emergency_stop_test
            dnxr_hil_phantom_disconnect_test
            dnxr_hil_recovery_sequence_test
            dnxr_hil_readiness_assessor_test
            dnxr_hil_scenario_runner_test
            dnxr_hil_session_report_test
            dnxr_hil_risk_coverage_matrix_test
            dnxr_hil_multi_fault_recovery_test
            dnxr_hil_latency_p99_test
            dnxr_canada_readiness_profile_test
            dnxr_us_readiness_profile_test
            dnxr_country_readiness_blocks_without_protocol_test
            PROPERTIES DISABLED TRUE)
    endif()
    if(NOT DNXR_ENABLE_LONG_SIMULATED_TESTS)
        set_tests_properties(${DNXR_LONG_SIMULATED_TEST_TARGETS} PROPERTIES DISABLED TRUE)
    endif()
    if(NOT DNXR_ENABLE_LONG_REAL_TESTS)
        set_tests_properties(${DNXR_LONG_REAL_TEST_TARGETS} PROPERTIES DISABLED TRUE)
    endif()
endif()
