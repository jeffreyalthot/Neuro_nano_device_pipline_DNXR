# Sanitizer UBSAN Report

> Confidentiality: Level 3 — NDA controlled.
> Boundary: UndefinedBehaviorSanitizer evidence for preclinical software due diligence; not clinical validation or human-use authorization.

## Run metadata
- Configure command: `cmake -S . -B /tmp/neuro_build_ubsan -DDNXR_BUILD_TESTS=ON -DDNXR_SIMULATION_ONLY=ON -DDNXR_ENABLE_UBSAN=ON`
- Build command: `cmake --build /tmp/neuro_build_ubsan -j2`
- Test command: `ctest --test-dir /tmp/neuro_build_ubsan --output-on-failure`
- Result: PASS.

## Configure log
```text
-- The CXX compiler identification is GNU 13.3.0
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.7s)
-- Generating done (0.5s)
-- Build files have been written to: /tmp/neuro_build_ubsan
```

## Build log
```text
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/app/CliDashboard.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/affective/AffectiveRealityEngine.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/app/RuntimeApplication.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/app/RuntimeBootstrap.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/assets/AssetRegistry.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/assets/ClinicalAssetGate.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/bench/DummyLoadAdapter.cpp.o
[  0%] Building CXX object CMakeFiles/dnxr_core.dir/src/bench/NeuralChipPhantomInterface.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/bench/SignalSinkProbe.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/AdaptationLayer.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/CalibrationDependencyGraph.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/CalibrationDriftMonitor.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/CalibrationDriftReactionPolicy.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/CalibrationRollbackGuard.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/ModalityCalibrationEngine.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/NeuralCalibrationEngine.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/PatientSpecificCalibrationProfile.cpp.o
[  1%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/SafeAdaptiveCalibration.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/calibration/SessionCalibrationVerifier.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/clinical/ClinicalEvidenceVerifier.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/clinical/ClinicalModeGate.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/clinical/ClinicalSessionUnlock.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/clinical/ClinicalUnlockManager.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/clinical/HumanSubjectSessionController.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/ConsciousDreamingEngine.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/InducedVectorInfluenceEngine.cpp.o
[  2%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/IntentModel.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/NeuroDigitalTwin.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/PersistentNeuroCognitiveTwin.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/StateInferenceEngine.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/cognition/SubjectiveContinuityEngine.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/ConfigLoader.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/FullUnlockedConfig.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/JsonParser.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/RuntimeConfig.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/RuntimeMode.cpp.o
[  3%] Building CXX object CMakeFiles/dnxr_core.dir/src/config/SafetyProfileLoader.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/ExecutionReport.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/NeuroRealtimeScheduler.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/PipelineOrchestrator.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/PipelineStateMachine.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/RuntimeController.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/RuntimeDeploymentMode.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/RuntimeDiagnostics.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/core/SystemKernel.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/devices/HostComputerBridge.cpp.o
[  4%] Building CXX object CMakeFiles/dnxr_core.dir/src/devices/NanoNodeInterface.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/devices/XRHeadsetInterface.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/evidence/AdverseEventRecorder.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/evidence/TamperEvidentClinicalLog.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/BodySchemaEngine.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/ExpectedNeuralResponseModel.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/ExpectedPerceptionModel.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FiveSenseCoherenceEngine.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FiveSenseFusionModel.cpp.o
[  5%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FiveSenseNeuralState.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FrameDropCompensator.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveCapabilityMatrix.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveEvidenceRecorder.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveLatencyBudget.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveModeDegradationManager.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveNeuralFrame.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveReadinessAssessor.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/FullDiveSessionValidator.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/InteroceptiveSafetyModel.cpp.o
[  6%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/ModalityPhaseLockEngine.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/MotorIntentClosedLoop.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/NeuralSensoryInterpolator.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/NeuralStimulusTranslator.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/NeuralTargetMap.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/PatientSessionClock.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/PerModalityFeedbackAnalyzer.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/PerceptionDivergenceDetector.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/PerceptionErrorCorrector.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/SensoryEncodingModelRegistry.cpp.o
[  7%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/auditory_chip/AuditoryTonotopicEncoder.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/chip_pipeline/ClosedLoopPerceptionErrorController.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/chip_pipeline/SanitizeFailureFrameFactory.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/chip_pipeline/SensorEncoderOutputContract.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/chip_pipeline/WorldFrameSanitizer.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/gustatory_chip/NeuralGustatoryEncoder.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/AvatarBalanceController.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/AvatarGaitPhaseController.cpp.o
[  8%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/AvatarInverseKinematicsSolver.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/AvatarKinematicRigController.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/AvatarSafetyPoseController.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/MotorCortexReadFrameAdapter.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/MotorIntentConfidenceGate.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/motor_chip/MotorIntentDecoder.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/olfactory_chip/NeuralOlfactoryEncoder.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/ChipRealityAnchorHardChannel.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/ExperimentalChemicalChannelGate.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/ModalityExposureLedger.cpp.o
[  9%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/PerModalityIsolationBroker.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/PerModalityRateAndDosePolicy.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/PerModalitySafetyEnvelope.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/safety/RealityAnchorDegradationModel.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/sync/MultisensoryPhaseLockOrchestrator.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/sync/NeuralChipLatencyBudget.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/touch_chip/AvatarSomatotopicSurfaceMap.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/touch_chip/NeuralBodySenseEncoders.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/touch_chip/NeuralVirtualEpidermisEncoder.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/visual_chip/PhospheneFrameStabilizer.cpp.o
[ 10%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/visual_chip/VisualCorticalWorldEncoder.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/full_dive/visual_chip/VisualSceneSemanticExtractor.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/BenchOnlyNeuralHardwareAdapter.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/ChipCapabilityNegotiation.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/ClosedLoopFeedbackFrame.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/ClosedLoopNeuralFeedbackReader.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/DeviceCapabilityLimiter.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/DeviceIdentityAttestation.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/NeuralChipAck.cpp.o
[ 11%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/NeuralChipHAL.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/NeuralChipSafetySupervisor.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/NeuralDeviceHandshake.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/RealNeuralChipHAL.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/RealNeuralChipPacket.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/RealNeuralChipProtocol.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/RealNeuralChipTransport.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware/RealNeuralChipTransportFactory.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware_bench/HardwareBenchEmergencyStop.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware_bench/HardwareBenchRuntime.cpp.o
[ 12%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware_bench/HardwareBenchSelfTest.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware_bench/HardwareBenchSession.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hardware_bench/HardwareBenchTelemetry.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/BenchDeviceAdapter.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/FaultInjectionEngine.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILBenchFaultInjector.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILController.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILEvidenceRecorder.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILLatencyHistogram.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILNeuralChipModel.cpp.o
[ 13%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILReadinessAssessor.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILRiskCoverageMatrix.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILScenario.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILScenarioResult.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILScenarioRunner.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HILSessionReport.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HardwareBenchReadinessGate.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HardwareBenchSessionRunner.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/HardwarePhantomProfile.cpp.o
[ 14%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/LatencyInstrumentation.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/NeuralSignalEmulator.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/P95P99AcceptanceGate.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/hil/StimulusLoadSimulator.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/ClinicalNeutralSceneController.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/GameDesignBridge.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/GustatorySafetyEngine.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/HapticTextureEngine.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/IdentityContinuityMemory.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/ImmersionCore.cpp.o
[ 15%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/MultiSenseOrchestrator.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/MultisensoryClock.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/NeuralPerceptionSynthesizer.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/OlfactoryPersistenceEngine.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/RealityAnchorOverlay.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SafeImpactModel.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SceneCapabilityContract.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SceneRiskAnnotator.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SceneTransitionSafetyEngine.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SensoryFrameSchema.cpp.o
[ 16%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SensoryRenderer.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SpatialAudioEngine.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/SyntheticProprioceptionLayer.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/ThermalPerceptionEngine.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/VestibularEmulationEngine.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/VirtualBodyPoseProvider.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/VirtualBodySafetyContract.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/immersion/VirtualWorldState.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/input/FileReplaySignalProvider.cpp.o
[ 17%] Building CXX object CMakeFiles/dnxr_core.dir/src/input/RuntimeSignalRouter.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/input/SimulatedSignalProvider.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/interface/NeuralLinkDeviceInterface.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/memory/ImmersiveMemoryPersistenceEngine.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/mmorpg/QuestSystem.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/NeuralCryptoTransport.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/NeuralIntentProtocol.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/NeuralMeshProtocol.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/OpenNeuralInteropProtocol.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/PacketRouter.cpp.o
[ 18%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/PacketSigner.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/ReplayProtectionWindow.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/SecureSession.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/network/SimulationCryptoTransport.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural/MotorPredictionLoop.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural/NeuralOscillationModel.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/AckHoldFrameFactory.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/ChipCapabilityContract.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/DecisionTraceCoverageValidator.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/DisabledModalityNeutralityValidator.cpp.o
[ 19%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/EmergencyLatchController.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/EmergencyResetAuditTrail.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/FrameLifecycleGuard.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/FullDiveChipFrameDiff.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/FullDiveChipFrameToSafeStimulusTranslator.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/HoldFrameSequencer.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/HoldStabilityTracker.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/HumanSubjectUseGate.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipAckHandshake.cpp.o
[ 20%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipBus.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipEmergencyLatch.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipFrameCommitter.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipHoldCommitPolicy.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipPipelineConfigLoader.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipPipelineConfigValidator.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipPipelineDecisionTrace.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipPipelineStateGate.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipPipelineStateMachine.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipReplayPlayer.cpp.o
[ 21%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipReplayRecorder.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipSessionDigest.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipSessionReport.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipSessionState.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipSessionSupervisor.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralChipTimebase.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralVectorFieldRegistry.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralVectorSlewRateLimiter.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/NeuralVectorValidator.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PatientModalityPolicyGate.cpp.o
[ 22%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PatientSpecificNeuralMap.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PerformanceTelemetryRecorder.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelinePathClassifier.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineReentrancyGuard.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineResumeController.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineResumeStateBridge.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineRuntimeInvariantMonitor.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineSafetyCaseReport.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineStateTransitionAudit.cpp.o
[ 23%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/PipelineStepMetrics.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/ReleaseModeLock.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/ReplayConsistencyVerifier.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/SafeChipFrameHoldBuffer.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/SafetyInvariantEngine.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/StateMachineTransitionValidator.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neural_chip/StepResultConsistencyVerifier.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neuro/BrainStateTopology.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/DeepImmersionPlatform.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/MultiWindowIntentDecoder.cpp.o
[ 24%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/NeuralArtifactClassifier.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/NeuralSignalQualityGate.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/NeuroSDKBridge.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/PatientBaselineTracker.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/neurotech/PerceivedBodyPoseEstimator.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/perception/RealityCoherenceEngine.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/FrameBudgetMonitor.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/HotPathDiagnostics.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/LatencyTracker.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/PerformanceMetrics.cpp.o
[ 25%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/PipelineBenchmark.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/performance/RuntimeProfiler.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/plugin/PluginRegistry.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ClinicalBuildBoundary.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ClinicalClaimLinter.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ClinicalReleaseLock.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ClinicalTestAcceptanceReviewGate.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/MedicalReviewGate.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/QmsRevocationList.cpp.o
[ 26%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/QmsSignatureVerifier.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/QmsTrustStore.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/RNDTechnicalDossierBuilder.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/RegulatoryAuthorizationEvidenceGate.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ReleaseReadinessGate.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/RequirementTraceabilityEngine.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ReviewGate.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/ReviewRecord.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/SignOffRecord.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/SignedEvidenceManifest.cpp.o
[ 27%] Building CXX object CMakeFiles/dnxr_core.dir/src/qms/TraceabilityCoverageGate.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/CanadaReadinessProfile.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/ClinicalTestReadinessAssessor.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/CountryReadinessProfile.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/EvidenceArtifact.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/EvidenceRepository.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/EvidenceReviewState.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/EvidenceStatus.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/PreclinicalReadinessAssessor.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/ReadinessScorecard.cpp.o
[ 28%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/RegulatoryClaimGuard.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/RegulatoryEvidenceIndex.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/readiness/USReadinessProfile.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/real/HardwareBenchAuthorization.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/real/RealModePolicy.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/real/RealReadinessGate.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/real/RealTransportUnlockContext.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/rendering/XRRenderer.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/replay/ReplayConsistencyReport.cpp.o
[ 29%] Building CXX object CMakeFiles/dnxr_core.dir/src/replay/ReplayPlayer.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/replay/ReplayRecorder.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/replay/ReplayVerifier.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/ClinicalRealtimeLoop.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/ConsciousnessBridge.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/DeadlineViolationPolicy.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/FullDiveClinicalOrchestrator.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/FullDiveFrameScheduler.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/FullDiveSafetyBridge.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/FullDiveSynchronizationEngine.cpp.o
[ 30%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeAcquisitionStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeAllocationGuard.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeBodyPoseExtractionStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeBodySchemaStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeCalibrationStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeClosedLoopFeedbackStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeCycleFinalizer.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeDeterminismReport.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeFiveSenseCoherenceStage.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeFrame.cpp.o
[ 31%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeFullDiveStage.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeImmersionFrameStage.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeInferenceStage.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeJitterAnalyzer.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeLatencyProfiler.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeModalityPhaseLockStage.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeNoAllocationZone.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimePerceptionCorrectionStage.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimePipelineRunner.cpp.o
[ 32%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimePostCorrectionSafetyStage.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeRenderStage.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeSafetyStage.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeSignalStage.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeStageServices.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeState.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeStimulusStage.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeTypes.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/RuntimeWCETRecorder.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/runtime/SafeFrameHoldBuffer.cpp.o
[ 33%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/AdaptiveSafetyEnvelope.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/AddictionRiskGuard.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/CognitiveReversibilitySystem.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/DecompressionStateMachine.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/DerealizationDissociationGuard.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/EmbodimentDriftGuard.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/EmergencyLatchController.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/EmergencyShutdown.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/EthicalGuardEngine.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/FiveSenseExposureLedger.cpp.o
[ 34%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/FlashPatternBlocker.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/FullDiveSafetySupervisor.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/HumanStopSignalMonitor.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/IndependentStimulusCutoff.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/IndependentWatchdog.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/IntentRiskClassifier.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/LockedInStatePreventer.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NeuralDoseLimiter.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NeuralIdentityVault.cpp.o
[ 35%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NeuralStimulusSafetyEnvelope.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NeuroConsentManager.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NeuroRightsPolicy.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/NoPainGuarantee.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/PatientSpecificSafetyEnvelope.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/PhysiologicalGuard.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/PsychologicalSafetyMonitor.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/RealityAnchorHardChannel.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/RealityAnchorSystem.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafeNeuralStimulusCommandFixed.cpp.o
[ 36%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafetyAuditTrail.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafetyDecision.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafetyGovernor.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafetyPipeline.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SafetyStateMachine.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SeizureRiskGuard.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SensoryExposureBudget.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/StabilityMonitor.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/StimulusRampController.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/SubconsciousInfluenceBlocker.cpp.o
[ 37%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/VestibularConflictGuard.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety/VestibularHardLimiter.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/safety_kernel/SafetyKernel.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/save/SaveManager.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/save/SaveMigration.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/ClinicalPacketSigner.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/ClinicalPacketVerifier.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/KeyLifecyclePolicy.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/NeuralSafetyMonitor.cpp.o
[ 38%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/SBOMManifest.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/SecretProvider.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/SecureUpdatePolicy.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/SecurityAuditTrail.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/SessionNonceManager.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/security/ThreatEvent.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/signal/EnergyThermalOptimizer.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/signal/FeatureVectorizer.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/signal/SignalStreamProcessor.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/simulation/BiometricSimulator.cpp.o
[ 39%] Building CXX object CMakeFiles/dnxr_core.dir/src/simulation/NeuralSignalSimulator.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/simulation/ScenarioRunner.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/social/SocialPresenceEngine.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/time/SubjectiveTimeEngine.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/AuditFileWriter.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/ConsoleLogSink.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/DeterministicSessionReplay.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/FileLogSink.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/Logger.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/RuntimeMetrics.cpp.o
[ 40%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/RuntimeTraceRecorder.cpp.o
[ 41%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/SessionSafetyJournal.cpp.o
[ 41%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/TerminalControl.cpp.o
[ 41%] Building CXX object CMakeFiles/dnxr_core.dir/src/utils/TimeSync.cpp.o
[ 41%] Linking CXX static library libdnxr_core.a
[ 41%] Built target dnxr_core
[ 41%] Building CXX object CMakeFiles/runtime_mode_test.dir/tests/runtime_mode_test.cpp.o
[ 41%] Building CXX object CMakeFiles/distributed_neural_xr.dir/src/main.cpp.o
[ 41%] Linking CXX executable runtime_mode_test
[ 41%] Linking CXX executable distributed_neural_xr
[ 41%] Built target runtime_mode_test
[ 41%] Building CXX object CMakeFiles/full_unlocked_runtime_test.dir/tests/full_unlocked_runtime_test.cpp.o
[ 41%] Built target distributed_neural_xr
[ 42%] Building CXX object CMakeFiles/pipeline_state_machine_test.dir/tests/pipeline_state_machine_test.cpp.o
[ 42%] Linking CXX executable full_unlocked_runtime_test
[ 42%] Built target full_unlocked_runtime_test
[ 42%] Linking CXX executable pipeline_state_machine_test
[ 42%] Building CXX object CMakeFiles/replay_consistency_test.dir/tests/replay_consistency_test.cpp.o
[ 42%] Built target pipeline_state_machine_test
[ 42%] Building CXX object CMakeFiles/performance_smoke_test.dir/tests/performance_smoke_test.cpp.o
[ 42%] Linking CXX executable replay_consistency_test
[ 42%] Built target replay_consistency_test
[ 42%] Building CXX object CMakeFiles/safety_decision_test.dir/tests/safety_decision_test.cpp.o
[ 42%] Linking CXX executable performance_smoke_test
[ 42%] Linking CXX executable safety_decision_test
[ 42%] Built target performance_smoke_test
[ 42%] Building CXX object CMakeFiles/timebase_sync_test.dir/tests/timebase_sync_test.cpp.o
[ 42%] Built target safety_decision_test
[ 43%] Linking CXX executable timebase_sync_test
[ 43%] Building CXX object CMakeFiles/dnxr_state_smoke_test.dir/tests/dnxr_state_smoke_test.cpp.o
[ 43%] Built target timebase_sync_test
[ 44%] Building CXX object CMakeFiles/dnxr_config_loader_test.dir/tests/dnxr_config_loader_test.cpp.o
[ 44%] Linking CXX executable dnxr_config_loader_test
[ 44%] Linking CXX executable dnxr_state_smoke_test
[ 44%] Built target dnxr_config_loader_test
[ 44%] Building CXX object CMakeFiles/dnxr_json_parser_test.dir/tests/dnxr_json_parser_test.cpp.o
[ 44%] Built target dnxr_state_smoke_test
[ 44%] Building CXX object CMakeFiles/dnxr_calibration_drift_monitor_test.dir/tests/dnxr_calibration_drift_monitor_test.cpp.o
[ 44%] Linking CXX executable dnxr_json_parser_test
[ 44%] Built target dnxr_json_parser_test
[ 44%] Linking CXX executable dnxr_calibration_drift_monitor_test
[ 44%] Building CXX object CMakeFiles/dnxr_save_asset_manager_test.dir/tests/dnxr_save_asset_manager_test.cpp.o
[ 44%] Built target dnxr_calibration_drift_monitor_test
[ 44%] Building CXX object CMakeFiles/dnxr_plugin_registry_test.dir/tests/dnxr_plugin_registry_test.cpp.o
[ 44%] Linking CXX executable dnxr_save_asset_manager_test
[ 44%] Built target dnxr_save_asset_manager_test
[ 44%] Building CXX object CMakeFiles/dnxr_signal_unit_test.dir/tests/dnxr_signal_unit_test.cpp.o
[ 44%] Linking CXX executable dnxr_plugin_registry_test
[ 44%] Built target dnxr_plugin_registry_test
[ 45%] Building CXX object CMakeFiles/dnxr_file_replay_signal_provider_test.dir/tests/dnxr_file_replay_signal_provider_test.cpp.o
[ 45%] Linking CXX executable dnxr_signal_unit_test
[ 45%] Built target dnxr_signal_unit_test
[ 45%] Building CXX object CMakeFiles/dnxr_signal_dropout_test.dir/tests/dnxr_signal_dropout_test.cpp.o
[ 45%] Linking CXX executable dnxr_file_replay_signal_provider_test
[ 45%] Built target dnxr_file_replay_signal_provider_test
[ 45%] Building CXX object CMakeFiles/dnxr_cognition_unit_test.dir/tests/dnxr_cognition_unit_test.cpp.o
[ 45%] Linking CXX executable dnxr_signal_dropout_test
[ 45%] Built target dnxr_signal_dropout_test
[ 45%] Building CXX object CMakeFiles/dnxr_safety_unit_test.dir/tests/dnxr_safety_unit_test.cpp.o
[ 45%] Linking CXX executable dnxr_cognition_unit_test
[ 45%] Built target dnxr_cognition_unit_test
[ 45%] Linking CXX executable dnxr_safety_unit_test
[ 45%] Building CXX object CMakeFiles/dnxr_safety_state_machine_test.dir/tests/dnxr_safety_state_machine_test.cpp.o
[ 45%] Built target dnxr_safety_unit_test
[ 45%] Building CXX object CMakeFiles/dnxr_watchdog_failure_test.dir/tests/dnxr_watchdog_failure_test.cpp.o
[ 46%] Linking CXX executable dnxr_safety_state_machine_test
[ 46%] Built target dnxr_safety_state_machine_test
[ 46%] Building CXX object CMakeFiles/dnxr_safety_profile_loader_test.dir/tests/dnxr_safety_profile_loader_test.cpp.o
[ 46%] Linking CXX executable dnxr_watchdog_failure_test
[ 46%] Built target dnxr_watchdog_failure_test
[ 46%] Building CXX object CMakeFiles/dnxr_simulation_only_guard_test.dir/tests/dnxr_simulation_only_guard_test.cpp.o
[ 46%] Linking CXX executable dnxr_safety_profile_loader_test
[ 46%] Built target dnxr_safety_profile_loader_test
[ 46%] Building CXX object CMakeFiles/dnxr_secret_provider_test.dir/tests/dnxr_secret_provider_test.cpp.o
[ 46%] Linking CXX executable dnxr_simulation_only_guard_test
[ 46%] Built target dnxr_simulation_only_guard_test
[ 46%] Building CXX object CMakeFiles/dnxr_safety_pipeline_test.dir/tests/dnxr_safety_pipeline_test.cpp.o
[ 46%] Linking CXX executable dnxr_secret_provider_test
[ 46%] Built target dnxr_secret_provider_test
[ 46%] Building CXX object CMakeFiles/dnxr_reality_anchor_system_test.dir/tests/dnxr_reality_anchor_system_test.cpp.o
[ 46%] Linking CXX executable dnxr_safety_pipeline_test
[ 46%] Built target dnxr_safety_pipeline_test
[ 46%] Building CXX object CMakeFiles/dnxr_multisensory_clock_test.dir/tests/dnxr_multisensory_clock_test.cpp.o
[ 47%] Linking CXX executable dnxr_reality_anchor_system_test
[ 47%] Built target dnxr_reality_anchor_system_test
[ 47%] Building CXX object CMakeFiles/dnxr_intent_risk_classifier_test.dir/tests/dnxr_intent_risk_classifier_test.cpp.o
[ 47%] Linking CXX executable dnxr_multisensory_clock_test
[ 47%] Built target dnxr_multisensory_clock_test
[ 47%] Building CXX object CMakeFiles/dnxr_neurorights_policy_test.dir/tests/dnxr_neurorights_policy_test.cpp.o
[ 47%] Linking CXX executable dnxr_intent_risk_classifier_test
[ 47%] Built target dnxr_intent_risk_classifier_test
[ 47%] Building CXX object CMakeFiles/dnxr_safety_fuzz_test.dir/tests/dnxr_safety_fuzz_test.cpp.o
[ 47%] Linking CXX executable dnxr_neurorights_policy_test
[ 47%] Built target dnxr_neurorights_policy_test
[ 47%] Building CXX object CMakeFiles/dnxr_renderer_qos_stress_test.dir/tests/dnxr_renderer_qos_stress_test.cpp.o
[ 48%] Linking CXX executable dnxr_safety_fuzz_test
[ 48%] Built target dnxr_safety_fuzz_test
[ 48%] Building CXX object CMakeFiles/dnxr_pipeline_integration_test.dir/tests/dnxr_pipeline_integration_test.cpp.o
[ 48%] Linking CXX executable dnxr_renderer_qos_stress_test
[ 48%] Built target dnxr_renderer_qos_stress_test
[ 49%] Building CXX object CMakeFiles/dnxr_neurotech_unit_test.dir/tests/dnxr_neurotech_unit_test.cpp.o
[ 49%] Linking CXX executable dnxr_pipeline_integration_test
[ 49%] Linking CXX executable dnxr_neurotech_unit_test
[ 49%] Built target dnxr_pipeline_integration_test
[ 49%] Building CXX object CMakeFiles/dnxr_neuro_sdk_bridge_test.dir/tests/dnxr_neuro_sdk_bridge_test.cpp.o
[ 49%] Built target dnxr_neurotech_unit_test
[ 49%] Building CXX object CMakeFiles/dnxr_runtime_realtime_test.dir/tests/dnxr_runtime_realtime_test.cpp.o
[ 49%] Linking CXX executable dnxr_neuro_sdk_bridge_test
[ 49%] Built target dnxr_neuro_sdk_bridge_test
[ 49%] Building CXX object CMakeFiles/dnxr_runtime_10k_cycles_test.dir/tests/dnxr_runtime_10k_cycles_test.cpp.o
[ 49%] Linking CXX executable dnxr_runtime_realtime_test
[ 49%] Built target dnxr_runtime_realtime_test
[ 49%] Building CXX object CMakeFiles/dnxr_runtime_types_test.dir/tests/dnxr_runtime_types_test.cpp.o
[ 49%] Linking CXX executable dnxr_runtime_types_test
[ 49%] Built target dnxr_runtime_types_test
[ 49%] Building CXX object CMakeFiles/dnxr_runtime_trace_recorder_test.dir/tests/dnxr_runtime_trace_recorder_test.cpp.o
[ 49%] Linking CXX executable dnxr_runtime_10k_cycles_test
[ 49%] Built target dnxr_runtime_10k_cycles_test
[ 49%] Building CXX object CMakeFiles/dnxr_logger_sink_test.dir/tests/dnxr_logger_sink_test.cpp.o
[ 50%] Linking CXX executable dnxr_runtime_trace_recorder_test
[ 50%] Built target dnxr_runtime_trace_recorder_test
[ 50%] Building CXX object CMakeFiles/dnxr_replay_determinism_test.dir/tests/dnxr_replay_determinism_test.cpp.o
[ 50%] Linking CXX executable dnxr_logger_sink_test
[ 50%] Built target dnxr_logger_sink_test
[ 50%] Building CXX object CMakeFiles/dnxr_security_foundation_test.dir/tests/dnxr_security_foundation_test.cpp.o
[ 50%] Linking CXX executable dnxr_replay_determinism_test
[ 50%] Linking CXX executable dnxr_security_foundation_test
[ 50%] Built target dnxr_replay_determinism_test
[ 50%] Built target dnxr_security_foundation_test
[ 50%] Building CXX object CMakeFiles/dnxr_network_replay_attack_test.dir/tests/dnxr_network_replay_attack_test.cpp.o
[ 50%] Building CXX object CMakeFiles/dnxr_neural_chip_hal_test.dir/tests/dnxr_neural_chip_hal_test.cpp.o
[ 50%] Linking CXX executable dnxr_network_replay_attack_test
[ 50%] Linking CXX executable dnxr_neural_chip_hal_test
[ 50%] Built target dnxr_network_replay_attack_test
[ 50%] Building CXX object CMakeFiles/dnxr_vestibular_budget_boundary_test.dir/tests/dnxr_vestibular_budget_boundary_test.cpp.o
[ 50%] Built target dnxr_neural_chip_hal_test
[ 51%] Building CXX object CMakeFiles/dnxr_sensory_frame_schema_test.dir/tests/dnxr_sensory_frame_schema_test.cpp.o
[ 52%] Linking CXX executable dnxr_vestibular_budget_boundary_test
[ 52%] Linking CXX executable dnxr_sensory_frame_schema_test
[ 52%] Built target dnxr_vestibular_budget_boundary_test
[ 52%] Building CXX object CMakeFiles/dnxr_vestibular_exposure_budget_test.dir/tests/dnxr_vestibular_exposure_budget_test.cpp.o
[ 52%] Built target dnxr_sensory_frame_schema_test
[ 53%] Building CXX object CMakeFiles/dnxr_full_dive_bridge_intervention_test.dir/tests/dnxr_full_dive_bridge_intervention_test.cpp.o
[ 53%] Linking CXX executable dnxr_vestibular_exposure_budget_test
[ 53%] Built target dnxr_vestibular_exposure_budget_test
[ 54%] Building CXX object CMakeFiles/dnxr_full_dive_intervention_window_test.dir/tests/dnxr_full_dive_intervention_window_test.cpp.o
[ 54%] Linking CXX executable dnxr_full_dive_bridge_intervention_test
[ 54%] Built target dnxr_full_dive_bridge_intervention_test
[ 54%] Building CXX object CMakeFiles/dnxr_stimulus_envelope_concurrency_test.dir/tests/dnxr_stimulus_envelope_concurrency_test.cpp.o
[ 54%] Linking CXX executable dnxr_full_dive_intervention_window_test
[ 54%] Built target dnxr_full_dive_intervention_window_test
[ 54%] Building CXX object CMakeFiles/dnxr_vestibular_exposure_budget_concurrency_test.dir/tests/dnxr_vestibular_exposure_budget_concurrency_test.cpp.o
[ 54%] Linking CXX executable dnxr_stimulus_envelope_concurrency_test
[ 54%] Built target dnxr_stimulus_envelope_concurrency_test
[ 54%] Building CXX object CMakeFiles/dnxr_safety_state_machine_concurrency_test.dir/tests/dnxr_safety_state_machine_concurrency_test.cpp.o
[ 54%] Linking CXX executable dnxr_vestibular_exposure_budget_concurrency_test
[ 54%] Built target dnxr_vestibular_exposure_budget_concurrency_test
[ 54%] Building CXX object CMakeFiles/dnxr_open_interop_protocol_test.dir/tests/dnxr_open_interop_protocol_test.cpp.o
[ 54%] Linking CXX executable dnxr_safety_state_machine_concurrency_test
[ 54%] Linking CXX executable dnxr_open_interop_protocol_test
[ 54%] Built target dnxr_safety_state_machine_concurrency_test
[ 54%] Building CXX object CMakeFiles/dnxr_neural_intent_protocol_test.dir/tests/dnxr_neural_intent_protocol_test.cpp.o
[ 54%] Built target dnxr_open_interop_protocol_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_acquisition_stage_test.dir/tests/dnxr_runtime_acquisition_stage_test.cpp.o
[ 54%] Linking CXX executable dnxr_neural_intent_protocol_test
[ 54%] Built target dnxr_neural_intent_protocol_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_inference_stage_test.dir/tests/dnxr_runtime_inference_stage_test.cpp.o
[ 54%] Linking CXX executable dnxr_runtime_acquisition_stage_test
[ 54%] Built target dnxr_runtime_acquisition_stage_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_calibration_stage_test.dir/tests/dnxr_runtime_calibration_stage_test.cpp.o
[ 54%] Linking CXX executable dnxr_runtime_inference_stage_test
[ 54%] Built target dnxr_runtime_inference_stage_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_immersion_frame_stage_test.dir/tests/dnxr_runtime_immersion_frame_stage_test.cpp.o
[ 54%] Linking CXX executable dnxr_runtime_calibration_stage_test
[ 54%] Built target dnxr_runtime_calibration_stage_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_cycle_finalizer_test.dir/tests/dnxr_runtime_cycle_finalizer_test.cpp.o
[ 54%] Linking CXX executable dnxr_runtime_cycle_finalizer_test
[ 54%] Built target dnxr_runtime_cycle_finalizer_test
[ 54%] Building CXX object CMakeFiles/dnxr_runtime_callback_ref_test.dir/tests/dnxr_runtime_callback_ref_test.cpp.o
[ 55%] Linking CXX executable dnxr_runtime_immersion_frame_stage_test
[ 55%] Linking CXX executable dnxr_runtime_callback_ref_test
[ 55%] Built target dnxr_runtime_immersion_frame_stage_test
[ 55%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_runner_test.dir/tests/dnxr_runtime_pipeline_runner_test.cpp.o
[ 55%] Built target dnxr_runtime_callback_ref_test
[ 55%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_services_only_test.dir/tests/dnxr_runtime_pipeline_services_only_test.cpp.o
[ 55%] Linking CXX executable dnxr_runtime_pipeline_services_only_test
[ 55%] Built target dnxr_runtime_pipeline_services_only_test
[ 55%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_sink_refs_test.dir/tests/dnxr_runtime_pipeline_sink_refs_test.cpp.o
[ 55%] Linking CXX executable dnxr_runtime_pipeline_runner_test
[ 55%] Built target dnxr_runtime_pipeline_runner_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_run_input_ref_test.dir/tests/dnxr_runtime_pipeline_run_input_ref_test.cpp.o
[ 56%] Linking CXX executable dnxr_runtime_pipeline_sink_refs_test
[ 56%] Built target dnxr_runtime_pipeline_sink_refs_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_legacy_input_test.dir/tests/dnxr_runtime_pipeline_legacy_input_test.cpp.o
[ 56%] Linking CXX executable dnxr_runtime_pipeline_run_input_ref_test
[ 56%] Built target dnxr_runtime_pipeline_run_input_ref_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_controller_pipeline_smoke_test.dir/tests/dnxr_runtime_controller_pipeline_smoke_test.cpp.o
[ 56%] Linking CXX executable dnxr_runtime_pipeline_legacy_input_test
[ 56%] Built target dnxr_runtime_pipeline_legacy_input_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_controller_no_allocation_smoke_test.dir/tests/dnxr_runtime_controller_no_allocation_smoke_test.cpp.o
[ 56%] Linking CXX executable dnxr_runtime_controller_pipeline_smoke_test
[ 56%] Linking CXX executable dnxr_runtime_controller_no_allocation_smoke_test
[ 56%] Built target dnxr_runtime_controller_pipeline_smoke_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_controller_pipeline_10k_cycles_test.dir/tests/dnxr_runtime_controller_pipeline_10k_cycles_test.cpp.o
[ 56%] Built target dnxr_runtime_controller_no_allocation_smoke_test
[ 56%] Building CXX object CMakeFiles/dnxr_runtime_controller_pipeline_memory_growth_test.dir/tests/dnxr_runtime_controller_pipeline_memory_growth_test.cpp.o
[ 57%] Linking CXX executable dnxr_runtime_controller_pipeline_memory_growth_test
[ 57%] Linking CXX executable dnxr_runtime_controller_pipeline_10k_cycles_test
[ 57%] Built target dnxr_runtime_controller_pipeline_memory_growth_test
[ 57%] Building CXX object CMakeFiles/dnxr_runtime_controller_jitter_budget_test.dir/tests/dnxr_runtime_controller_jitter_budget_test.cpp.o
[ 57%] Built target dnxr_runtime_controller_pipeline_10k_cycles_test
[ 57%] Building CXX object CMakeFiles/dnxr_runtime_safety_stage_test.dir/tests/dnxr_runtime_safety_stage_test.cpp.o
[ 57%] Linking CXX executable dnxr_runtime_controller_jitter_budget_test
[ 57%] Built target dnxr_runtime_controller_jitter_budget_test
[ 57%] Building CXX object CMakeFiles/dnxr_runtime_full_dive_stage_test.dir/tests/dnxr_runtime_full_dive_stage_test.cpp.o
[ 57%] Linking CXX executable dnxr_runtime_safety_stage_test
[ 57%] Built target dnxr_runtime_safety_stage_test
[ 58%] Building CXX object CMakeFiles/dnxr_runtime_render_stage_test.dir/tests/dnxr_runtime_render_stage_test.cpp.o
[ 58%] Linking CXX executable dnxr_runtime_full_dive_stage_test
[ 58%] Built target dnxr_runtime_full_dive_stage_test
[ 58%] Building CXX object CMakeFiles/dnxr_runtime_stimulus_stage_test.dir/tests/dnxr_runtime_stimulus_stage_test.cpp.o
[ 58%] Linking CXX executable dnxr_runtime_render_stage_test
[ 58%] Linking CXX executable dnxr_runtime_stimulus_stage_test
[ 58%] Built target dnxr_runtime_render_stage_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_allocation_guard_test.dir/tests/dnxr_runtime_allocation_guard_test.cpp.o
[ 59%] Built target dnxr_runtime_stimulus_stage_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_memory_growth_test.dir/tests/dnxr_runtime_memory_growth_test.cpp.o
[ 59%] Linking CXX executable dnxr_runtime_memory_growth_test
[ 59%] Linking CXX executable dnxr_runtime_allocation_guard_test
[ 59%] Built target dnxr_runtime_memory_growth_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_100k_cycles_test.dir/tests/dnxr_runtime_pipeline_100k_cycles_test.cpp.o
[ 59%] Built target dnxr_runtime_allocation_guard_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_pipeline_memory_growth_test.dir/tests/dnxr_runtime_pipeline_memory_growth_test.cpp.o
[ 59%] Linking CXX executable dnxr_runtime_pipeline_100k_cycles_test
[ 59%] Linking CXX executable dnxr_runtime_pipeline_memory_growth_test
[ 59%] Built target dnxr_runtime_pipeline_100k_cycles_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_stage_jitter_simulation_test.dir/tests/dnxr_runtime_stage_jitter_simulation_test.cpp.o
[ 59%] Built target dnxr_runtime_pipeline_memory_growth_test
[ 59%] Building CXX object CMakeFiles/dnxr_runtime_1h_stability_test.dir/tests/dnxr_runtime_1h_stability_test.cpp.o
[ 59%] Linking CXX executable dnxr_runtime_1h_stability_test
[ 59%] Built target dnxr_runtime_1h_stability_test
[ 59%] Building CXX object CMakeFiles/dnxr_vestibular_exposure_budget_long_test.dir/tests/dnxr_vestibular_exposure_budget_long_test.cpp.o
[ 59%] Linking CXX executable dnxr_vestibular_exposure_budget_long_test
[ 59%] Built target dnxr_vestibular_exposure_budget_long_test
[ 59%] Linking CXX executable dnxr_runtime_stage_jitter_simulation_test
[ 59%] Building CXX object CMakeFiles/dnxr_full_dive_intervention_window_long_test.dir/tests/dnxr_full_dive_intervention_window_long_test.cpp.o
[ 59%] Built target dnxr_runtime_stage_jitter_simulation_test
[ 60%] Building CXX object CMakeFiles/dnxr_watchdog_stale_subsystem_long_test.dir/tests/dnxr_watchdog_stale_subsystem_long_test.cpp.o
[ 60%] Linking CXX executable dnxr_full_dive_intervention_window_long_test
[ 60%] Linking CXX executable dnxr_watchdog_stale_subsystem_long_test
[ 60%] Built target dnxr_watchdog_stale_subsystem_long_test
[ 60%] Built target dnxr_full_dive_intervention_window_long_test
[ 60%] Building CXX object CMakeFiles/dnxr_clinical_readiness_assessor_test.dir/tests/dnxr_clinical_readiness_assessor_test.cpp.o
[ 60%] Building CXX object CMakeFiles/dnxr_neural_chip_immersion_integration_test.dir/tests/dnxr_neural_chip_immersion_integration_test.cpp.o
[ 60%] Linking CXX executable dnxr_clinical_readiness_assessor_test
[ 60%] Built target dnxr_clinical_readiness_assessor_test
[ 61%] Building CXX object CMakeFiles/dnxr_preclinical_readiness_assessor_test.dir/tests/dnxr_preclinical_readiness_assessor_test.cpp.o
[ 61%] Linking CXX executable dnxr_preclinical_readiness_assessor_test
[ 61%] Linking CXX executable dnxr_neural_chip_immersion_integration_test
[ 61%] Built target dnxr_preclinical_readiness_assessor_test
[ 61%] Building CXX object CMakeFiles/dnxr_readiness_cannot_skip_hil_test.dir/tests/dnxr_readiness_cannot_skip_hil_test.cpp.o
[ 61%] Built target dnxr_neural_chip_immersion_integration_test
[ 61%] Building CXX object CMakeFiles/dnxr_readiness_requires_traceability_test.dir/tests/dnxr_readiness_requires_traceability_test.cpp.o
[ 61%] Linking CXX executable dnxr_readiness_cannot_skip_hil_test
[ 61%] Linking CXX executable dnxr_readiness_requires_traceability_test
[ 61%] Built target dnxr_readiness_cannot_skip_hil_test
[ 61%] Building CXX object CMakeFiles/dnxr_readiness_evidence_repository_test.dir/tests/dnxr_readiness_evidence_repository_test.cpp.o
[ 61%] Built target dnxr_readiness_requires_traceability_test
[ 61%] Building CXX object CMakeFiles/dnxr_clinical_readiness_blocks_unsigned_evidence_test.dir/tests/dnxr_clinical_readiness_blocks_unsigned_evidence_test.cpp.o
[ 61%] Linking CXX executable dnxr_readiness_evidence_repository_test
[ 62%] Linking CXX executable dnxr_clinical_readiness_blocks_unsigned_evidence_test
[ 62%] Built target dnxr_readiness_evidence_repository_test
[ 62%] Building CXX object CMakeFiles/dnxr_regulatory_claim_guard_test.dir/tests/dnxr_regulatory_claim_guard_test.cpp.o
[ 62%] Built target dnxr_clinical_readiness_blocks_unsigned_evidence_test
[ 62%] Building CXX object CMakeFiles/dnxr_clinical_test_acceptance_review_gate_test.dir/tests/dnxr_clinical_test_acceptance_review_gate_test.cpp.o
[ 62%] Linking CXX executable dnxr_regulatory_claim_guard_test
[ 62%] Linking CXX executable dnxr_clinical_test_acceptance_review_gate_test
[ 62%] Built target dnxr_regulatory_claim_guard_test
[ 62%] Building CXX object CMakeFiles/dnxr_preclinical_readiness_requires_hil_reports_test.dir/tests/dnxr_preclinical_readiness_requires_hil_reports_test.cpp.o
[ 62%] Built target dnxr_clinical_test_acceptance_review_gate_test
[ 62%] Building CXX object CMakeFiles/dnxr_country_readiness_requires_signed_artifacts_test.dir/tests/dnxr_country_readiness_requires_signed_artifacts_test.cpp.o
[ 62%] Linking CXX executable dnxr_preclinical_readiness_requires_hil_reports_test
[ 62%] Linking CXX executable dnxr_country_readiness_requires_signed_artifacts_test
[ 62%] Built target dnxr_preclinical_readiness_requires_hil_reports_test
[ 62%] Building CXX object CMakeFiles/dnxr_qms_review_record_test.dir/tests/dnxr_qms_review_record_test.cpp.o
[ 62%] Built target dnxr_country_readiness_requires_signed_artifacts_test
[ 62%] Building CXX object CMakeFiles/dnxr_qms_signoff_record_test.dir/tests/dnxr_qms_signoff_record_test.cpp.o
[ 62%] Linking CXX executable dnxr_qms_review_record_test
[ 63%] Linking CXX executable dnxr_qms_signoff_record_test
[ 63%] Built target dnxr_qms_review_record_test
[ 63%] Built target dnxr_qms_signoff_record_test
[ 63%] Building CXX object CMakeFiles/dnxr_qms_review_gate_blocks_unsigned_high_risk_test.dir/tests/dnxr_qms_review_gate_blocks_unsigned_high_risk_test.cpp.o
[ 63%] Building CXX object CMakeFiles/dnxr_full_dive_capability_matrix_test.dir/tests/dnxr_full_dive_capability_matrix_test.cpp.o
[ 63%] Linking CXX executable dnxr_qms_review_gate_blocks_unsigned_high_risk_test
[ 63%] Linking CXX executable dnxr_full_dive_capability_matrix_test
[ 63%] Built target dnxr_qms_review_gate_blocks_unsigned_high_risk_test
[ 63%] Built target dnxr_full_dive_capability_matrix_test
[ 63%] Building CXX object CMakeFiles/dnxr_full_dive_readiness_assessor_test.dir/tests/dnxr_full_dive_readiness_assessor_test.cpp.o
[ 63%] Building CXX object CMakeFiles/dnxr_full_dive_session_validator_test.dir/tests/dnxr_full_dive_session_validator_test.cpp.o
[ 63%] Linking CXX executable dnxr_full_dive_session_validator_test
[ 63%] Linking CXX executable dnxr_full_dive_readiness_assessor_test
[ 63%] Built target dnxr_full_dive_session_validator_test
[ 63%] Building CXX object CMakeFiles/dnxr_hil_fault_injection_test.dir/tests/dnxr_hil_fault_injection_test.cpp.o
[ 63%] Built target dnxr_full_dive_readiness_assessor_test
[ 63%] Building CXX object CMakeFiles/dnxr_hil_latency_budget_test.dir/tests/dnxr_hil_latency_budget_test.cpp.o
[ 63%] Linking CXX executable dnxr_hil_latency_budget_test
[ 64%] Linking CXX executable dnxr_hil_fault_injection_test
[ 64%] Built target dnxr_hil_latency_budget_test
[ 64%] Building CXX object CMakeFiles/dnxr_hil_emergency_stop_test.dir/tests/dnxr_hil_emergency_stop_test.cpp.o
[ 64%] Built target dnxr_hil_fault_injection_test
[ 64%] Building CXX object CMakeFiles/dnxr_hil_phantom_disconnect_test.dir/tests/dnxr_hil_phantom_disconnect_test.cpp.o
[ 64%] Linking CXX executable dnxr_hil_emergency_stop_test
[ 64%] Linking CXX executable dnxr_hil_phantom_disconnect_test
[ 64%] Built target dnxr_hil_emergency_stop_test
[ 64%] Building CXX object CMakeFiles/dnxr_hil_recovery_sequence_test.dir/tests/dnxr_hil_recovery_sequence_test.cpp.o
[ 64%] Built target dnxr_hil_phantom_disconnect_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_readiness_assessor_test.dir/tests/dnxr_hil_readiness_assessor_test.cpp.o
[ 65%] Linking CXX executable dnxr_hil_recovery_sequence_test
[ 65%] Linking CXX executable dnxr_hil_readiness_assessor_test
[ 65%] Built target dnxr_hil_recovery_sequence_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_scenario_runner_test.dir/tests/dnxr_hil_scenario_runner_test.cpp.o
[ 65%] Built target dnxr_hil_readiness_assessor_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_session_report_test.dir/tests/dnxr_hil_session_report_test.cpp.o
[ 65%] Linking CXX executable dnxr_hil_scenario_runner_test
[ 65%] Linking CXX executable dnxr_hil_session_report_test
[ 65%] Built target dnxr_hil_scenario_runner_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_risk_coverage_matrix_test.dir/tests/dnxr_hil_risk_coverage_matrix_test.cpp.o
[ 65%] Built target dnxr_hil_session_report_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_multi_fault_recovery_test.dir/tests/dnxr_hil_multi_fault_recovery_test.cpp.o
[ 65%] Linking CXX executable dnxr_hil_multi_fault_recovery_test
[ 65%] Linking CXX executable dnxr_hil_risk_coverage_matrix_test
[ 65%] Built target dnxr_hil_multi_fault_recovery_test
[ 65%] Building CXX object CMakeFiles/dnxr_hil_latency_p99_test.dir/tests/dnxr_hil_latency_p99_test.cpp.o
[ 65%] Built target dnxr_hil_risk_coverage_matrix_test
[ 65%] Building CXX object CMakeFiles/dnxr_canada_readiness_profile_test.dir/tests/dnxr_canada_readiness_profile_test.cpp.o
[ 65%] Linking CXX executable dnxr_hil_latency_p99_test
[ 65%] Linking CXX executable dnxr_canada_readiness_profile_test
[ 65%] Built target dnxr_hil_latency_p99_test
[ 65%] Building CXX object CMakeFiles/dnxr_us_readiness_profile_test.dir/tests/dnxr_us_readiness_profile_test.cpp.o
[ 65%] Built target dnxr_canada_readiness_profile_test
[ 65%] Building CXX object CMakeFiles/dnxr_country_readiness_blocks_without_protocol_test.dir/tests/dnxr_country_readiness_blocks_without_protocol_test.cpp.o
[ 65%] Linking CXX executable dnxr_us_readiness_profile_test
[ 65%] Linking CXX executable dnxr_country_readiness_blocks_without_protocol_test
[ 65%] Built target dnxr_us_readiness_profile_test
[ 65%] Building CXX object CMakeFiles/dnxr_security_audit_trail_test.dir/tests/dnxr_security_audit_trail_test.cpp.o
[ 65%] Built target dnxr_country_readiness_blocks_without_protocol_test
[ 65%] Building CXX object CMakeFiles/dnxr_threat_event_test.dir/tests/dnxr_threat_event_test.cpp.o
[ 65%] Linking CXX executable dnxr_security_audit_trail_test
[ 66%] Linking CXX executable dnxr_threat_event_test
[ 66%] Built target dnxr_security_audit_trail_test
[ 66%] Building CXX object CMakeFiles/dnxr_replay_attack_blocked_test.dir/tests/dnxr_replay_attack_blocked_test.cpp.o
[ 66%] Built target dnxr_threat_event_test
[ 66%] Building CXX object CMakeFiles/dnxr_packet_tamper_detection_test.dir/tests/dnxr_packet_tamper_detection_test.cpp.o
[ 66%] Linking CXX executable dnxr_replay_attack_blocked_test
[ 66%] Linking CXX executable dnxr_packet_tamper_detection_test
[ 66%] Built target dnxr_replay_attack_blocked_test
[ 67%] Building CXX object CMakeFiles/dnxr_secure_update_policy_test.dir/tests/dnxr_secure_update_policy_test.cpp.o
[ 67%] Built target dnxr_packet_tamper_detection_test
[ 67%] Building CXX object CMakeFiles/dnxr_runtime_controller_100k_cycles_test.dir/tests/dnxr_runtime_controller_100k_cycles_test.cpp.o
[ 67%] Linking CXX executable dnxr_secure_update_policy_test
[ 67%] Linking CXX executable dnxr_runtime_controller_100k_cycles_test
[ 67%] Built target dnxr_secure_update_policy_test
[ 67%] Built target dnxr_runtime_controller_100k_cycles_test
[ 68%] Building CXX object CMakeFiles/dnxr_runtime_no_allocation_zone_test.dir/tests/dnxr_runtime_no_allocation_zone_test.cpp.o
[ 68%] Building CXX object CMakeFiles/dnxr_runtime_latency_profiler_test.dir/tests/dnxr_runtime_latency_profiler_test.cpp.o
[ 68%] Linking CXX executable dnxr_runtime_no_allocation_zone_test
[ 68%] Linking CXX executable dnxr_runtime_latency_profiler_test
[ 68%] Built target dnxr_runtime_no_allocation_zone_test
[ 68%] Building CXX object CMakeFiles/dnxr_runtime_jitter_analyzer_test.dir/tests/dnxr_runtime_jitter_analyzer_test.cpp.o
[ 68%] Built target dnxr_runtime_latency_profiler_test
[ 68%] Building CXX object CMakeFiles/dnxr_runtime_wcet_recorder_test.dir/tests/dnxr_runtime_wcet_recorder_test.cpp.o
[ 68%] Linking CXX executable dnxr_runtime_wcet_recorder_test
[ 68%] Linking CXX executable dnxr_runtime_jitter_analyzer_test
[ 68%] Built target dnxr_runtime_jitter_analyzer_test
[ 68%] Built target dnxr_runtime_wcet_recorder_test
[ 69%] Building CXX object CMakeFiles/dnxr_human_use_blocked_without_authorization_test.dir/tests/dnxr_human_use_blocked_without_authorization_test.cpp.o
[ 69%] Building CXX object CMakeFiles/dnxr_stimulus_zeroed_on_missing_monitoring_test.dir/tests/dnxr_stimulus_zeroed_on_missing_monitoring_test.cpp.o
[ 69%] Linking CXX executable dnxr_human_use_blocked_without_authorization_test
[ 69%] Linking CXX executable dnxr_stimulus_zeroed_on_missing_monitoring_test
[ 69%] Built target dnxr_human_use_blocked_without_authorization_test
[ 69%] Building CXX object CMakeFiles/dnxr_deadline_violation_triggers_emergency_test.dir/tests/dnxr_deadline_violation_triggers_emergency_test.cpp.o
[ 69%] Built target dnxr_stimulus_zeroed_on_missing_monitoring_test
[ 69%] Building CXX object CMakeFiles/dnxr_patient_stop_signal_latches_shutdown_test.dir/tests/dnxr_patient_stop_signal_latches_shutdown_test.cpp.o
[ 70%] Linking CXX executable dnxr_deadline_violation_triggers_emergency_test
[ 71%] Linking CXX executable dnxr_patient_stop_signal_latches_shutdown_test
[ 71%] Built target dnxr_deadline_violation_triggers_emergency_test
[ 71%] Built target dnxr_patient_stop_signal_latches_shutdown_test
[ 71%] Building CXX object CMakeFiles/dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test.dir/tests/dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test.cpp.o
[ 71%] Building CXX object CMakeFiles/dnxr_runtime_five_sense_coherence_stage_test.dir/tests/dnxr_runtime_five_sense_coherence_stage_test.cpp.o
[ 71%] Linking CXX executable dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test
[ 71%] Built target dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test
[ 71%] Building CXX object CMakeFiles/dnxr_runtime_body_schema_stage_test.dir/tests/dnxr_runtime_body_schema_stage_test.cpp.o
[ 71%] Linking CXX executable dnxr_runtime_five_sense_coherence_stage_test
[ 71%] Built target dnxr_runtime_five_sense_coherence_stage_test
[ 71%] Building CXX object CMakeFiles/dnxr_runtime_closed_loop_feedback_stage_test.dir/tests/dnxr_runtime_closed_loop_feedback_stage_test.cpp.o
[ 71%] Linking CXX executable dnxr_runtime_body_schema_stage_test
[ 71%] Built target dnxr_runtime_body_schema_stage_test
[ 71%] Building CXX object CMakeFiles/dnxr_runtime_perception_correction_stage_test.dir/tests/dnxr_runtime_perception_correction_stage_test.cpp.o
[ 72%] Linking CXX executable dnxr_runtime_closed_loop_feedback_stage_test
[ 72%] Built target dnxr_runtime_closed_loop_feedback_stage_test
[ 72%] Building CXX object CMakeFiles/dnxr_neural_stimulus_translator_path_test.dir/tests/dnxr_neural_stimulus_translator_path_test.cpp.o
[ 72%] Linking CXX executable dnxr_runtime_perception_correction_stage_test
[ 72%] Built target dnxr_runtime_perception_correction_stage_test
[ 72%] Linking CXX executable dnxr_neural_stimulus_translator_path_test
[ 72%] Building CXX object CMakeFiles/dnxr_full_dive_neural_frame_no_allocation_test.dir/tests/dnxr_full_dive_neural_frame_no_allocation_test.cpp.o
[ 72%] Built target dnxr_neural_stimulus_translator_path_test
[ 72%] Building CXX object CMakeFiles/dnxr_neural_chip_only_interface_test.dir/tests/dnxr_neural_chip_only_interface_test.cpp.o
[ 73%] Linking CXX executable dnxr_neural_chip_only_interface_test
[ 73%] Linking CXX executable dnxr_full_dive_neural_frame_no_allocation_test
[ 73%] Built target dnxr_neural_chip_only_interface_test
[ 73%] Built target dnxr_full_dive_neural_frame_no_allocation_test
[ 73%] Building CXX object CMakeFiles/dnxr_modality_phase_lock_engine_test.dir/tests/dnxr_modality_phase_lock_engine_test.cpp.o
[ 73%] Building CXX object CMakeFiles/dnxr_perception_divergence_detector_test.dir/tests/dnxr_perception_divergence_detector_test.cpp.o
[ 73%] Linking CXX executable dnxr_modality_phase_lock_engine_test
[ 73%] Linking CXX executable dnxr_perception_divergence_detector_test
[ 73%] Built target dnxr_modality_phase_lock_engine_test
[ 73%] Building CXX object CMakeFiles/dnxr_device_capability_limiter_test.dir/tests/dnxr_device_capability_limiter_test.cpp.o
[ 73%] Built target dnxr_perception_divergence_detector_test
[ 73%] Building CXX object CMakeFiles/dnxr_neural_chip_ack_required_test.dir/tests/dnxr_neural_chip_ack_required_test.cpp.o
[ 73%] Linking CXX executable dnxr_device_capability_limiter_test
[ 73%] Linking CXX executable dnxr_neural_chip_ack_required_test
[ 73%] Built target dnxr_device_capability_limiter_test
[ 73%] Building CXX object CMakeFiles/dnxr_hal_accepts_seven_modality_full_dive_command_test.dir/tests/hardware/dnxr_hal_accepts_seven_modality_full_dive_command_test.cpp.o
[ 73%] Built target dnxr_neural_chip_ack_required_test
[ 73%] Building CXX object CMakeFiles/dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test.dir/tests/runtime/dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test.cpp.o
[ 73%] Linking CXX executable dnxr_hal_accepts_seven_modality_full_dive_command_test
[ 74%] Linking CXX executable dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test
[ 74%] Built target dnxr_hal_accepts_seven_modality_full_dive_command_test
[ 74%] Building CXX object CMakeFiles/dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test.dir/tests/runtime/dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test.cpp.o
[ 74%] Built target dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test
[ 74%] Building CXX object CMakeFiles/dnxr_signal_quality_gate_blocks_stimulus_test.dir/tests/runtime/dnxr_signal_quality_gate_blocks_stimulus_test.cpp.o
[ 74%] Linking CXX executable dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test
[ 74%] Linking CXX executable dnxr_signal_quality_gate_blocks_stimulus_test
[ 74%] Built target dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test
[ 74%] Building CXX object CMakeFiles/dnxr_neural_stimulus_translator_fixed_command_no_allocation_test.dir/tests/performance/dnxr_neural_stimulus_translator_fixed_command_no_allocation_test.cpp.o
[ 74%] Built target dnxr_signal_quality_gate_blocks_stimulus_test
[ 74%] Building CXX object CMakeFiles/dnxr_clinical_boundary_rejects_research_orchestrator_test.dir/tests/qms/dnxr_clinical_boundary_rejects_research_orchestrator_test.cpp.o
[ 74%] Linking CXX executable dnxr_clinical_boundary_rejects_research_orchestrator_test
[ 74%] Built target dnxr_clinical_boundary_rejects_research_orchestrator_test
[ 74%] Linking CXX executable dnxr_neural_stimulus_translator_fixed_command_no_allocation_test
[ 74%] Building CXX object CMakeFiles/dnxr_clinical_asset_gate_blocks_mmorpg_combat_test.dir/tests/assets/dnxr_clinical_asset_gate_blocks_mmorpg_combat_test.cpp.o
[ 74%] Built target dnxr_neural_stimulus_translator_fixed_command_no_allocation_test
[ 74%] Building CXX object CMakeFiles/dnxr_five_sense_exposure_ledger_session_limit_test.dir/tests/safety/dnxr_five_sense_exposure_ledger_session_limit_test.cpp.o
[ 74%] Linking CXX executable dnxr_clinical_asset_gate_blocks_mmorpg_combat_test
[ 74%] Built target dnxr_clinical_asset_gate_blocks_mmorpg_combat_test
[ 74%] Linking CXX executable dnxr_five_sense_exposure_ledger_session_limit_test
[ 74%] Building CXX object CMakeFiles/dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test.dir/tests/safety/dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test.cpp.o
[ 74%] Built target dnxr_five_sense_exposure_ledger_session_limit_test
[ 74%] Building CXX object CMakeFiles/dnxr_neural_vector_validator_test.dir/tests/dnxr_neural_vector_validator_test.cpp.o
[ 74%] Linking CXX executable dnxr_neural_vector_validator_test
[ 74%] Linking CXX executable dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test
[ 74%] Built target dnxr_neural_vector_validator_test
[ 74%] Building CXX object CMakeFiles/dnxr_patient_specific_neural_map_test.dir/tests/dnxr_patient_specific_neural_map_test.cpp.o
[ 74%] Linking CXX executable dnxr_patient_specific_neural_map_test
[ 74%] Built target dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test
[ 74%] Building CXX object CMakeFiles/dnxr_modality_exposure_ledger_test.dir/tests/dnxr_modality_exposure_ledger_test.cpp.o
[ 74%] Linking CXX executable dnxr_modality_exposure_ledger_test
[ 74%] Built target dnxr_patient_specific_neural_map_test
[ 74%] Building CXX object CMakeFiles/dnxr_per_modality_isolation_broker_test.dir/tests/dnxr_per_modality_isolation_broker_test.cpp.o
[ 74%] Linking CXX executable dnxr_per_modality_isolation_broker_test
[ 74%] Built target dnxr_modality_exposure_ledger_test
[ 74%] Building CXX object CMakeFiles/dnxr_neural_chip_only_pipeline_test.dir/tests/dnxr_neural_chip_only_pipeline_test.cpp.o
[ 74%] Built target dnxr_per_modality_isolation_broker_test
[ 74%] Building CXX object CMakeFiles/dnxr_visual_cortical_world_encoder_test.dir/tests/dnxr_visual_cortical_world_encoder_test.cpp.o
[ 74%] Linking CXX executable dnxr_visual_cortical_world_encoder_test
[ 74%] Built target dnxr_visual_cortical_world_encoder_test
[ 75%] Building CXX object CMakeFiles/dnxr_virtual_epidermis_contact_codec_test.dir/tests/dnxr_virtual_epidermis_contact_codec_test.cpp.o
[ 75%] Linking CXX executable dnxr_neural_chip_only_pipeline_test
[ 75%] Linking CXX executable dnxr_virtual_epidermis_contact_codec_test
[ 75%] Built target dnxr_neural_chip_only_pipeline_test
[ 75%] Building CXX object CMakeFiles/dnxr_avatar_kinematic_rig_controller_test.dir/tests/dnxr_avatar_kinematic_rig_controller_test.cpp.o
[ 75%] Built target dnxr_virtual_epidermis_contact_codec_test
[ 75%] Building CXX object CMakeFiles/dnxr_chemical_channels_hold_test.dir/tests/dnxr_chemical_channels_hold_test.cpp.o
[ 76%] Linking CXX executable dnxr_chemical_channels_hold_test
[ 76%] Linking CXX executable dnxr_avatar_kinematic_rig_controller_test
[ 76%] Built target dnxr_chemical_channels_hold_test
[ 76%] Building CXX object CMakeFiles/dnxr_reality_anchor_hard_channel_test.dir/tests/dnxr_reality_anchor_hard_channel_test.cpp.o
[ 76%] Built target dnxr_avatar_kinematic_rig_controller_test
[ 76%] Linking CXX executable dnxr_reality_anchor_hard_channel_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_bus_direction_split_test.dir/tests/dnxr_neural_chip_bus_direction_split_test.cpp.o
[ 76%] Linking CXX executable dnxr_neural_chip_bus_direction_split_test
[ 76%] Built target dnxr_reality_anchor_hard_channel_test
[ 76%] Building CXX object CMakeFiles/dnxr_motor_intent_decoder_pipeline_test.dir/tests/dnxr_motor_intent_decoder_pipeline_test.cpp.o
[ 76%] Built target dnxr_neural_chip_bus_direction_split_test
[ 76%] Linking CXX executable dnxr_motor_intent_decoder_pipeline_test
[ 76%] Building CXX object CMakeFiles/dnxr_frame_lifecycle_guard_test.dir/tests/dnxr_frame_lifecycle_guard_test.cpp.o
[ 76%] Linking CXX executable dnxr_frame_lifecycle_guard_test
[ 76%] Built target dnxr_motor_intent_decoder_pipeline_test
[ 76%] Building CXX object CMakeFiles/dnxr_patient_modality_policy_gate_test.dir/tests/dnxr_patient_modality_policy_gate_test.cpp.o
[ 76%] Built target dnxr_frame_lifecycle_guard_test
[ 76%] Linking CXX executable dnxr_patient_modality_policy_gate_test
[ 76%] Building CXX object CMakeFiles/dnxr_safe_chip_frame_hold_buffer_test.dir/tests/dnxr_safe_chip_frame_hold_buffer_test.cpp.o
[ 76%] Linking CXX executable dnxr_safe_chip_frame_hold_buffer_test
[ 76%] Built target dnxr_patient_modality_policy_gate_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_latency_budget_test.dir/tests/dnxr_neural_chip_latency_budget_test.cpp.o
[ 76%] Built target dnxr_safe_chip_frame_hold_buffer_test
[ 76%] Linking CXX executable dnxr_neural_chip_latency_budget_test
[ 76%] Building CXX object CMakeFiles/dnxr_closed_loop_perception_error_controller_test.dir/tests/dnxr_closed_loop_perception_error_controller_test.cpp.o
[ 76%] Linking CXX executable dnxr_closed_loop_perception_error_controller_test
[ 76%] Built target dnxr_neural_chip_latency_budget_test
[ 76%] Building CXX object CMakeFiles/dnxr_chip_capability_contract_test.dir/tests/dnxr_chip_capability_contract_test.cpp.o
[ 76%] Built target dnxr_closed_loop_perception_error_controller_test
[ 76%] Linking CXX executable dnxr_chip_capability_contract_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_adversarial_frame_test.dir/tests/dnxr_neural_chip_adversarial_frame_test.cpp.o
[ 76%] Built target dnxr_chip_capability_contract_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_frame_committer_test.dir/tests/dnxr_neural_chip_frame_committer_test.cpp.o
[ 76%] Linking CXX executable dnxr_neural_chip_adversarial_frame_test
[ 76%] Built target dnxr_neural_chip_adversarial_frame_test
[ 76%] Linking CXX executable dnxr_neural_chip_frame_committer_test
[ 76%] Building CXX object CMakeFiles/dnxr_closed_loop_pipeline_integration_test.dir/tests/dnxr_closed_loop_pipeline_integration_test.cpp.o
[ 76%] Built target dnxr_neural_chip_frame_committer_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_vector_slew_rate_limiter_test.dir/tests/dnxr_neural_vector_slew_rate_limiter_test.cpp.o
[ 76%] Linking CXX executable dnxr_neural_vector_slew_rate_limiter_test
[ 76%] Linking CXX executable dnxr_closed_loop_pipeline_integration_test
[ 76%] Built target dnxr_neural_vector_slew_rate_limiter_test
[ 76%] Building CXX object CMakeFiles/dnxr_experimental_chemical_channel_gate_test.dir/tests/dnxr_experimental_chemical_channel_gate_test.cpp.o
[ 76%] Linking CXX executable dnxr_experimental_chemical_channel_gate_test
[ 76%] Built target dnxr_closed_loop_pipeline_integration_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_pipeline_config_test.dir/tests/dnxr_neural_chip_pipeline_config_test.cpp.o
[ 76%] Built target dnxr_experimental_chemical_channel_gate_test
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_decision_trace_test.dir/tests/dnxr_neural_chip_decision_trace_test.cpp.o
[ 76%] Linking CXX executable dnxr_neural_chip_pipeline_config_test
[ 76%] Linking CXX executable dnxr_neural_chip_decision_trace_test
[ 76%] Built target dnxr_neural_chip_decision_trace_test
[ 76%] Built target dnxr_neural_chip_pipeline_config_test
[ 76%] Building CXX object CMakeFiles/dnxr_full_dive_chip_frame_diff_test.dir/tests/dnxr_full_dive_chip_frame_diff_test.cpp.o
[ 76%] Building CXX object CMakeFiles/dnxr_neural_chip_session_state_test.dir/tests/dnxr_neural_chip_session_state_test.cpp.o
[ 76%] Linking CXX executable dnxr_neural_chip_session_state_test
[ 76%] Linking CXX executable dnxr_full_dive_chip_frame_diff_test
[ 76%] Built target dnxr_neural_chip_session_state_test
[ 77%] Building CXX object CMakeFiles/dnxr_neural_chip_emergency_latch_test.dir/tests/dnxr_neural_chip_emergency_latch_test.cpp.o
[ 77%] Linking CXX executable dnxr_neural_chip_emergency_latch_test
[ 77%] Built target dnxr_full_dive_chip_frame_diff_test
[ 77%] Building CXX object CMakeFiles/dnxr_neural_chip_only_long_session_stability_test.dir/tests/dnxr_neural_chip_only_long_session_stability_test.cpp.o
[ 77%] Built target dnxr_neural_chip_emergency_latch_test
[ 78%] Building CXX object CMakeFiles/dnxr_neural_chip_adversarial_sequence_test.dir/tests/dnxr_neural_chip_adversarial_sequence_test.cpp.o
[ 78%] Linking CXX executable dnxr_neural_chip_only_long_session_stability_test
[ 78%] Linking CXX executable dnxr_neural_chip_adversarial_sequence_test
[ 78%] Built target dnxr_neural_chip_only_long_session_stability_test
[ 79%] Building CXX object CMakeFiles/dnxr_neural_chip_hold_commit_policy_test.dir/tests/dnxr_neural_chip_hold_commit_policy_test.cpp.o
[ 79%] Linking CXX executable dnxr_neural_chip_hold_commit_policy_test
[ 79%] Built target dnxr_neural_chip_adversarial_sequence_test
[ 79%] Building CXX object CMakeFiles/dnxr_neural_chip_replay_recorder_integration_test.dir/tests/dnxr_neural_chip_replay_recorder_integration_test.cpp.o
[ 79%] Built target dnxr_neural_chip_hold_commit_policy_test
[ 79%] Building CXX object CMakeFiles/dnxr_full_dive_chip_frame_diff_coverage_test.dir/tests/dnxr_full_dive_chip_frame_diff_coverage_test.cpp.o
[ 79%] Linking CXX executable dnxr_full_dive_chip_frame_diff_coverage_test
[ 79%] Linking CXX executable dnxr_neural_chip_replay_recorder_integration_test
[ 79%] Built target dnxr_full_dive_chip_frame_diff_coverage_test
[ 79%] Building CXX object CMakeFiles/dnxr_per_modality_rate_and_dose_policy_integration_test.dir/tests/dnxr_per_modality_rate_and_dose_policy_integration_test.cpp.o
[ 79%] Built target dnxr_neural_chip_replay_recorder_integration_test
[ 79%] Linking CXX executable dnxr_per_modality_rate_and_dose_policy_integration_test
[ 79%] Building CXX object CMakeFiles/dnxr_neural_chip_session_supervisor_test.dir/tests/dnxr_neural_chip_session_supervisor_test.cpp.o
[ 79%] Linking CXX executable dnxr_neural_chip_session_supervisor_test
[ 79%] Built target dnxr_per_modality_rate_and_dose_policy_integration_test
[ 79%] Building CXX object CMakeFiles/dnxr_neural_chip_emergency_latch_authorized_reset_test.dir/tests/dnxr_neural_chip_emergency_latch_authorized_reset_test.cpp.o
[ 79%] Built target dnxr_neural_chip_session_supervisor_test
[ 79%] Linking CXX executable dnxr_neural_chip_emergency_latch_authorized_reset_test
[ 79%] Building CXX object CMakeFiles/dnxr_neural_chip_ack_handshake_test.dir/tests/dnxr_neural_chip_ack_handshake_test.cpp.o
[ 79%] Linking CXX executable dnxr_neural_chip_ack_handshake_test
[ 79%] Built target dnxr_neural_chip_emergency_latch_authorized_reset_test
[ 79%] Building CXX object CMakeFiles/dnxr_lifecycle_two_phase_commit_test.dir/tests/dnxr_lifecycle_two_phase_commit_test.cpp.o
[ 79%] Built target dnxr_neural_chip_ack_handshake_test
[ 79%] Linking CXX executable dnxr_lifecycle_two_phase_commit_test
[ 80%] Building CXX object CMakeFiles/dnxr_hold_fallback_chain_does_not_poison_lifecycle_test.dir/tests/dnxr_hold_fallback_chain_does_not_poison_lifecycle_test.cpp.o
[ 80%] Built target dnxr_lifecycle_two_phase_commit_test
[ 80%] Building CXX object CMakeFiles/dnxr_ack_pending_state_test.dir/tests/dnxr_ack_pending_state_test.cpp.o
[ 80%] Linking CXX executable dnxr_hold_fallback_chain_does_not_poison_lifecycle_test
[ 80%] Linking CXX executable dnxr_ack_pending_state_test
[ 80%] Built target dnxr_hold_fallback_chain_does_not_poison_lifecycle_test
[ 80%] Built target dnxr_ack_pending_state_test
[ 80%] Building CXX object CMakeFiles/dnxr_hold_reality_anchor_safe_test.dir/tests/dnxr_hold_reality_anchor_safe_test.cpp.o
[ 80%] Building CXX object CMakeFiles/dnxr_world_frame_lifecycle_reject_test.dir/tests/dnxr_world_frame_lifecycle_reject_test.cpp.o
[ 80%] Linking CXX executable dnxr_hold_reality_anchor_safe_test
[ 80%] Built target dnxr_hold_reality_anchor_safe_test
[ 80%] Building CXX object CMakeFiles/dnxr_disabled_modality_full_neutrality_test.dir/tests/dnxr_disabled_modality_full_neutrality_test.cpp.o
[ 80%] Linking CXX executable dnxr_disabled_modality_full_neutrality_test
[ 80%] Built target dnxr_disabled_modality_full_neutrality_test
[ 80%] Building CXX object CMakeFiles/dnxr_sensor_encoder_contract_full_coverage_test.dir/tests/dnxr_sensor_encoder_contract_full_coverage_test.cpp.o
[ 80%] Linking CXX executable dnxr_sensor_encoder_contract_full_coverage_test
[ 80%] Linking CXX executable dnxr_world_frame_lifecycle_reject_test
[ 80%] Built target dnxr_sensor_encoder_contract_full_coverage_test
[ 80%] Built target dnxr_world_frame_lifecycle_reject_test
[ 81%] Building CXX object CMakeFiles/dnxr_pipeline_resume_after_latch_reset_test.dir/tests/dnxr_pipeline_resume_after_latch_reset_test.cpp.o
[ 82%] Building CXX object CMakeFiles/dnxr_replay_recorder_bounded_memory_test.dir/tests/dnxr_replay_recorder_bounded_memory_test.cpp.o
[ 82%] Linking CXX executable dnxr_pipeline_resume_after_latch_reset_test
[ 82%] Built target dnxr_pipeline_resume_after_latch_reset_test
[ 82%] Building CXX object CMakeFiles/dnxr_session_digest_test.dir/tests/dnxr_session_digest_test.cpp.o
[ 82%] Linking CXX executable dnxr_replay_recorder_bounded_memory_test
[ 82%] Built target dnxr_replay_recorder_bounded_memory_test
[ 82%] Linking CXX executable dnxr_session_digest_test
[ 82%] Building CXX object CMakeFiles/dnxr_motor_intent_confidence_gate_test.dir/tests/dnxr_motor_intent_confidence_gate_test.cpp.o
[ 82%] Linking CXX executable dnxr_motor_intent_confidence_gate_test
[ 82%] Built target dnxr_session_digest_test
[ 82%] Building CXX object CMakeFiles/dnxr_avatar_safety_pose_controller_test.dir/tests/dnxr_avatar_safety_pose_controller_test.cpp.o
[ 82%] Built target dnxr_motor_intent_confidence_gate_test
[ 82%] Building CXX object CMakeFiles/dnxr_neural_chip_pipeline_config_validator_test.dir/tests/dnxr_neural_chip_pipeline_config_validator_test.cpp.o
[ 83%] Linking CXX executable dnxr_avatar_safety_pose_controller_test
[ 83%] Built target dnxr_avatar_safety_pose_controller_test
[ 83%] Building CXX object CMakeFiles/dnxr_neural_chip_pipeline_fuzz_session_test.dir/tests/dnxr_neural_chip_pipeline_fuzz_session_test.cpp.o
[ 84%] Linking CXX executable dnxr_neural_chip_pipeline_config_validator_test
[ 84%] Built target dnxr_neural_chip_pipeline_config_validator_test
[ 84%] Building CXX object CMakeFiles/dnxr_neural_chip_ack_external_injection_test.dir/tests/dnxr_neural_chip_ack_external_injection_test.cpp.o
[ 84%] Linking CXX executable dnxr_neural_chip_pipeline_fuzz_session_test
[ 85%] Linking CXX executable dnxr_neural_chip_ack_external_injection_test
[ 85%] Built target dnxr_neural_chip_pipeline_fuzz_session_test
[ 85%] Building CXX object CMakeFiles/dnxr_hold_frame_sequencer_test.dir/tests/dnxr_hold_frame_sequencer_test.cpp.o
[ 85%] Built target dnxr_neural_chip_ack_external_injection_test
[ 85%] Linking CXX executable dnxr_hold_frame_sequencer_test
[ 85%] Building CXX object CMakeFiles/dnxr_neural_chip_hold_lifecycle_test.dir/tests/dnxr_neural_chip_hold_lifecycle_test.cpp.o
[ 85%] Built target dnxr_hold_frame_sequencer_test
[ 85%] Building CXX object CMakeFiles/dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test.dir/tests/dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test.cpp.o
[ 85%] Linking CXX executable dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test
[ 85%] Linking CXX executable dnxr_neural_chip_hold_lifecycle_test
[ 85%] Built target dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test
[ 85%] Built target dnxr_neural_chip_hold_lifecycle_test
[ 85%] Building CXX object CMakeFiles/dnxr_pipeline_resume_controller_test.dir/tests/dnxr_pipeline_resume_controller_test.cpp.o
[ 85%] Building CXX object CMakeFiles/dnxr_emergency_reset_audit_trail_test.dir/tests/dnxr_emergency_reset_audit_trail_test.cpp.o
[ 85%] Linking CXX executable dnxr_pipeline_resume_controller_test
[ 85%] Linking CXX executable dnxr_emergency_reset_audit_trail_test
[ 85%] Built target dnxr_pipeline_resume_controller_test
[ 85%] Building CXX object CMakeFiles/dnxr_neural_chip_replay_recorder_jsonl_test.dir/tests/dnxr_neural_chip_replay_recorder_jsonl_test.cpp.o
[ 85%] Built target dnxr_emergency_reset_audit_trail_test
[ 85%] Building CXX object CMakeFiles/dnxr_neural_chip_replay_player_test.dir/tests/dnxr_neural_chip_replay_player_test.cpp.o
[ 86%] Linking CXX executable dnxr_neural_chip_replay_recorder_jsonl_test
[ 86%] Linking CXX executable dnxr_neural_chip_replay_player_test
[ 86%] Built target dnxr_neural_chip_replay_recorder_jsonl_test
[ 86%] Building CXX object CMakeFiles/dnxr_decision_trace_modality_fieldpath_test.dir/tests/dnxr_decision_trace_modality_fieldpath_test.cpp.o
[ 86%] Built target dnxr_neural_chip_replay_player_test
[ 86%] Building CXX object CMakeFiles/dnxr_decision_trace_coverage_validator_test.dir/tests/dnxr_decision_trace_coverage_validator_test.cpp.o
[ 86%] Linking CXX executable dnxr_decision_trace_modality_fieldpath_test
[ 86%] Linking CXX executable dnxr_decision_trace_coverage_validator_test
[ 86%] Built target dnxr_decision_trace_modality_fieldpath_test
[ 86%] Building CXX object CMakeFiles/dnxr_disabled_modality_neutrality_validator_test.dir/tests/dnxr_disabled_modality_neutrality_validator_test.cpp.o
[ 86%] Built target dnxr_decision_trace_coverage_validator_test
[ 86%] Building CXX object CMakeFiles/dnxr_reality_anchor_degradation_model_test.dir/tests/dnxr_reality_anchor_degradation_model_test.cpp.o
[ 86%] Linking CXX executable dnxr_disabled_modality_neutrality_validator_test
[ 86%] Linking CXX executable dnxr_reality_anchor_degradation_model_test
[ 86%] Built target dnxr_disabled_modality_neutrality_validator_test
[ 86%] Built target dnxr_reality_anchor_degradation_model_test
[ 86%] Building CXX object CMakeFiles/dnxr_world_frame_sanitizer_test.dir/tests/dnxr_world_frame_sanitizer_test.cpp.o
[ 86%] Building CXX object CMakeFiles/dnxr_sensor_encoder_output_contract_test.dir/tests/dnxr_sensor_encoder_output_contract_test.cpp.o
[ 86%] Linking CXX executable dnxr_sensor_encoder_output_contract_test
[ 86%] Built target dnxr_sensor_encoder_output_contract_test
[ 86%] Building CXX object CMakeFiles/dnxr_neural_chip_pipeline_state_machine_test.dir/tests/dnxr_neural_chip_pipeline_state_machine_test.cpp.o
[ 86%] Linking CXX executable dnxr_world_frame_sanitizer_test
[ 86%] Built target dnxr_world_frame_sanitizer_test
[ 86%] Building CXX object CMakeFiles/dnxr_pipeline_state_machine_ack_timeout_sequence_test.dir/tests/dnxr_pipeline_state_machine_ack_timeout_sequence_test.cpp.o
[ 86%] Linking CXX executable dnxr_neural_chip_pipeline_state_machine_test
[ 86%] Built target dnxr_neural_chip_pipeline_state_machine_test
[ 86%] Building CXX object CMakeFiles/dnxr_pipeline_config_loader_parser_result_test.dir/tests/dnxr_pipeline_config_loader_parser_result_test.cpp.o
[ 86%] Linking CXX executable dnxr_pipeline_state_machine_ack_timeout_sequence_test
[ 86%] Built target dnxr_pipeline_state_machine_ack_timeout_sequence_test
[ 87%] Linking CXX executable dnxr_pipeline_config_loader_parser_result_test
[ 87%] Building CXX object CMakeFiles/dnxr_decision_trace_path_aware_coverage_test.dir/tests/dnxr_decision_trace_path_aware_coverage_test.cpp.o
[ 87%] Built target dnxr_pipeline_config_loader_parser_result_test
[ 87%] Building CXX object CMakeFiles/dnxr_emergency_latch_controller_audit_test.dir/tests/dnxr_emergency_latch_controller_audit_test.cpp.o
[ 87%] Linking CXX executable dnxr_decision_trace_path_aware_coverage_test
[ 88%] Linking CXX executable dnxr_emergency_latch_controller_audit_test
[ 88%] Built target dnxr_decision_trace_path_aware_coverage_test
[ 88%] Building CXX object CMakeFiles/dnxr_pipeline_session_digest_integration_test.dir/tests/dnxr_pipeline_session_digest_integration_test.cpp.o
[ 88%] Built target dnxr_emergency_latch_controller_audit_test
[ 88%] Building CXX object CMakeFiles/dnxr_safety_invariant_engine_test.dir/tests/dnxr_safety_invariant_engine_test.cpp.o
[ 88%] Linking CXX executable dnxr_safety_invariant_engine_test
[ 88%] Built target dnxr_safety_invariant_engine_test
[ 88%] Building CXX object CMakeFiles/dnxr_sanitize_failure_frame_factory_test.dir/tests/dnxr_sanitize_failure_frame_factory_test.cpp.o
[ 88%] Linking CXX executable dnxr_pipeline_session_digest_integration_test
[ 88%] Built target dnxr_pipeline_session_digest_integration_test
[ 88%] Building CXX object CMakeFiles/dnxr_neural_chip_timebase_lifecycle_ack_test.dir/tests/dnxr_neural_chip_timebase_lifecycle_ack_test.cpp.o
[ 88%] Linking CXX executable dnxr_neural_chip_timebase_lifecycle_ack_test
[ 88%] Linking CXX executable dnxr_sanitize_failure_frame_factory_test
[ 88%] Built target dnxr_neural_chip_timebase_lifecycle_ack_test
[ 88%] Built target dnxr_sanitize_failure_frame_factory_test
[ 89%] Building CXX object CMakeFiles/dnxr_neural_vector_field_registry_single_source_test.dir/tests/dnxr_neural_vector_field_registry_single_source_test.cpp.o
[ 89%] Building CXX object CMakeFiles/dnxr_pipeline_state_gate_test.dir/tests/dnxr_pipeline_state_gate_test.cpp.o
[ 89%] Linking CXX executable dnxr_neural_vector_field_registry_single_source_test
[ 89%] Linking CXX executable dnxr_pipeline_state_gate_test
[ 89%] Built target dnxr_neural_vector_field_registry_single_source_test
[ 89%] Building CXX object CMakeFiles/dnxr_ack_early_gate_before_encoders_test.dir/tests/dnxr_ack_early_gate_before_encoders_test.cpp.o
[ 89%] Built target dnxr_pipeline_state_gate_test
[ 89%] Building CXX object CMakeFiles/dnxr_reject_with_hold_path_kind_test.dir/tests/dnxr_reject_with_hold_path_kind_test.cpp.o
[ 89%] Linking CXX executable dnxr_ack_early_gate_before_encoders_test
[ 89%] Linking CXX executable dnxr_reject_with_hold_path_kind_test
[ 89%] Built target dnxr_ack_early_gate_before_encoders_test
[ 89%] Built target dnxr_reject_with_hold_path_kind_test
[ 89%] Building CXX object CMakeFiles/dnxr_pipeline_path_classifier_test.dir/tests/dnxr_pipeline_path_classifier_test.cpp.o
[ 89%] Building CXX object CMakeFiles/dnxr_decision_trace_coverage_pipeline_integration_test.dir/tests/dnxr_decision_trace_coverage_pipeline_integration_test.cpp.o
[ 89%] Linking CXX executable dnxr_pipeline_path_classifier_test
[ 89%] Built target dnxr_pipeline_path_classifier_test
[ 89%] Building CXX object CMakeFiles/dnxr_real_readiness_gate_allows_hardware_bench_no_human_test.dir/tests/real/dnxr_real_readiness_gate_allows_hardware_bench_no_human_test.cpp.o
[ 89%] Linking CXX executable dnxr_decision_trace_coverage_pipeline_integration_test
[ 89%] Linking CXX executable dnxr_real_readiness_gate_allows_hardware_bench_no_human_test
[ 89%] Built target dnxr_decision_trace_coverage_pipeline_integration_test
[ 89%] Building CXX object CMakeFiles/dnxr_real_readiness_gate_blocks_human_without_authorization_test.dir/tests/real/dnxr_real_readiness_gate_blocks_human_without_authorization_test.cpp.o
[ 89%] Built target dnxr_real_readiness_gate_allows_hardware_bench_no_human_test
[ 89%] Building CXX object CMakeFiles/dnxr_hardware_bench_requires_dummy_load_test.dir/tests/hardware_bench/dnxr_hardware_bench_requires_dummy_load_test.cpp.o
[ 89%] Linking CXX executable dnxr_real_readiness_gate_blocks_human_without_authorization_test
[ 89%] Built target dnxr_real_readiness_gate_blocks_human_without_authorization_test
[ 89%] Building CXX object CMakeFiles/dnxr_hardware_bench_requires_emergency_stop_test.dir/tests/hardware_bench/dnxr_hardware_bench_requires_emergency_stop_test.cpp.o
[ 90%] Linking CXX executable dnxr_hardware_bench_requires_dummy_load_test
[ 90%] Built target dnxr_hardware_bench_requires_dummy_load_test
[ 90%] Linking CXX executable dnxr_hardware_bench_requires_emergency_stop_test
[ 90%] Building CXX object CMakeFiles/dnxr_hardware_bench_requires_trace_recorder_test.dir/tests/hardware_bench/dnxr_hardware_bench_requires_trace_recorder_test.cpp.o
[ 90%] Built target dnxr_hardware_bench_requires_emergency_stop_test
[ 90%] Building CXX object CMakeFiles/dnxr_real_hal_rejects_raw_sensory_payload_compile_test.dir/tests/real/dnxr_real_hal_rejects_raw_sensory_payload_compile_test.cpp.o
[ 90%] Linking CXX executable dnxr_hardware_bench_requires_trace_recorder_test
[ 90%] Built target dnxr_hardware_bench_requires_trace_recorder_test
[ 91%] Linking CXX executable dnxr_real_hal_rejects_raw_sensory_payload_compile_test
[ 91%] Building CXX object CMakeFiles/dnxr_real_hal_accepts_bounded_stimulus_command_test.dir/tests/real/dnxr_real_hal_accepts_bounded_stimulus_command_test.cpp.o
[ 91%] Built target dnxr_real_hal_rejects_raw_sensory_payload_compile_test
[ 91%] Building CXX object CMakeFiles/dnxr_hardware_bench_dry_run_sends_no_active_output_test.dir/tests/hardware_bench/dnxr_hardware_bench_dry_run_sends_no_active_output_test.cpp.o
[ 91%] Linking CXX executable dnxr_real_hal_accepts_bounded_stimulus_command_test
[ 91%] Built target dnxr_real_hal_accepts_bounded_stimulus_command_test
[ 91%] Linking CXX executable dnxr_hardware_bench_dry_run_sends_no_active_output_test
[ 91%] Building CXX object CMakeFiles/dnxr_emergency_latch_controller_pipeline_integration_test.dir/tests/dnxr_emergency_latch_controller_pipeline_integration_test.cpp.o
[ 91%] Built target dnxr_hardware_bench_dry_run_sends_no_active_output_test
[ 91%] Building CXX object CMakeFiles/dnxr_hold_stability_tracker_test.dir/tests/dnxr_hold_stability_tracker_test.cpp.o
[ 91%] Linking CXX executable dnxr_emergency_latch_controller_pipeline_integration_test
[ 91%] Linking CXX executable dnxr_hold_stability_tracker_test
[ 91%] Built target dnxr_emergency_latch_controller_pipeline_integration_test
[ 91%] Built target dnxr_hold_stability_tracker_test
[ 91%] Building CXX object CMakeFiles/dnxr_pipeline_resume_state_bridge_test.dir/tests/dnxr_pipeline_resume_state_bridge_test.cpp.o
[ 91%] Building CXX object CMakeFiles/dnxr_replay_schema_version_test.dir/tests/dnxr_replay_schema_version_test.cpp.o
[ 91%] Linking CXX executable dnxr_pipeline_resume_state_bridge_test
[ 91%] Built target dnxr_pipeline_resume_state_bridge_test
[ 91%] Building CXX object CMakeFiles/dnxr_replay_consistency_verifier_test.dir/tests/dnxr_replay_consistency_verifier_test.cpp.o
[ 91%] Linking CXX executable dnxr_replay_schema_version_test
[ 91%] Built target dnxr_replay_schema_version_test
[ 91%] Building CXX object CMakeFiles/dnxr_pipeline_safety_case_report_test.dir/tests/dnxr_pipeline_safety_case_report_test.cpp.o
[ 91%] Linking CXX executable dnxr_replay_consistency_verifier_test
[ 91%] Built target dnxr_replay_consistency_verifier_test
[ 91%] Building CXX object CMakeFiles/dnxr_pipeline_state_machine_runtime_invariant_test.dir/tests/dnxr_pipeline_state_machine_runtime_invariant_test.cpp.o
[ 91%] Linking CXX executable dnxr_pipeline_safety_case_report_test
[ 91%] Built target dnxr_pipeline_safety_case_report_test
[ 91%] Building CXX object CMakeFiles/dnxr_pipeline_no_auto_ack_recovery_test.dir/tests/dnxr_pipeline_no_auto_ack_recovery_test.cpp.o
[ 91%] Linking CXX executable dnxr_pipeline_state_machine_runtime_invariant_test
[ 91%] Built target dnxr_pipeline_state_machine_runtime_invariant_test
[ 91%] Linking CXX executable dnxr_pipeline_no_auto_ack_recovery_test
[ 91%] Building CXX object CMakeFiles/dnxr_timebase_advances_on_repeated_world_timestamp_test.dir/tests/dnxr_timebase_advances_on_repeated_world_timestamp_test.cpp.o
[ 91%] Built target dnxr_pipeline_no_auto_ack_recovery_test
[ 92%] Building CXX object CMakeFiles/dnxr_pipeline_shutdown_no_normal_frames_test.dir/tests/dnxr_pipeline_shutdown_no_normal_frames_test.cpp.o
[ 92%] Linking CXX executable dnxr_timebase_advances_on_repeated_world_timestamp_test
[ 92%] Built target dnxr_timebase_advances_on_repeated_world_timestamp_test
[ 92%] Linking CXX executable dnxr_pipeline_shutdown_no_normal_frames_test
[ 92%] Building CXX object CMakeFiles/dnxr_state_machine_transition_matrix_test.dir/tests/dnxr_state_machine_transition_matrix_test.cpp.o
[ 92%] Built target dnxr_pipeline_shutdown_no_normal_frames_test
[ 92%] Building CXX object CMakeFiles/dnxr_strict_trace_coverage_blocks_incomplete_trace_test.dir/tests/dnxr_strict_trace_coverage_blocks_incomplete_trace_test.cpp.o
[ 92%] Linking CXX executable dnxr_state_machine_transition_matrix_test
[ 92%] Built target dnxr_state_machine_transition_matrix_test
[ 92%] Building CXX object CMakeFiles/dnxr_frame_commit_result_bus_submitted_test.dir/tests/dnxr_frame_commit_result_bus_submitted_test.cpp.o
[ 92%] Linking CXX executable dnxr_strict_trace_coverage_blocks_incomplete_trace_test
[ 92%] Linking CXX executable dnxr_frame_commit_result_bus_submitted_test
[ 92%] Built target dnxr_strict_trace_coverage_blocks_incomplete_trace_test
[ 92%] Building CXX object CMakeFiles/dnxr_emergency_transition_requires_audited_reset_test.dir/tests/dnxr_emergency_transition_requires_audited_reset_test.cpp.o
[ 92%] Built target dnxr_frame_commit_result_bus_submitted_test
[ 92%] Building CXX object CMakeFiles/dnxr_step_result_consistency_verifier_test.dir/tests/dnxr_step_result_consistency_verifier_test.cpp.o
[ 92%] Linking CXX executable dnxr_emergency_transition_requires_audited_reset_test
[ 93%] Linking CXX executable dnxr_step_result_consistency_verifier_test
[ 93%] Built target dnxr_emergency_transition_requires_audited_reset_test
[ 93%] Building CXX object CMakeFiles/dnxr_pipeline_reject_code_classifier_test.dir/tests/dnxr_pipeline_reject_code_classifier_test.cpp.o
[ 93%] Built target dnxr_step_result_consistency_verifier_test
[ 93%] Building CXX object CMakeFiles/dnxr_neural_chip_stateful_long_session_test.dir/tests/dnxr_neural_chip_stateful_long_session_test.cpp.o
[ 93%] Linking CXX executable dnxr_pipeline_reject_code_classifier_test
[ 93%] Built target dnxr_pipeline_reject_code_classifier_test
[ 93%] Building CXX object CMakeFiles/dnxr_human_subject_use_gate_test.dir/tests/dnxr_human_subject_use_gate_test.cpp.o
[ 93%] Linking CXX executable dnxr_neural_chip_stateful_long_session_test
[ 93%] Linking CXX executable dnxr_human_subject_use_gate_test
[ 93%] Built target dnxr_neural_chip_stateful_long_session_test
[ 93%] Building CXX object CMakeFiles/dnxr_release_mode_lock_test.dir/tests/dnxr_release_mode_lock_test.cpp.o
[ 93%] Built target dnxr_human_subject_use_gate_test
[ 93%] Building CXX object CMakeFiles/dnxr_medical_review_gate_test.dir/tests/dnxr_medical_review_gate_test.cpp.o
[ 93%] Linking CXX executable dnxr_release_mode_lock_test
[ 93%] Linking CXX executable dnxr_medical_review_gate_test
[ 93%] Built target dnxr_release_mode_lock_test
[ 93%] Building CXX object CMakeFiles/dnxr_regulatory_authorization_evidence_gate_test.dir/tests/dnxr_regulatory_authorization_evidence_gate_test.cpp.o
[ 93%] Built target dnxr_medical_review_gate_test
[ 93%] Building CXX object CMakeFiles/dnxr_hardware_bench_readiness_gate_test.dir/tests/dnxr_hardware_bench_readiness_gate_test.cpp.o
[ 93%] Linking CXX executable dnxr_regulatory_authorization_evidence_gate_test
[ 93%] Linking CXX executable dnxr_hardware_bench_readiness_gate_test
[ 93%] Built target dnxr_regulatory_authorization_evidence_gate_test
[ 93%] Building CXX object CMakeFiles/dnxr_p95_p99_acceptance_gate_test.dir/tests/dnxr_p95_p99_acceptance_gate_test.cpp.o
[ 93%] Built target dnxr_hardware_bench_readiness_gate_test
[ 93%] Building CXX object CMakeFiles/dnxr_performance_telemetry_recorder_test.dir/tests/dnxr_performance_telemetry_recorder_test.cpp.o
[ 94%] Linking CXX executable dnxr_p95_p99_acceptance_gate_test
[ 94%] Built target dnxr_p95_p99_acceptance_gate_test
[ 94%] Linking CXX executable dnxr_performance_telemetry_recorder_test
[ 94%] Building CXX object CMakeFiles/dnxr_hardware_bench_session_runner_test.dir/tests/dnxr_hardware_bench_session_runner_test.cpp.o
[ 94%] Built target dnxr_performance_telemetry_recorder_test
[ 94%] Building CXX object CMakeFiles/dnxr_bench_only_neural_hardware_adapter_test.dir/tests/dnxr_bench_only_neural_hardware_adapter_test.cpp.o
[ 94%] Linking CXX executable dnxr_hardware_bench_session_runner_test
[ 94%] Linking CXX executable dnxr_bench_only_neural_hardware_adapter_test
[ 94%] Built target dnxr_hardware_bench_session_runner_test
[ 94%] Building CXX object CMakeFiles/dnxr_traceability_coverage_gate_test.dir/tests/dnxr_traceability_coverage_gate_test.cpp.o
[ 94%] Built target dnxr_bench_only_neural_hardware_adapter_test
[ 95%] Building CXX object CMakeFiles/dnxr_release_readiness_gate_test.dir/tests/dnxr_release_readiness_gate_test.cpp.o
[ 95%] Linking CXX executable dnxr_traceability_coverage_gate_test
[ 95%] Linking CXX executable dnxr_release_readiness_gate_test
[ 95%] Built target dnxr_traceability_coverage_gate_test
[ 95%] Built target dnxr_release_readiness_gate_test
[ 96%] Building CXX object CMakeFiles/dnxr_signed_evidence_manifest_test.dir/tests/dnxr_signed_evidence_manifest_test.cpp.o
[ 96%] Building CXX object CMakeFiles/dnxr_gustatory_discrete_stimulation_test.dir/tests/full_dive/dnxr_gustatory_discrete_stimulation_test.cpp.o
[ 97%] Linking CXX executable dnxr_gustatory_discrete_stimulation_test
[ 97%] Built target dnxr_gustatory_discrete_stimulation_test
[ 97%] Building CXX object CMakeFiles/dnxr_haptic_proprioception_mismatch_test.dir/tests/full_dive/dnxr_haptic_proprioception_mismatch_test.cpp.o
[ 97%] Linking CXX executable dnxr_signed_evidence_manifest_test
[ 97%] Built target dnxr_signed_evidence_manifest_test
[ 97%] Building CXX object CMakeFiles/dnxr_no_pain_mapping_test.dir/tests/full_dive/dnxr_no_pain_mapping_test.cpp.o
[ 97%] Linking CXX executable dnxr_haptic_proprioception_mismatch_test
[ 97%] Built target dnxr_haptic_proprioception_mismatch_test
[ 97%] Building CXX object CMakeFiles/dnxr_olfactory_persistence_budget_test.dir/tests/full_dive/dnxr_olfactory_persistence_budget_test.cpp.o
[ 97%] Linking CXX executable dnxr_no_pain_mapping_test
[ 97%] Built target dnxr_no_pain_mapping_test
[ 97%] Building CXX object CMakeFiles/dnxr_thermal_burn_lockout_test.dir/tests/full_dive/dnxr_thermal_burn_lockout_test.cpp.o
[ 97%] Linking CXX executable dnxr_olfactory_persistence_budget_test
[ 97%] Built target dnxr_olfactory_persistence_budget_test
[ 97%] Linking CXX executable dnxr_thermal_burn_lockout_test
[ 97%] Building CXX object CMakeFiles/dnxr_visual_audio_phase_lock_test.dir/tests/full_dive/dnxr_visual_audio_phase_lock_test.cpp.o
[ 97%] Built target dnxr_thermal_burn_lockout_test
[ 97%] Building CXX object CMakeFiles/dnxr_visual_vestibular_conflict_blocks_motion_test.dir/tests/full_dive/dnxr_visual_vestibular_conflict_blocks_motion_test.cpp.o
[ 97%] Linking CXX executable dnxr_visual_audio_phase_lock_test
[ 97%] Built target dnxr_visual_audio_phase_lock_test
[ 97%] Linking CXX executable dnxr_visual_vestibular_conflict_blocks_motion_test
[ 97%] Building CXX object CMakeFiles/dnxr_neural_chip_ack_timeout_test.dir/tests/hardware/dnxr_neural_chip_ack_timeout_test.cpp.o
[ 97%] Built target dnxr_visual_vestibular_conflict_blocks_motion_test
[ 97%] Building CXX object CMakeFiles/dnxr_neural_chip_capability_mismatch_test.dir/tests/hardware/dnxr_neural_chip_capability_mismatch_test.cpp.o
[ 97%] Linking CXX executable dnxr_neural_chip_ack_timeout_test
[ 97%] Built target dnxr_neural_chip_ack_timeout_test
[ 97%] Linking CXX executable dnxr_neural_chip_capability_mismatch_test
[ 97%] Building CXX object CMakeFiles/dnxr_neural_chip_feedback_required_test.dir/tests/hardware/dnxr_neural_chip_feedback_required_test.cpp.o
[ 97%] Built target dnxr_neural_chip_capability_mismatch_test
[ 97%] Building CXX object CMakeFiles/dnxr_neural_chip_firmware_mismatch_test.dir/tests/hardware/dnxr_neural_chip_firmware_mismatch_test.cpp.o
[ 97%] Linking CXX executable dnxr_neural_chip_feedback_required_test
[ 97%] Built target dnxr_neural_chip_feedback_required_test
[ 97%] Linking CXX executable dnxr_neural_chip_firmware_mismatch_test
[ 97%] Building CXX object CMakeFiles/dnxr_neural_chip_packet_hash_chain_test.dir/tests/hardware/dnxr_neural_chip_packet_hash_chain_test.cpp.o
[ 97%] Built target dnxr_neural_chip_firmware_mismatch_test
[ 98%] Building CXX object CMakeFiles/dnxr_neural_chip_temperature_cutoff_test.dir/tests/hardware/dnxr_neural_chip_temperature_cutoff_test.cpp.o
[ 98%] Linking CXX executable dnxr_neural_chip_packet_hash_chain_test
[ 98%] Built target dnxr_neural_chip_packet_hash_chain_test
[ 98%] Linking CXX executable dnxr_neural_chip_temperature_cutoff_test
[ 98%] Building CXX object CMakeFiles/dnxr_decompression_state_machine_test.dir/tests/safety/dnxr_decompression_state_machine_test.cpp.o
[ 98%] Built target dnxr_neural_chip_temperature_cutoff_test
[ 98%] Building CXX object CMakeFiles/dnxr_dissociation_reality_anchor_test.dir/tests/safety/dnxr_dissociation_reality_anchor_test.cpp.o
[ 99%] Linking CXX executable dnxr_decompression_state_machine_test
[ 99%] Built target dnxr_decompression_state_machine_test
[ 99%] Linking CXX executable dnxr_dissociation_reality_anchor_test
[ 99%] Building CXX object CMakeFiles/dnxr_flash_pattern_visual_block_test.dir/tests/safety/dnxr_flash_pattern_visual_block_test.cpp.o
[ 99%] Built target dnxr_dissociation_reality_anchor_test
[ 99%] Building CXX object CMakeFiles/dnxr_locked_in_channel_loss_test.dir/tests/safety/dnxr_locked_in_channel_loss_test.cpp.o
[ 99%] Linking CXX executable dnxr_flash_pattern_visual_block_test
[ 99%] Built target dnxr_flash_pattern_visual_block_test
[100%] Linking CXX executable dnxr_locked_in_channel_loss_test
[100%] Building CXX object CMakeFiles/dnxr_pain_perception_emergency_stop_test.dir/tests/safety/dnxr_pain_perception_emergency_stop_test.cpp.o
[100%] Built target dnxr_locked_in_channel_loss_test
[100%] Building CXX object CMakeFiles/dnxr_vestibular_hard_limit_test.dir/tests/safety/dnxr_vestibular_hard_limit_test.cpp.o
[100%] Linking CXX executable dnxr_pain_perception_emergency_stop_test
[100%] Built target dnxr_pain_perception_emergency_stop_test
[100%] Linking CXX executable dnxr_vestibular_hard_limit_test
[100%] Built target dnxr_vestibular_hard_limit_test
```

## CTest log
```text
Internal ctest changing into directory: /tmp/neuro_build_ubsan
Test project /tmp/neuro_build_ubsan
        Start   1: runtime_mode_test
  1/283 Test   #1: runtime_mode_test .......................................................   Passed    0.01 sec
        Start   2: full_unlocked_runtime_test
  2/283 Test   #2: full_unlocked_runtime_test ..............................................   Passed    0.01 sec
        Start   3: pipeline_state_machine_test
  3/283 Test   #3: pipeline_state_machine_test .............................................   Passed    0.01 sec
        Start   4: replay_consistency_test
  4/283 Test   #4: replay_consistency_test .................................................   Passed    0.01 sec
        Start   5: performance_smoke_test
  5/283 Test   #5: performance_smoke_test ..................................................   Passed    0.01 sec
        Start   6: safety_decision_test
  6/283 Test   #6: safety_decision_test ....................................................   Passed    0.01 sec
        Start   7: timebase_sync_test
  7/283 Test   #7: timebase_sync_test ......................................................   Passed    0.01 sec
        Start   8: dnxr_state_smoke_test
  8/283 Test   #8: dnxr_state_smoke_test ...................................................   Passed    0.01 sec
        Start   9: dnxr_config_loader_test
  9/283 Test   #9: dnxr_config_loader_test .................................................   Passed    0.01 sec
        Start  10: dnxr_json_parser_test
 10/283 Test  #10: dnxr_json_parser_test ...................................................   Passed    0.01 sec
        Start  11: dnxr_calibration_drift_monitor_test
 11/283 Test  #11: dnxr_calibration_drift_monitor_test .....................................   Passed    0.01 sec
        Start  12: dnxr_save_asset_manager_test
 12/283 Test  #12: dnxr_save_asset_manager_test ............................................   Passed    0.04 sec
        Start  13: dnxr_plugin_registry_test
 13/283 Test  #13: dnxr_plugin_registry_test ...............................................   Passed    0.01 sec
        Start  14: dnxr_signal_unit_test
 14/283 Test  #14: dnxr_signal_unit_test ...................................................   Passed    0.01 sec
        Start  15: dnxr_file_replay_signal_provider_test
 15/283 Test  #15: dnxr_file_replay_signal_provider_test ...................................   Passed    0.01 sec
        Start  16: dnxr_signal_dropout_test
 16/283 Test  #16: dnxr_signal_dropout_test ................................................   Passed    0.01 sec
        Start  17: dnxr_cognition_unit_test
 17/283 Test  #17: dnxr_cognition_unit_test ................................................   Passed    0.01 sec
        Start  18: dnxr_safety_unit_test
 18/283 Test  #18: dnxr_safety_unit_test ...................................................   Passed    0.01 sec
        Start  19: dnxr_safety_state_machine_test
 19/283 Test  #19: dnxr_safety_state_machine_test ..........................................   Passed    0.01 sec
        Start  20: dnxr_watchdog_failure_test
 20/283 Test  #20: dnxr_watchdog_failure_test ..............................................   Passed    0.01 sec
        Start  21: dnxr_safety_profile_loader_test
 21/283 Test  #21: dnxr_safety_profile_loader_test .........................................   Passed    0.01 sec
        Start  22: dnxr_simulation_only_guard_test
 22/283 Test  #22: dnxr_simulation_only_guard_test .........................................   Passed    0.01 sec
        Start  23: dnxr_secret_provider_test
 23/283 Test  #23: dnxr_secret_provider_test ...............................................   Passed    0.01 sec
        Start  24: dnxr_safety_pipeline_test
 24/283 Test  #24: dnxr_safety_pipeline_test ...............................................   Passed    0.01 sec
        Start  25: dnxr_reality_anchor_system_test
 25/283 Test  #25: dnxr_reality_anchor_system_test .........................................   Passed    0.01 sec
        Start  26: dnxr_multisensory_clock_test
 26/283 Test  #26: dnxr_multisensory_clock_test ............................................   Passed    0.01 sec
        Start  27: dnxr_intent_risk_classifier_test
 27/283 Test  #27: dnxr_intent_risk_classifier_test ........................................   Passed    0.01 sec
        Start  28: dnxr_neurorights_policy_test
 28/283 Test  #28: dnxr_neurorights_policy_test ............................................   Passed    0.01 sec
        Start  29: dnxr_safety_fuzz_test
 29/283 Test  #29: dnxr_safety_fuzz_test ...................................................   Passed    0.05 sec
        Start  30: dnxr_renderer_qos_stress_test
 30/283 Test  #30: dnxr_renderer_qos_stress_test ...........................................   Passed    0.02 sec
        Start  31: dnxr_pipeline_integration_test
 31/283 Test  #31: dnxr_pipeline_integration_test ..........................................   Passed    0.01 sec
        Start  32: dnxr_neurotech_unit_test
 32/283 Test  #32: dnxr_neurotech_unit_test ................................................   Passed    0.01 sec
        Start  33: dnxr_neuro_sdk_bridge_test
 33/283 Test  #33: dnxr_neuro_sdk_bridge_test ..............................................   Passed    0.01 sec
        Start  34: dnxr_runtime_realtime_test
 34/283 Test  #34: dnxr_runtime_realtime_test ..............................................   Passed    0.02 sec
        Start  35: dnxr_runtime_10k_cycles_test
 35/283 Test  #35: dnxr_runtime_10k_cycles_test ............................................   Passed    8.38 sec
        Start  36: dnxr_runtime_types_test
 36/283 Test  #36: dnxr_runtime_types_test .................................................   Passed    0.01 sec
        Start  37: dnxr_runtime_trace_recorder_test
 37/283 Test  #37: dnxr_runtime_trace_recorder_test ........................................   Passed    0.01 sec
        Start  38: dnxr_logger_sink_test
 38/283 Test  #38: dnxr_logger_sink_test ...................................................   Passed    0.01 sec
        Start  39: dnxr_replay_determinism_test
 39/283 Test  #39: dnxr_replay_determinism_test ............................................   Passed    0.02 sec
        Start  40: dnxr_security_foundation_test
 40/283 Test  #40: dnxr_security_foundation_test ...........................................   Passed    0.01 sec
        Start  41: dnxr_network_replay_attack_test
 41/283 Test  #41: dnxr_network_replay_attack_test .........................................   Passed    0.01 sec
        Start  42: dnxr_neural_chip_hal_test
 42/283 Test  #42: dnxr_neural_chip_hal_test ...............................................   Passed    0.01 sec
        Start  43: dnxr_vestibular_budget_boundary_test
 43/283 Test  #43: dnxr_vestibular_budget_boundary_test ....................................   Passed    0.01 sec
        Start  44: dnxr_sensory_frame_schema_test
 44/283 Test  #44: dnxr_sensory_frame_schema_test ..........................................   Passed    0.01 sec
        Start  45: dnxr_vestibular_exposure_budget_test
 45/283 Test  #45: dnxr_vestibular_exposure_budget_test ....................................   Passed    0.01 sec
        Start  46: dnxr_full_dive_bridge_intervention_test
 46/283 Test  #46: dnxr_full_dive_bridge_intervention_test .................................   Passed    0.36 sec
        Start  47: dnxr_full_dive_intervention_window_test
 47/283 Test  #47: dnxr_full_dive_intervention_window_test .................................   Passed    0.35 sec
        Start  48: dnxr_stimulus_envelope_concurrency_test
 48/283 Test  #48: dnxr_stimulus_envelope_concurrency_test .................................   Passed    0.03 sec
        Start  49: dnxr_vestibular_exposure_budget_concurrency_test
 49/283 Test  #49: dnxr_vestibular_exposure_budget_concurrency_test ........................   Passed    0.02 sec
        Start  50: dnxr_safety_state_machine_concurrency_test
 50/283 Test  #50: dnxr_safety_state_machine_concurrency_test ..............................   Passed    0.01 sec
        Start  51: dnxr_open_interop_protocol_test
 51/283 Test  #51: dnxr_open_interop_protocol_test .........................................   Passed    0.01 sec
        Start  52: dnxr_neural_intent_protocol_test
 52/283 Test  #52: dnxr_neural_intent_protocol_test ........................................   Passed    0.01 sec
        Start  53: dnxr_runtime_acquisition_stage_test
 53/283 Test  #53: dnxr_runtime_acquisition_stage_test .....................................   Passed    0.01 sec
        Start  54: dnxr_runtime_inference_stage_test
 54/283 Test  #54: dnxr_runtime_inference_stage_test .......................................   Passed    0.01 sec
        Start  55: dnxr_runtime_calibration_stage_test
 55/283 Test  #55: dnxr_runtime_calibration_stage_test .....................................   Passed    0.01 sec
        Start  56: dnxr_runtime_immersion_frame_stage_test
 56/283 Test  #56: dnxr_runtime_immersion_frame_stage_test .................................   Passed    0.01 sec
        Start  57: dnxr_runtime_cycle_finalizer_test
 57/283 Test  #57: dnxr_runtime_cycle_finalizer_test .......................................   Passed    0.01 sec
        Start  58: dnxr_runtime_callback_ref_test
 58/283 Test  #58: dnxr_runtime_callback_ref_test ..........................................   Passed    0.01 sec
        Start  59: dnxr_runtime_pipeline_runner_test
 59/283 Test  #59: dnxr_runtime_pipeline_runner_test .......................................   Passed    0.03 sec
        Start  60: dnxr_runtime_pipeline_services_only_test
 60/283 Test  #60: dnxr_runtime_pipeline_services_only_test ................................   Passed    0.66 sec
        Start  61: dnxr_runtime_pipeline_sink_refs_test
 61/283 Test  #61: dnxr_runtime_pipeline_sink_refs_test ....................................   Passed    0.01 sec
        Start  62: dnxr_runtime_pipeline_run_input_ref_test
 62/283 Test  #62: dnxr_runtime_pipeline_run_input_ref_test ................................   Passed    0.01 sec
        Start  63: dnxr_runtime_pipeline_legacy_input_test
 63/283 Test  #63: dnxr_runtime_pipeline_legacy_input_test .................................   Passed    0.01 sec
        Start  64: dnxr_runtime_controller_pipeline_smoke_test
 64/283 Test  #64: dnxr_runtime_controller_pipeline_smoke_test .............................   Passed    2.36 sec
        Start  65: dnxr_runtime_controller_no_allocation_smoke_test
 65/283 Test  #65: dnxr_runtime_controller_no_allocation_smoke_test ........................   Passed    0.01 sec
        Start  66: dnxr_runtime_controller_pipeline_10k_cycles_test
 66/283 Test  #66: dnxr_runtime_controller_pipeline_10k_cycles_test ........................***Not Run (Disabled)   0.00 sec
        Start  67: dnxr_runtime_controller_pipeline_memory_growth_test
 67/283 Test  #67: dnxr_runtime_controller_pipeline_memory_growth_test .....................   Passed    3.25 sec
        Start  68: dnxr_runtime_controller_jitter_budget_test
 68/283 Test  #68: dnxr_runtime_controller_jitter_budget_test ..............................   Passed    2.27 sec
        Start  69: dnxr_runtime_safety_stage_test
 69/283 Test  #69: dnxr_runtime_safety_stage_test ..........................................   Passed    0.01 sec
        Start  70: dnxr_runtime_full_dive_stage_test
 70/283 Test  #70: dnxr_runtime_full_dive_stage_test .......................................   Passed    0.02 sec
        Start  71: dnxr_runtime_render_stage_test
 71/283 Test  #71: dnxr_runtime_render_stage_test ..........................................   Passed    0.02 sec
        Start  72: dnxr_runtime_stimulus_stage_test
 72/283 Test  #72: dnxr_runtime_stimulus_stage_test ........................................   Passed    0.01 sec
        Start  73: dnxr_runtime_allocation_guard_test
 73/283 Test  #73: dnxr_runtime_allocation_guard_test ......................................   Passed    0.01 sec
        Start  74: dnxr_runtime_memory_growth_test
 74/283 Test  #74: dnxr_runtime_memory_growth_test .........................................   Passed    0.08 sec
        Start  75: dnxr_runtime_pipeline_100k_cycles_test
 75/283 Test  #75: dnxr_runtime_pipeline_100k_cycles_test ..................................   Passed   16.15 sec
        Start  76: dnxr_runtime_pipeline_memory_growth_test
 76/283 Test  #76: dnxr_runtime_pipeline_memory_growth_test ................................   Passed    3.62 sec
        Start  77: dnxr_runtime_stage_jitter_simulation_test
 77/283 Test  #77: dnxr_runtime_stage_jitter_simulation_test ...............................   Passed    0.01 sec
        Start  78: dnxr_runtime_1h_stability_test
 78/283 Test  #78: dnxr_runtime_1h_stability_test ..........................................   Passed    0.44 sec
        Start  79: dnxr_vestibular_exposure_budget_long_test
 79/283 Test  #79: dnxr_vestibular_exposure_budget_long_test ...............................***Not Run (Disabled)   0.00 sec
        Start  80: dnxr_full_dive_intervention_window_long_test
 80/283 Test  #80: dnxr_full_dive_intervention_window_long_test ............................***Not Run (Disabled)   0.00 sec
        Start  81: dnxr_watchdog_stale_subsystem_long_test
 81/283 Test  #81: dnxr_watchdog_stale_subsystem_long_test .................................   Passed    0.01 sec
        Start  82: dnxr_neural_chip_immersion_integration_test
 82/283 Test  #82: dnxr_neural_chip_immersion_integration_test .............................   Passed    0.02 sec
        Start  83: dnxr_clinical_readiness_assessor_test
 83/283 Test  #83: dnxr_clinical_readiness_assessor_test ...................................   Passed    0.01 sec
        Start  84: dnxr_preclinical_readiness_assessor_test
 84/283 Test  #84: dnxr_preclinical_readiness_assessor_test ................................   Passed    0.01 sec
        Start  85: dnxr_readiness_cannot_skip_hil_test
 85/283 Test  #85: dnxr_readiness_cannot_skip_hil_test .....................................   Passed    0.01 sec
        Start  86: dnxr_readiness_requires_traceability_test
 86/283 Test  #86: dnxr_readiness_requires_traceability_test ...............................   Passed    0.01 sec
        Start  87: dnxr_readiness_evidence_repository_test
 87/283 Test  #87: dnxr_readiness_evidence_repository_test .................................   Passed    0.01 sec
        Start  88: dnxr_clinical_readiness_blocks_unsigned_evidence_test
 88/283 Test  #88: dnxr_clinical_readiness_blocks_unsigned_evidence_test ...................   Passed    0.01 sec
        Start  89: dnxr_regulatory_claim_guard_test
 89/283 Test  #89: dnxr_regulatory_claim_guard_test ........................................   Passed    0.01 sec
        Start  90: dnxr_clinical_test_acceptance_review_gate_test
 90/283 Test  #90: dnxr_clinical_test_acceptance_review_gate_test ..........................   Passed    0.01 sec
        Start  91: dnxr_preclinical_readiness_requires_hil_reports_test
 91/283 Test  #91: dnxr_preclinical_readiness_requires_hil_reports_test ....................   Passed    0.01 sec
        Start  92: dnxr_country_readiness_requires_signed_artifacts_test
 92/283 Test  #92: dnxr_country_readiness_requires_signed_artifacts_test ...................   Passed    0.01 sec
        Start  93: dnxr_qms_review_record_test
 93/283 Test  #93: dnxr_qms_review_record_test .............................................   Passed    0.01 sec
        Start  94: dnxr_qms_signoff_record_test
 94/283 Test  #94: dnxr_qms_signoff_record_test ............................................   Passed    0.01 sec
        Start  95: dnxr_qms_review_gate_blocks_unsigned_high_risk_test
 95/283 Test  #95: dnxr_qms_review_gate_blocks_unsigned_high_risk_test .....................   Passed    0.01 sec
        Start  96: dnxr_full_dive_capability_matrix_test
 96/283 Test  #96: dnxr_full_dive_capability_matrix_test ...................................   Passed    0.01 sec
        Start  97: dnxr_full_dive_readiness_assessor_test
 97/283 Test  #97: dnxr_full_dive_readiness_assessor_test ..................................   Passed    0.01 sec
        Start  98: dnxr_full_dive_session_validator_test
 98/283 Test  #98: dnxr_full_dive_session_validator_test ...................................   Passed    0.01 sec
        Start  99: dnxr_hil_fault_injection_test
 99/283 Test  #99: dnxr_hil_fault_injection_test ...........................................   Passed    0.01 sec
        Start 100: dnxr_hil_latency_budget_test
100/283 Test #100: dnxr_hil_latency_budget_test ............................................   Passed    0.01 sec
        Start 101: dnxr_hil_emergency_stop_test
101/283 Test #101: dnxr_hil_emergency_stop_test ............................................   Passed    0.01 sec
        Start 102: dnxr_hil_phantom_disconnect_test
102/283 Test #102: dnxr_hil_phantom_disconnect_test ........................................   Passed    0.01 sec
        Start 103: dnxr_hil_recovery_sequence_test
103/283 Test #103: dnxr_hil_recovery_sequence_test .........................................   Passed    0.01 sec
        Start 104: dnxr_hil_readiness_assessor_test
104/283 Test #104: dnxr_hil_readiness_assessor_test ........................................   Passed    0.01 sec
        Start 105: dnxr_hil_scenario_runner_test
105/283 Test #105: dnxr_hil_scenario_runner_test ...........................................   Passed    0.01 sec
        Start 106: dnxr_hil_session_report_test
106/283 Test #106: dnxr_hil_session_report_test ............................................   Passed    0.01 sec
        Start 107: dnxr_hil_risk_coverage_matrix_test
107/283 Test #107: dnxr_hil_risk_coverage_matrix_test ......................................   Passed    0.01 sec
        Start 108: dnxr_hil_multi_fault_recovery_test
108/283 Test #108: dnxr_hil_multi_fault_recovery_test ......................................   Passed    0.01 sec
        Start 109: dnxr_hil_latency_p99_test
109/283 Test #109: dnxr_hil_latency_p99_test ...............................................   Passed    0.01 sec
        Start 110: dnxr_canada_readiness_profile_test
110/283 Test #110: dnxr_canada_readiness_profile_test ......................................   Passed    0.01 sec
        Start 111: dnxr_us_readiness_profile_test
111/283 Test #111: dnxr_us_readiness_profile_test ..........................................   Passed    0.01 sec
        Start 112: dnxr_country_readiness_blocks_without_protocol_test
112/283 Test #112: dnxr_country_readiness_blocks_without_protocol_test .....................   Passed    0.01 sec
        Start 113: dnxr_security_audit_trail_test
113/283 Test #113: dnxr_security_audit_trail_test ..........................................   Passed    0.01 sec
        Start 114: dnxr_threat_event_test
114/283 Test #114: dnxr_threat_event_test ..................................................   Passed    0.01 sec
        Start 115: dnxr_replay_attack_blocked_test
115/283 Test #115: dnxr_replay_attack_blocked_test .........................................   Passed    0.01 sec
        Start 116: dnxr_packet_tamper_detection_test
116/283 Test #116: dnxr_packet_tamper_detection_test .......................................   Passed    0.01 sec
        Start 117: dnxr_secure_update_policy_test
117/283 Test #117: dnxr_secure_update_policy_test ..........................................   Passed    0.01 sec
        Start 118: dnxr_runtime_controller_100k_cycles_test
118/283 Test #118: dnxr_runtime_controller_100k_cycles_test ................................***Not Run (Disabled)   0.00 sec
        Start 119: dnxr_runtime_no_allocation_zone_test
119/283 Test #119: dnxr_runtime_no_allocation_zone_test ....................................   Passed    0.01 sec
        Start 120: dnxr_runtime_latency_profiler_test
120/283 Test #120: dnxr_runtime_latency_profiler_test ......................................   Passed    0.01 sec
        Start 121: dnxr_runtime_jitter_analyzer_test
121/283 Test #121: dnxr_runtime_jitter_analyzer_test .......................................   Passed    0.01 sec
        Start 122: dnxr_runtime_wcet_recorder_test
122/283 Test #122: dnxr_runtime_wcet_recorder_test .........................................   Passed    0.01 sec
        Start 123: dnxr_human_use_blocked_without_authorization_test
123/283 Test #123: dnxr_human_use_blocked_without_authorization_test .......................   Passed    0.01 sec
        Start 124: dnxr_stimulus_zeroed_on_missing_monitoring_test
124/283 Test #124: dnxr_stimulus_zeroed_on_missing_monitoring_test .........................   Passed    0.01 sec
        Start 125: dnxr_deadline_violation_triggers_emergency_test
125/283 Test #125: dnxr_deadline_violation_triggers_emergency_test .........................   Passed    0.01 sec
        Start 126: dnxr_patient_stop_signal_latches_shutdown_test
126/283 Test #126: dnxr_patient_stop_signal_latches_shutdown_test ..........................   Passed    0.01 sec
        Start 127: dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test
127/283 Test #127: dnxr_unsigned_evidence_cannot_unlock_clinical_mode_test .................   Passed    0.01 sec
        Start 128: dnxr_runtime_five_sense_coherence_stage_test
128/283 Test #128: dnxr_runtime_five_sense_coherence_stage_test ............................   Passed    0.01 sec
        Start 129: dnxr_runtime_body_schema_stage_test
129/283 Test #129: dnxr_runtime_body_schema_stage_test .....................................   Passed    0.01 sec
        Start 130: dnxr_runtime_closed_loop_feedback_stage_test
130/283 Test #130: dnxr_runtime_closed_loop_feedback_stage_test ............................   Passed    0.01 sec
        Start 131: dnxr_runtime_perception_correction_stage_test
131/283 Test #131: dnxr_runtime_perception_correction_stage_test ...........................   Passed    0.01 sec
        Start 132: dnxr_neural_stimulus_translator_path_test
132/283 Test #132: dnxr_neural_stimulus_translator_path_test ...............................   Passed    0.01 sec
        Start 133: dnxr_full_dive_neural_frame_no_allocation_test
133/283 Test #133: dnxr_full_dive_neural_frame_no_allocation_test ..........................   Passed    0.01 sec
        Start 134: dnxr_neural_chip_only_interface_test
134/283 Test #134: dnxr_neural_chip_only_interface_test ....................................   Passed    0.01 sec
        Start 135: dnxr_modality_phase_lock_engine_test
135/283 Test #135: dnxr_modality_phase_lock_engine_test ....................................   Passed    0.01 sec
        Start 136: dnxr_perception_divergence_detector_test
136/283 Test #136: dnxr_perception_divergence_detector_test ................................   Passed    0.01 sec
        Start 137: dnxr_device_capability_limiter_test
137/283 Test #137: dnxr_device_capability_limiter_test .....................................   Passed    0.01 sec
        Start 138: dnxr_neural_chip_ack_required_test
138/283 Test #138: dnxr_neural_chip_ack_required_test ......................................   Passed    0.01 sec
        Start 139: dnxr_hal_accepts_seven_modality_full_dive_command_test
139/283 Test #139: dnxr_hal_accepts_seven_modality_full_dive_command_test ..................   Passed    0.01 sec
        Start 140: dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test
140/283 Test #140: dnxr_modern_stimulus_stage_does_not_call_legacy_nodes_test ..............   Passed    0.01 sec
        Start 141: dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test
141/283 Test #141: dnxr_body_pose_uses_acquisition_chip_frame_before_stimulus_test .........   Passed    0.01 sec
        Start 142: dnxr_signal_quality_gate_blocks_stimulus_test
142/283 Test #142: dnxr_signal_quality_gate_blocks_stimulus_test ...........................   Passed    0.01 sec
        Start 143: dnxr_neural_stimulus_translator_fixed_command_no_allocation_test
143/283 Test #143: dnxr_neural_stimulus_translator_fixed_command_no_allocation_test ........   Passed    0.01 sec
        Start 144: dnxr_clinical_boundary_rejects_research_orchestrator_test
144/283 Test #144: dnxr_clinical_boundary_rejects_research_orchestrator_test ...............   Passed    0.01 sec
        Start 145: dnxr_clinical_asset_gate_blocks_mmorpg_combat_test
145/283 Test #145: dnxr_clinical_asset_gate_blocks_mmorpg_combat_test ......................   Passed    0.01 sec
        Start 146: dnxr_five_sense_exposure_ledger_session_limit_test
146/283 Test #146: dnxr_five_sense_exposure_ledger_session_limit_test ......................   Passed    0.01 sec
        Start 147: dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test
147/283 Test #147: dnxr_safe_frame_hold_suppresses_vestibular_and_chemical_channels_test ...   Passed    0.01 sec
        Start 148: dnxr_neural_vector_validator_test
148/283 Test #148: dnxr_neural_vector_validator_test .......................................   Passed    0.01 sec
        Start 149: dnxr_patient_specific_neural_map_test
149/283 Test #149: dnxr_patient_specific_neural_map_test ...................................   Passed    0.01 sec
        Start 150: dnxr_modality_exposure_ledger_test
150/283 Test #150: dnxr_modality_exposure_ledger_test ......................................   Passed    0.01 sec
        Start 151: dnxr_per_modality_isolation_broker_test
151/283 Test #151: dnxr_per_modality_isolation_broker_test .................................   Passed    0.01 sec
        Start 152: dnxr_neural_chip_only_pipeline_test
152/283 Test #152: dnxr_neural_chip_only_pipeline_test .....................................   Passed    0.01 sec
        Start 153: dnxr_visual_cortical_world_encoder_test
153/283 Test #153: dnxr_visual_cortical_world_encoder_test .................................   Passed    0.01 sec
        Start 154: dnxr_virtual_epidermis_contact_codec_test
154/283 Test #154: dnxr_virtual_epidermis_contact_codec_test ...............................   Passed    0.01 sec
        Start 155: dnxr_avatar_kinematic_rig_controller_test
155/283 Test #155: dnxr_avatar_kinematic_rig_controller_test ...............................   Passed    0.01 sec
        Start 156: dnxr_chemical_channels_hold_test
156/283 Test #156: dnxr_chemical_channels_hold_test ........................................   Passed    0.01 sec
        Start 157: dnxr_reality_anchor_hard_channel_test
157/283 Test #157: dnxr_reality_anchor_hard_channel_test ...................................   Passed    0.01 sec
        Start 158: dnxr_neural_chip_bus_direction_split_test
158/283 Test #158: dnxr_neural_chip_bus_direction_split_test ...............................   Passed    0.01 sec
        Start 159: dnxr_motor_intent_decoder_pipeline_test
159/283 Test #159: dnxr_motor_intent_decoder_pipeline_test .................................   Passed    0.01 sec
        Start 160: dnxr_frame_lifecycle_guard_test
160/283 Test #160: dnxr_frame_lifecycle_guard_test .........................................   Passed    0.01 sec
        Start 161: dnxr_patient_modality_policy_gate_test
161/283 Test #161: dnxr_patient_modality_policy_gate_test ..................................   Passed    0.01 sec
        Start 162: dnxr_safe_chip_frame_hold_buffer_test
162/283 Test #162: dnxr_safe_chip_frame_hold_buffer_test ...................................   Passed    0.01 sec
        Start 163: dnxr_neural_chip_latency_budget_test
163/283 Test #163: dnxr_neural_chip_latency_budget_test ....................................   Passed    0.01 sec
        Start 164: dnxr_closed_loop_perception_error_controller_test
164/283 Test #164: dnxr_closed_loop_perception_error_controller_test .......................   Passed    0.01 sec
        Start 165: dnxr_chip_capability_contract_test
165/283 Test #165: dnxr_chip_capability_contract_test ......................................   Passed    0.01 sec
        Start 166: dnxr_neural_chip_adversarial_frame_test
166/283 Test #166: dnxr_neural_chip_adversarial_frame_test .................................   Passed    0.01 sec
        Start 167: dnxr_neural_chip_frame_committer_test
167/283 Test #167: dnxr_neural_chip_frame_committer_test ...................................   Passed    0.01 sec
        Start 168: dnxr_closed_loop_pipeline_integration_test
168/283 Test #168: dnxr_closed_loop_pipeline_integration_test ..............................   Passed    0.01 sec
        Start 169: dnxr_neural_vector_slew_rate_limiter_test
169/283 Test #169: dnxr_neural_vector_slew_rate_limiter_test ...............................   Passed    0.01 sec
        Start 170: dnxr_experimental_chemical_channel_gate_test
170/283 Test #170: dnxr_experimental_chemical_channel_gate_test ............................   Passed    0.01 sec
        Start 171: dnxr_neural_chip_pipeline_config_test
171/283 Test #171: dnxr_neural_chip_pipeline_config_test ...................................   Passed    0.01 sec
        Start 172: dnxr_neural_chip_decision_trace_test
172/283 Test #172: dnxr_neural_chip_decision_trace_test ....................................   Passed    0.01 sec
        Start 173: dnxr_full_dive_chip_frame_diff_test
173/283 Test #173: dnxr_full_dive_chip_frame_diff_test .....................................   Passed    0.01 sec
        Start 174: dnxr_neural_chip_session_state_test
174/283 Test #174: dnxr_neural_chip_session_state_test .....................................   Passed    0.01 sec
        Start 175: dnxr_neural_chip_emergency_latch_test
175/283 Test #175: dnxr_neural_chip_emergency_latch_test ...................................   Passed    0.01 sec
        Start 176: dnxr_neural_chip_only_long_session_stability_test
176/283 Test #176: dnxr_neural_chip_only_long_session_stability_test .......................   Passed    0.49 sec
        Start 177: dnxr_neural_chip_adversarial_sequence_test
177/283 Test #177: dnxr_neural_chip_adversarial_sequence_test ..............................   Passed    0.02 sec
        Start 178: dnxr_neural_chip_hold_commit_policy_test
178/283 Test #178: dnxr_neural_chip_hold_commit_policy_test ................................   Passed    0.01 sec
        Start 179: dnxr_neural_chip_replay_recorder_integration_test
179/283 Test #179: dnxr_neural_chip_replay_recorder_integration_test .......................   Passed    0.01 sec
        Start 180: dnxr_full_dive_chip_frame_diff_coverage_test
180/283 Test #180: dnxr_full_dive_chip_frame_diff_coverage_test ............................   Passed    0.01 sec
        Start 181: dnxr_per_modality_rate_and_dose_policy_integration_test
181/283 Test #181: dnxr_per_modality_rate_and_dose_policy_integration_test .................   Passed    0.01 sec
        Start 182: dnxr_neural_chip_session_supervisor_test
182/283 Test #182: dnxr_neural_chip_session_supervisor_test ................................   Passed    0.01 sec
        Start 183: dnxr_neural_chip_emergency_latch_authorized_reset_test
183/283 Test #183: dnxr_neural_chip_emergency_latch_authorized_reset_test ..................   Passed    0.01 sec
        Start 184: dnxr_neural_chip_ack_handshake_test
184/283 Test #184: dnxr_neural_chip_ack_handshake_test .....................................   Passed    0.01 sec
        Start 185: dnxr_lifecycle_two_phase_commit_test
185/283 Test #185: dnxr_lifecycle_two_phase_commit_test ....................................   Passed    0.01 sec
        Start 186: dnxr_hold_fallback_chain_does_not_poison_lifecycle_test
186/283 Test #186: dnxr_hold_fallback_chain_does_not_poison_lifecycle_test .................   Passed    0.01 sec
        Start 187: dnxr_ack_pending_state_test
187/283 Test #187: dnxr_ack_pending_state_test .............................................   Passed    0.01 sec
        Start 188: dnxr_hold_reality_anchor_safe_test
188/283 Test #188: dnxr_hold_reality_anchor_safe_test ......................................   Passed    0.01 sec
        Start 189: dnxr_world_frame_lifecycle_reject_test
189/283 Test #189: dnxr_world_frame_lifecycle_reject_test ..................................   Passed    0.01 sec
        Start 190: dnxr_disabled_modality_full_neutrality_test
190/283 Test #190: dnxr_disabled_modality_full_neutrality_test .............................   Passed    0.01 sec
        Start 191: dnxr_sensor_encoder_contract_full_coverage_test
191/283 Test #191: dnxr_sensor_encoder_contract_full_coverage_test .........................   Passed    0.01 sec
        Start 192: dnxr_pipeline_resume_after_latch_reset_test
192/283 Test #192: dnxr_pipeline_resume_after_latch_reset_test .............................   Passed    0.01 sec
        Start 193: dnxr_replay_recorder_bounded_memory_test
193/283 Test #193: dnxr_replay_recorder_bounded_memory_test ................................   Passed    0.01 sec
        Start 194: dnxr_session_digest_test
194/283 Test #194: dnxr_session_digest_test ................................................   Passed    0.01 sec
        Start 195: dnxr_motor_intent_confidence_gate_test
195/283 Test #195: dnxr_motor_intent_confidence_gate_test ..................................   Passed    0.01 sec
        Start 196: dnxr_avatar_safety_pose_controller_test
196/283 Test #196: dnxr_avatar_safety_pose_controller_test .................................   Passed    0.01 sec
        Start 197: dnxr_neural_chip_pipeline_config_validator_test
197/283 Test #197: dnxr_neural_chip_pipeline_config_validator_test .........................   Passed    0.01 sec
        Start 198: dnxr_neural_chip_pipeline_fuzz_session_test
198/283 Test #198: dnxr_neural_chip_pipeline_fuzz_session_test .............................   Passed    0.09 sec
        Start 199: dnxr_neural_chip_ack_external_injection_test
199/283 Test #199: dnxr_neural_chip_ack_external_injection_test ............................   Passed    0.01 sec
        Start 200: dnxr_hold_frame_sequencer_test
200/283 Test #200: dnxr_hold_frame_sequencer_test ..........................................   Passed    0.01 sec
        Start 201: dnxr_neural_chip_hold_lifecycle_test
201/283 Test #201: dnxr_neural_chip_hold_lifecycle_test ....................................   Passed    0.01 sec
        Start 202: dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test
202/283 Test #202: dnxr_neural_chip_safety_hold_does_not_mutate_motor_read_test ............   Passed    0.01 sec
        Start 203: dnxr_pipeline_resume_controller_test
203/283 Test #203: dnxr_pipeline_resume_controller_test ....................................   Passed    0.01 sec
        Start 204: dnxr_emergency_reset_audit_trail_test
204/283 Test #204: dnxr_emergency_reset_audit_trail_test ...................................   Passed    0.01 sec
        Start 205: dnxr_neural_chip_replay_recorder_jsonl_test
205/283 Test #205: dnxr_neural_chip_replay_recorder_jsonl_test .............................   Passed    0.01 sec
        Start 206: dnxr_neural_chip_replay_player_test
206/283 Test #206: dnxr_neural_chip_replay_player_test .....................................   Passed    0.01 sec
        Start 207: dnxr_decision_trace_modality_fieldpath_test
207/283 Test #207: dnxr_decision_trace_modality_fieldpath_test .............................   Passed    0.01 sec
        Start 208: dnxr_decision_trace_coverage_validator_test
208/283 Test #208: dnxr_decision_trace_coverage_validator_test .............................   Passed    0.01 sec
        Start 209: dnxr_disabled_modality_neutrality_validator_test
209/283 Test #209: dnxr_disabled_modality_neutrality_validator_test ........................   Passed    0.01 sec
        Start 210: dnxr_reality_anchor_degradation_model_test
210/283 Test #210: dnxr_reality_anchor_degradation_model_test ..............................   Passed    0.01 sec
        Start 211: dnxr_world_frame_sanitizer_test
211/283 Test #211: dnxr_world_frame_sanitizer_test .........................................   Passed    0.01 sec
        Start 212: dnxr_sensor_encoder_output_contract_test
212/283 Test #212: dnxr_sensor_encoder_output_contract_test ................................   Passed    0.01 sec
        Start 213: dnxr_neural_chip_pipeline_state_machine_test
213/283 Test #213: dnxr_neural_chip_pipeline_state_machine_test ............................   Passed    0.01 sec
        Start 214: dnxr_pipeline_state_machine_ack_timeout_sequence_test
214/283 Test #214: dnxr_pipeline_state_machine_ack_timeout_sequence_test ...................   Passed    0.01 sec
        Start 215: dnxr_pipeline_config_loader_parser_result_test
215/283 Test #215: dnxr_pipeline_config_loader_parser_result_test ..........................   Passed    0.01 sec
        Start 216: dnxr_decision_trace_path_aware_coverage_test
216/283 Test #216: dnxr_decision_trace_path_aware_coverage_test ............................   Passed    0.01 sec
        Start 217: dnxr_emergency_latch_controller_audit_test
217/283 Test #217: dnxr_emergency_latch_controller_audit_test ..............................   Passed    0.01 sec
        Start 218: dnxr_pipeline_session_digest_integration_test
218/283 Test #218: dnxr_pipeline_session_digest_integration_test ...........................   Passed    0.01 sec
        Start 219: dnxr_safety_invariant_engine_test
219/283 Test #219: dnxr_safety_invariant_engine_test .......................................   Passed    0.01 sec
        Start 220: dnxr_sanitize_failure_frame_factory_test
220/283 Test #220: dnxr_sanitize_failure_frame_factory_test ................................   Passed    0.01 sec
        Start 221: dnxr_neural_chip_timebase_lifecycle_ack_test
221/283 Test #221: dnxr_neural_chip_timebase_lifecycle_ack_test ............................   Passed    0.01 sec
        Start 222: dnxr_neural_vector_field_registry_single_source_test
222/283 Test #222: dnxr_neural_vector_field_registry_single_source_test ....................   Passed    0.01 sec
        Start 223: dnxr_pipeline_state_gate_test
223/283 Test #223: dnxr_pipeline_state_gate_test ...........................................   Passed    0.01 sec
        Start 224: dnxr_ack_early_gate_before_encoders_test
224/283 Test #224: dnxr_ack_early_gate_before_encoders_test ................................   Passed    0.01 sec
        Start 225: dnxr_reject_with_hold_path_kind_test
225/283 Test #225: dnxr_reject_with_hold_path_kind_test ....................................   Passed    0.01 sec
        Start 226: dnxr_pipeline_path_classifier_test
226/283 Test #226: dnxr_pipeline_path_classifier_test ......................................   Passed    0.01 sec
        Start 227: dnxr_decision_trace_coverage_pipeline_integration_test
227/283 Test #227: dnxr_decision_trace_coverage_pipeline_integration_test ..................   Passed    0.01 sec
        Start 228: dnxr_real_readiness_gate_allows_hardware_bench_no_human_test
228/283 Test #228: dnxr_real_readiness_gate_allows_hardware_bench_no_human_test ............   Passed    0.01 sec
        Start 229: dnxr_real_readiness_gate_blocks_human_without_authorization_test
229/283 Test #229: dnxr_real_readiness_gate_blocks_human_without_authorization_test ........   Passed    0.01 sec
        Start 230: dnxr_hardware_bench_requires_dummy_load_test
230/283 Test #230: dnxr_hardware_bench_requires_dummy_load_test ............................   Passed    0.01 sec
        Start 231: dnxr_hardware_bench_requires_emergency_stop_test
231/283 Test #231: dnxr_hardware_bench_requires_emergency_stop_test ........................   Passed    0.01 sec
        Start 232: dnxr_hardware_bench_requires_trace_recorder_test
232/283 Test #232: dnxr_hardware_bench_requires_trace_recorder_test ........................   Passed    0.01 sec
        Start 233: dnxr_real_hal_rejects_raw_sensory_payload_compile_test
233/283 Test #233: dnxr_real_hal_rejects_raw_sensory_payload_compile_test ..................   Passed    0.01 sec
        Start 234: dnxr_real_hal_accepts_bounded_stimulus_command_test
234/283 Test #234: dnxr_real_hal_accepts_bounded_stimulus_command_test .....................   Passed    0.01 sec
        Start 235: dnxr_hardware_bench_dry_run_sends_no_active_output_test
235/283 Test #235: dnxr_hardware_bench_dry_run_sends_no_active_output_test .................   Passed    0.01 sec
        Start 236: dnxr_emergency_latch_controller_pipeline_integration_test
236/283 Test #236: dnxr_emergency_latch_controller_pipeline_integration_test ...............   Passed    0.01 sec
        Start 237: dnxr_hold_stability_tracker_test
237/283 Test #237: dnxr_hold_stability_tracker_test ........................................   Passed    0.01 sec
        Start 238: dnxr_pipeline_resume_state_bridge_test
238/283 Test #238: dnxr_pipeline_resume_state_bridge_test ..................................   Passed    0.01 sec
        Start 239: dnxr_replay_schema_version_test
239/283 Test #239: dnxr_replay_schema_version_test .........................................   Passed    0.01 sec
        Start 240: dnxr_replay_consistency_verifier_test
240/283 Test #240: dnxr_replay_consistency_verifier_test ...................................   Passed    0.01 sec
        Start 241: dnxr_pipeline_safety_case_report_test
241/283 Test #241: dnxr_pipeline_safety_case_report_test ...................................   Passed    0.01 sec
        Start 242: dnxr_pipeline_state_machine_runtime_invariant_test
242/283 Test #242: dnxr_pipeline_state_machine_runtime_invariant_test ......................   Passed    0.01 sec
        Start 243: dnxr_pipeline_no_auto_ack_recovery_test
243/283 Test #243: dnxr_pipeline_no_auto_ack_recovery_test .................................   Passed    0.01 sec
        Start 244: dnxr_timebase_advances_on_repeated_world_timestamp_test
244/283 Test #244: dnxr_timebase_advances_on_repeated_world_timestamp_test .................   Passed    0.01 sec
        Start 245: dnxr_pipeline_shutdown_no_normal_frames_test
245/283 Test #245: dnxr_pipeline_shutdown_no_normal_frames_test ............................   Passed    0.01 sec
        Start 246: dnxr_state_machine_transition_matrix_test
246/283 Test #246: dnxr_state_machine_transition_matrix_test ...............................   Passed    0.01 sec
        Start 247: dnxr_strict_trace_coverage_blocks_incomplete_trace_test
247/283 Test #247: dnxr_strict_trace_coverage_blocks_incomplete_trace_test .................   Passed    0.01 sec
        Start 248: dnxr_frame_commit_result_bus_submitted_test
248/283 Test #248: dnxr_frame_commit_result_bus_submitted_test .............................   Passed    0.01 sec
        Start 249: dnxr_emergency_transition_requires_audited_reset_test
249/283 Test #249: dnxr_emergency_transition_requires_audited_reset_test ...................   Passed    0.01 sec
        Start 250: dnxr_step_result_consistency_verifier_test
250/283 Test #250: dnxr_step_result_consistency_verifier_test ..............................   Passed    0.01 sec
        Start 251: dnxr_pipeline_reject_code_classifier_test
251/283 Test #251: dnxr_pipeline_reject_code_classifier_test ...............................   Passed    0.01 sec
        Start 252: dnxr_neural_chip_stateful_long_session_test
252/283 Test #252: dnxr_neural_chip_stateful_long_session_test .............................   Passed    0.27 sec
        Start 253: dnxr_human_subject_use_gate_test
253/283 Test #253: dnxr_human_subject_use_gate_test ........................................   Passed    0.01 sec
        Start 254: dnxr_release_mode_lock_test
254/283 Test #254: dnxr_release_mode_lock_test .............................................   Passed    0.01 sec
        Start 255: dnxr_medical_review_gate_test
255/283 Test #255: dnxr_medical_review_gate_test ...........................................   Passed    0.01 sec
        Start 256: dnxr_regulatory_authorization_evidence_gate_test
256/283 Test #256: dnxr_regulatory_authorization_evidence_gate_test ........................   Passed    0.01 sec
        Start 257: dnxr_hardware_bench_readiness_gate_test
257/283 Test #257: dnxr_hardware_bench_readiness_gate_test .................................   Passed    0.01 sec
        Start 258: dnxr_p95_p99_acceptance_gate_test
258/283 Test #258: dnxr_p95_p99_acceptance_gate_test .......................................   Passed    0.01 sec
        Start 259: dnxr_performance_telemetry_recorder_test
259/283 Test #259: dnxr_performance_telemetry_recorder_test ................................   Passed    0.01 sec
        Start 260: dnxr_hardware_bench_session_runner_test
260/283 Test #260: dnxr_hardware_bench_session_runner_test .................................   Passed    0.01 sec
        Start 261: dnxr_bench_only_neural_hardware_adapter_test
261/283 Test #261: dnxr_bench_only_neural_hardware_adapter_test ............................   Passed    0.01 sec
        Start 262: dnxr_traceability_coverage_gate_test
262/283 Test #262: dnxr_traceability_coverage_gate_test ....................................   Passed    0.01 sec
        Start 263: dnxr_release_readiness_gate_test
263/283 Test #263: dnxr_release_readiness_gate_test ........................................   Passed    0.01 sec
        Start 264: dnxr_signed_evidence_manifest_test
264/283 Test #264: dnxr_signed_evidence_manifest_test ......................................   Passed    0.01 sec
        Start 265: dnxr_gustatory_discrete_stimulation_test
265/283 Test #265: dnxr_gustatory_discrete_stimulation_test ................................   Passed    0.01 sec
        Start 266: dnxr_haptic_proprioception_mismatch_test
266/283 Test #266: dnxr_haptic_proprioception_mismatch_test ................................   Passed    0.01 sec
        Start 267: dnxr_no_pain_mapping_test
267/283 Test #267: dnxr_no_pain_mapping_test ...............................................   Passed    0.01 sec
        Start 268: dnxr_olfactory_persistence_budget_test
268/283 Test #268: dnxr_olfactory_persistence_budget_test ..................................   Passed    0.01 sec
        Start 269: dnxr_thermal_burn_lockout_test
269/283 Test #269: dnxr_thermal_burn_lockout_test ..........................................   Passed    0.01 sec
        Start 270: dnxr_visual_audio_phase_lock_test
270/283 Test #270: dnxr_visual_audio_phase_lock_test .......................................   Passed    0.01 sec
        Start 271: dnxr_visual_vestibular_conflict_blocks_motion_test
271/283 Test #271: dnxr_visual_vestibular_conflict_blocks_motion_test ......................   Passed    0.01 sec
        Start 272: dnxr_neural_chip_ack_timeout_test
272/283 Test #272: dnxr_neural_chip_ack_timeout_test .......................................   Passed    0.01 sec
        Start 273: dnxr_neural_chip_capability_mismatch_test
273/283 Test #273: dnxr_neural_chip_capability_mismatch_test ...............................   Passed    0.01 sec
        Start 274: dnxr_neural_chip_feedback_required_test
274/283 Test #274: dnxr_neural_chip_feedback_required_test .................................   Passed    0.01 sec
        Start 275: dnxr_neural_chip_firmware_mismatch_test
275/283 Test #275: dnxr_neural_chip_firmware_mismatch_test .................................   Passed    0.01 sec
        Start 276: dnxr_neural_chip_packet_hash_chain_test
276/283 Test #276: dnxr_neural_chip_packet_hash_chain_test .................................   Passed    0.01 sec
        Start 277: dnxr_neural_chip_temperature_cutoff_test
277/283 Test #277: dnxr_neural_chip_temperature_cutoff_test ................................   Passed    0.01 sec
        Start 278: dnxr_decompression_state_machine_test
278/283 Test #278: dnxr_decompression_state_machine_test ...................................   Passed    0.01 sec
        Start 279: dnxr_dissociation_reality_anchor_test
279/283 Test #279: dnxr_dissociation_reality_anchor_test ...................................   Passed    0.01 sec
        Start 280: dnxr_flash_pattern_visual_block_test
280/283 Test #280: dnxr_flash_pattern_visual_block_test ....................................   Passed    0.01 sec
        Start 281: dnxr_locked_in_channel_loss_test
281/283 Test #281: dnxr_locked_in_channel_loss_test ........................................   Passed    0.01 sec
        Start 282: dnxr_pain_perception_emergency_stop_test
282/283 Test #282: dnxr_pain_perception_emergency_stop_test ................................   Passed    0.01 sec
        Start 283: dnxr_vestibular_hard_limit_test
283/283 Test #283: dnxr_vestibular_hard_limit_test .........................................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 279

Label Time Summary:
callback            =   0.01 sec*proc (1 test)
canada              =   0.01 sec*proc (1 test)
clinical_blocker    =   0.02 sec*proc (2 tests)
concurrency         =   0.06 sec*proc (3 tests)
controller          =   7.89 sec*proc (6 tests)
evidence            =   0.02 sec*proc (2 tests)
full_dive           =   0.73 sec*proc (4 tests)
hardware            =   0.05 sec*proc (4 tests)
hardware_bench      =   0.05 sec*proc (4 tests)
hil                 =   0.05 sec*proc (5 tests)
latency             =   0.01 sec*proc (1 test)
legacy              =   0.01 sec*proc (1 test)
long_fast           =  10.65 sec*proc (2 tests)
long_real           =   0.00 sec*proc (2 tests)
long_simulated      =  23.57 sec*proc (9 tests)
memory              =   6.97 sec*proc (5 tests)
readiness           =   0.03 sec*proc (3 tests)
real                =   0.07 sec*proc (6 tests)
regulatory          =   0.01 sec*proc (1 test)
risk                =   0.01 sec*proc (1 test)
runtime             =  28.54 sec*proc (25 tests)
safety              =   0.85 sec*proc (14 tests)
services            =   0.67 sec*proc (2 tests)
sinks               =   0.01 sec*proc (1 test)
soak                =  20.30 sec*proc (7 tests)
stage               =   0.16 sec*proc (10 tests)
unit                =   4.07 sec*proc (31 tests)
us                  =   0.01 sec*proc (1 test)

Total Test time (real) =  42.03 sec

The following tests did not run:
	 66 - dnxr_runtime_controller_pipeline_10k_cycles_test (Disabled)
	 79 - dnxr_vestibular_exposure_budget_long_test (Disabled)
	 80 - dnxr_full_dive_intervention_window_long_test (Disabled)
	118 - dnxr_runtime_controller_100k_cycles_test (Disabled)
```
