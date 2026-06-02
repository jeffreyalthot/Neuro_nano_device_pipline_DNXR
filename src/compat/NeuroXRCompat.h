#pragma once

#include "../affective/AffectiveRealityEngine.h"
#include "../cognition/ConsciousDreamingEngine.h"
#include "../cognition/PersistentNeuroCognitiveTwin.h"
#include "../cognition/SubjectiveContinuityEngine.h"
#include "../emotion/AffectiveResonanceMatrix.h"
#include "../immersion/NeuralPerceptionSynthesizer.h"
#include "../immersion/SyntheticProprioceptionLayer.h"
#include "../immersion/VestibularEmulationEngine.h"
#include "../interface/NeuralLinkDeviceInterface.h"
#include "../memory/ImmersiveMemoryPersistenceEngine.h"
#include "../memory/RecursiveMemoryReconstructionEngine.h"
#include "../network/DistributedConsciousnessNetwork.h"
#include "../neural/MotorPredictionLoop.h"
#include "../neural/NeuralOscillationModel.h"
#include "../neuro/BrainStateTopology.h"
#include "../perception/RealityCoherenceEngine.h"
#include "../physics/NeuralQuantumEntanglementEngine.h"
#include "../reality/SubjectiveTimeDilationEngine.h"
#include "../runtime/ConsciousnessBridge.h"
#include "../runtime/FullDiveOrchestrator.h"
#include "../runtime/FullDiveSynchronizationEngine.h"
#include "../security/NeuralSafetyMonitor.h"
#include "../social/SocialPresenceEngine.h"
#include "../subconscious/SubconsciousSimulationEngine.h"
#include "../time/SubjectiveTimeEngine.h"

// Centralized namespace-compatibility aggregation point for downstream code that
// still includes neuroxr aliases while the main source tree standardizes on dnxr.
// New production code should include dnxr headers directly and use namespace dnxr.
namespace neuroxr {
using EmotionalStateType = ::dnxr::EmotionalStateType;
using EmotionalVector = ::dnxr::EmotionalVector;
using AffectiveFrame = ::dnxr::AffectiveFrame;
using AffectiveRealityEngine = ::dnxr::AffectiveRealityEngine;
using DreamLayer = ::dnxr::DreamLayer;
using DreamSignal = ::dnxr::DreamSignal;
using DreamSequence = ::dnxr::DreamSequence;
using DreamingState = ::dnxr::DreamingState;
using ConsciousDreamingEngine = ::dnxr::ConsciousDreamingEngine;
using CognitiveSnapshot = ::dnxr::CognitiveSnapshot;
using NeuralBehaviorProfile = ::dnxr::NeuralBehaviorProfile;
using NeuroCognitiveState = ::dnxr::NeuroCognitiveState;
using PersistentNeuroCognitiveTwin = ::dnxr::PersistentNeuroCognitiveTwin;
using ContinuityFrame = ::dnxr::ContinuityFrame;
using SubjectiveState = ::dnxr::SubjectiveState;
using SubjectiveContinuityEngine = ::dnxr::SubjectiveContinuityEngine;
using AffectiveSpectrum = ::dnxr::AffectiveSpectrum;
using AffectiveSignal = ::dnxr::AffectiveSignal;
using AffectiveFieldState = ::dnxr::AffectiveFieldState;
using AffectiveResonanceMatrix = ::dnxr::AffectiveResonanceMatrix;
using SyntheticPerceptionFrame = ::dnxr::SyntheticPerceptionFrame;
using NeuralPerceptionSynthesizer = ::dnxr::NeuralPerceptionSynthesizer;
using LimbState = ::dnxr::LimbState;
using ProprioceptiveFrame = ::dnxr::ProprioceptiveFrame;
using SyntheticProprioceptionLayer = ::dnxr::SyntheticProprioceptionLayer;
using VestibularState = ::dnxr::VestibularState;
using VestibularEmulationEngine = ::dnxr::VestibularEmulationEngine;
using NeuralDeviceStatus = ::dnxr::NeuralDeviceStatus;
using NeuralElectrodeSignal = ::dnxr::NeuralElectrodeSignal;
using NeuralDeviceFrame = ::dnxr::NeuralDeviceFrame;
using NeuralLinkDeviceInterface = ::dnxr::NeuralLinkDeviceInterface;
using MemoryType = ::dnxr::MemoryType;
using MemoryFragment = ::dnxr::MemoryFragment;
using MemoryState = ::dnxr::MemoryState;
using ImmersiveMemoryPersistenceEngine = ::dnxr::ImmersiveMemoryPersistenceEngine;
using MemoryLayerType = ::dnxr::MemoryLayerType;
using ReconstructedMemoryFragment = ::dnxr::ReconstructedMemoryFragment;
using MemoryReconstructionState = ::dnxr::MemoryReconstructionState;
using RecursiveMemoryReconstructionEngine = ::dnxr::RecursiveMemoryReconstructionEngine;
using ConsciousNodeType = ::dnxr::ConsciousNodeType;
using ConsciousnessPacket = ::dnxr::ConsciousnessPacket;
using ConsciousnessNode = ::dnxr::ConsciousnessNode;
using DistributedConsciousnessState = ::dnxr::DistributedConsciousnessState;
using DistributedConsciousnessNetwork = ::dnxr::DistributedConsciousnessNetwork;
using MotorState = ::dnxr::MotorState;
using MotorPredictionFrame = ::dnxr::MotorPredictionFrame;
using MotorPredictionLoop = ::dnxr::MotorPredictionLoop;
using OscillationBand = ::dnxr::OscillationBand;
using OscillationSignal = ::dnxr::OscillationSignal;
using NeuralOscillationState = ::dnxr::NeuralOscillationState;
using NeuralOscillationModel = ::dnxr::NeuralOscillationModel;
using CorticalRegionType = ::dnxr::CorticalRegionType;
using NeuralNode = ::dnxr::NeuralNode;
using CorticalRegion = ::dnxr::CorticalRegion;
using BrainStateTopology = ::dnxr::BrainStateTopology;
using PerceptualLayer = ::dnxr::PerceptualLayer;
using PerceptualSignal = ::dnxr::PerceptualSignal;
using RealityFragment = ::dnxr::RealityFragment;
using RealityCoherenceState = ::dnxr::RealityCoherenceState;
using RealityCoherenceEngine = ::dnxr::RealityCoherenceEngine;
using QuantumEntanglementType = ::dnxr::QuantumEntanglementType;
using QuantumNeuralState = ::dnxr::QuantumNeuralState;
using QuantumSynchronizationFrame = ::dnxr::QuantumSynchronizationFrame;
using NeuralQuantumEntanglementEngine = ::dnxr::NeuralQuantumEntanglementEngine;
using TemporalLayerType = ::dnxr::TemporalLayerType;
using TemporalFrame = ::dnxr::TemporalFrame;
using TemporalRealityState = ::dnxr::TemporalRealityState;
using SubjectiveTimeDilationEngine = ::dnxr::SubjectiveTimeDilationEngine;
using ConsciousnessLayer = ::dnxr::ConsciousnessLayer;
using ConsciousnessSignal = ::dnxr::ConsciousnessSignal;
using ConsciousnessState = ::dnxr::ConsciousnessState;
using ConsciousnessBridge = ::dnxr::ConsciousnessBridge;
using FullDiveState = ::dnxr::FullDiveState;
using FullDiveOrchestrator = ::dnxr::FullDiveOrchestrator;
using FullDiveRuntimeState = ::dnxr::FullDiveRuntimeState;
using FullDiveUpdateMode = ::dnxr::FullDiveUpdateMode;
using FullDiveSynchronizationEngine = ::dnxr::FullDiveSynchronizationEngine;
using NeuralThreatLevel = ::dnxr::NeuralThreatLevel;
using NeuralSafetySample = ::dnxr::NeuralSafetySample;
using NeuralSafetyState = ::dnxr::NeuralSafetyState;
using NeuralSafetyMonitor = ::dnxr::NeuralSafetyMonitor;
using SocialSignal = ::dnxr::SocialSignal;
using SocialEntity = ::dnxr::SocialEntity;
using SocialState = ::dnxr::SocialState;
using SocialPresenceEngine = ::dnxr::SocialPresenceEngine;
using SubconsciousPatternType = ::dnxr::SubconsciousPatternType;
using SubconsciousPattern = ::dnxr::SubconsciousPattern;
using SubconsciousState = ::dnxr::SubconsciousState;
using SubconsciousSimulationEngine = ::dnxr::SubconsciousSimulationEngine;
using SubjectiveTimeFrame = ::dnxr::SubjectiveTimeFrame;
using SubjectiveTemporalState = ::dnxr::SubjectiveTemporalState;
using SubjectiveTimeEngine = ::dnxr::SubjectiveTimeEngine;
}
