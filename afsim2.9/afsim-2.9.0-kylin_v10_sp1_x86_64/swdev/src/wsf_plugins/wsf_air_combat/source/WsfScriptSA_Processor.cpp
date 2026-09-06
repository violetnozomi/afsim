// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptSA_Processor.hpp"

#include "UtScriptVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSA_Assess.hpp"
#include "WsfSA_GroupUtils.hpp"
#include "WsfSA_Perceive.hpp"
#include "WsfSA_Predict.hpp"
#include "WsfSA_Processor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptSA_ProcessorClass::WsfScriptSA_ProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptProcessorClass(aClassName, aScriptTypesPtr)
{
   mConstructible = false;
   SetClassName("WsfSA_Processor");

   // Assessment Methods
   AddMethod(ut::make_unique<Risk>());
   AddMethod(ut::make_unique<SelfRisk>());
   AddMethod(ut::make_unique<FlightRisk>());
   AddMethod(ut::make_unique<PackageRisk>());
   AddMethod(ut::make_unique<MissionRisk>());
   AddMethod(ut::make_unique<Defensiveness>());
   AddMethod(ut::make_unique<Urgency>());

   // Asset Methods
   AddMethod(ut::make_unique<PerceivedAssets>());
   AddMethod(ut::make_unique<PerceivedAircraftAssets>());
   AddMethod(ut::make_unique<PerceivedMissileAssets>());
   AddMethod(ut::make_unique<SetAssetImportant>("SetAssetImportant"));
   AddMethod(ut::make_unique<SetAssetImportant_2>("SetAssetImportant"));
   AddMethod(ut::make_unique<SetAssetTypeImportant>());
   AddMethod(ut::make_unique<SetAssetTypeUnimportant>());
   AddMethod(ut::make_unique<SetAssetUnimportant>("SetAssetUnimportant"));
   AddMethod(ut::make_unique<SetAssetUnimportant_2>("SetAssetUnimportant"));
   AddMethod(ut::make_unique<NearestAsset>());
   AddMethod(ut::make_unique<NearestAircraftAsset>());
   AddMethod(ut::make_unique<NearestMissileAsset>());

   AddMethod(ut::make_unique<MaxAssetLoad>());
   AddMethod(ut::make_unique<SetMaxAssetLoad>());
   AddMethod(ut::make_unique<ResetMaxAssetLoad>());
   AddMethod(ut::make_unique<MaxThreatLoad>());
   AddMethod(ut::make_unique<SetMaxThreatLoad>());
   AddMethod(ut::make_unique<ResetMaxThreatLoad>());
   AddMethod(ut::make_unique<MaxPrioritizedThreats>());
   AddMethod(ut::make_unique<SetMaxPrioritizedThreats>());
   AddMethod(ut::make_unique<ResetMaxPrioritizedThreats>());
   AddMethod(ut::make_unique<MaxPrioritizedTargets>());
   AddMethod(ut::make_unique<SetMaxPrioritizedTargets>());
   AddMethod(ut::make_unique<ResetMaxPrioritizedTargets>());

   // Bogie/Bandit Methods
   AddMethod(ut::make_unique<PerceivedBogies>());
   AddMethod(ut::make_unique<PerceivedAircraftBogies>());
   AddMethod(ut::make_unique<PerceivedMissileBogies>());
   AddMethod(ut::make_unique<PerceivedBandits>());
   AddMethod(ut::make_unique<PerceivedAircraftBandits>());
   AddMethod(ut::make_unique<PerceivedMissileBandits>());
   AddMethod(ut::make_unique<SetThreatImportant>());
   AddMethod(ut::make_unique<SetThreatTypeImportant>());
   AddMethod(ut::make_unique<SetThreatTypeUnimportant>());
   AddMethod(ut::make_unique<SetThreatUnimportant>());
   AddMethod(ut::make_unique<NearestBogie>());
   AddMethod(ut::make_unique<NearestAircraftBogie>());
   AddMethod(ut::make_unique<NearestMissileBogie>());
   AddMethod(ut::make_unique<NearestBandit>());
   AddMethod(ut::make_unique<NearestAircraftBandit>());
   AddMethod(ut::make_unique<NearestMissileBandit>());

   // Prioritized Threats Methods
   AddMethod(ut::make_unique<PrioritizedThreatItems>());
   AddMethod(ut::make_unique<PrioritizedThreatEntities>());
   AddMethod(ut::make_unique<PrioritizedAircraftThreatEntities>());
   AddMethod(ut::make_unique<PrioritizedMissileThreatEntities>());
   AddMethod(ut::make_unique<PrioritizedThreatGroups>());
   AddMethod(ut::make_unique<HighestThreat>());
   AddMethod(ut::make_unique<HighestThreatAircraft>());
   AddMethod(ut::make_unique<HighestThreatMissile>());
   AddMethod(ut::make_unique<HighestThreatGroup>());

   // Prioritized Targets Methods
   AddMethod(ut::make_unique<PrioritizedTargetItems>());
   AddMethod(ut::make_unique<PrioritizedTargetEntities>());
   AddMethod(ut::make_unique<PrioritizedAircraftTargetEntities>());
   AddMethod(ut::make_unique<PrioritizedMissileTargetEntities>());
   AddMethod(ut::make_unique<PrioritizedTargetGroups>());
   AddMethod(ut::make_unique<BestTarget>());
   AddMethod(ut::make_unique<BestTargetAircraft>());
   AddMethod(ut::make_unique<BestTargetMissile>());
   AddMethod(ut::make_unique<BestTargetGroup>());

   // Generic Prediction Methods
   AddMethod(ut::make_unique<ProjectPositionInTime>());

   // Prediction Primitives
   AddMethod(ut::make_unique<ProjectPositionForward>());
   AddMethod(ut::make_unique<ProjectPositionTurnToHeading>());
   AddMethod(ut::make_unique<ProjectPositionGoToPoint>());
   AddMethod(ut::make_unique<ProjectPositionLevelTurnLeft>());
   AddMethod(ut::make_unique<ProjectPositionLevelTurnRight>());
   AddMethod(ut::make_unique<ProjectPositionSlice>());
   AddMethod(ut::make_unique<ProjectPositionSliceToHeading>("ProjectPositionSlice"));
   AddMethod(ut::make_unique<ProjectPositionSplitS>());

   // Groups Methods
   AddMethod(ut::make_unique<PerceivedThreatItems>());
   AddMethod(ut::make_unique<PerceivedGroups>());
   AddMethod(ut::make_unique<GetGroup>());
   AddMethod(ut::make_unique<RenameGroup>());
   AddMethod(ut::make_unique<MergeGroups>());
   AddMethod(ut::make_unique<SplitGroup>());
   AddMethod(ut::make_unique<CreateGroup>());
   AddMethod(ut::make_unique<DisbandGroup>());
   AddMethod(ut::make_unique<TransferEntity>());
   AddMethod(ut::make_unique<RemoveEntityFromGroup>());
   AddMethod(ut::make_unique<SetFocus>());
   AddMethod(ut::make_unique<SetGroupImportant>());
   AddMethod(ut::make_unique<SetGroupUnimportant>());
   AddMethod(ut::make_unique<GetFocus>());
   AddMethod(ut::make_unique<GetGroupImportance>());
   AddMethod(ut::make_unique<FocusedGroups>());
   AddMethod(ut::make_unique<UnfocusedGroups>());
   AddMethod(ut::make_unique<SortedGroups>());
   AddMethod(ut::make_unique<CalculatePerceivedItemCount>());
   AddMethod(ut::make_unique<PerceivedItemCount>());
   AddMethod(ut::make_unique<PerceivedThreatItemLimit>());

   // Cognitive Loading
   AddMethod(ut::make_unique<AddExtraCognitiveLoading>());

   // Weapons Methods
   AddMethod(ut::make_unique<GetSelectedWeapon>());
   AddMethod(ut::make_unique<SetSelectedWeapon>());
   AddMethod(ut::make_unique<SetMasterArm>());
   AddMethod(ut::make_unique<GetMasterArm>());
   AddMethod(ut::make_unique<WeaponBayDoorsAreOpen>());
   AddMethod(ut::make_unique<OpenWeaponBayDoors>());
   AddMethod(ut::make_unique<CloseWeaponBayDoors>());
   AddMethod(ut::make_unique<SupportingWeapon>());
   AddMethod(ut::make_unique<ShootCueActive>());
   AddMethod(ut::make_unique<ShootCueBlinking>());

   // Warnings/Cautions
   AddMethod(ut::make_unique<MasterWarningActive>());
   AddMethod(ut::make_unique<MasterCautionActive>());
   AddMethod(ut::make_unique<JammingDetected>());

   // Expendable Countermeasures Methods
   AddMethod(ut::make_unique<QuantityOfChaff>());
   AddMethod(ut::make_unique<QuantityOfFlares>());
   AddMethod(ut::make_unique<QuantityOfDecoys>());
   AddMethod(ut::make_unique<DispenseChaff>());
   AddMethod(ut::make_unique<DispenseFlare>());
   AddMethod(ut::make_unique<DispenseDecoy>());

   // Emissions Methods
   AddMethod(ut::make_unique<JammerEmitting>());
   AddMethod(ut::make_unique<RadarEmitting>());
   AddMethod(ut::make_unique<OtherSystemsEmitting>());

   // Fuel Methods
   AddMethod(ut::make_unique<FuelStateBingoReached>());
   AddMethod(ut::make_unique<FuelStateJokerReached>());
   AddMethod(ut::make_unique<FuelStateNormalized>());
   AddMethod(ut::make_unique<TimeToJoker>());
   AddMethod(ut::make_unique<TimeToBingo>());
   AddMethod(ut::make_unique<TimeToEmpty>());

   // Detection Methods
   AddMethod(ut::make_unique<ExpectedDetectionRangeAgainstTarget>());
   AddMethod(ut::make_unique<ExpectedDetectionRangeByTarget>());

   // WEZ Methods
   AddMethod(ut::make_unique<ExpectedWezRangeAgainstTarget>());
   AddMethod(ut::make_unique<ExpectedWezRangeByTarget>());

   // Track Processor Methods
   AddMethod(ut::make_unique<ESM_TrackProcessor>());
   AddMethod(ut::make_unique<MWS_TrackProcessor>());
   AddMethod(ut::make_unique<RadarTrackProcessor>());
   AddMethod(ut::make_unique<IRST_TrackProcessor>());
   AddMethod(ut::make_unique<DAS_TrackProcessor>());
   AddMethod(ut::make_unique<FLIR_TrackProcessor>());
   AddMethod(ut::make_unique<EyeTrackProcessor>());
   AddMethod(ut::make_unique<PerceptionMasterTrackProcessor>());

   // Geometry Methods
   AddMethod(ut::make_unique<AngleOffTarget2D>());
   AddMethod(ut::make_unique<AngleOffTarget2DSelf>());
   AddMethod(ut::make_unique<AngleOffTarget3D>());
   AddMethod(ut::make_unique<AngleOffTarget3DSelf>());
   AddMethod(ut::make_unique<AspectAngleForTarget2D>());
   AddMethod(ut::make_unique<AspectAngleForTarget2DSelf>());
   AddMethod(ut::make_unique<AspectAngleForTarget3D>());
   AddMethod(ut::make_unique<AspectAngleForTarget3DSelf>());

   // Get, Set, Update Interval Updates
   AddMethod(ut::make_unique<GetUpdateInterval>());
   AddMethod(ut::make_unique<SetUpdateInterval>());
   AddMethod(ut::make_unique<ResetUpdateInterval>());
   AddMethod(ut::make_unique<GetAssetDataPurgeLifetime>());
   AddMethod(ut::make_unique<SetAssetDataPurgeLifetime>());
   AddMethod(ut::make_unique<ResetAssetDataPurgeLifetime>());
   AddMethod(ut::make_unique<GetVisualPerceptionDelay>());
   AddMethod(ut::make_unique<SetVisualPerceptionDelay>());
   AddMethod(ut::make_unique<GetDisplayPerceptionDelay>());
   AddMethod(ut::make_unique<SetDisplayPerceptionDelay>());
   AddMethod(ut::make_unique<GetStartupInterval>());
}

void* WsfScriptSA_ProcessorClass::Clone(void* aObjectPtr)
{
   WsfSA_Processor* objPtr = static_cast<WsfSA_Processor*>(aObjectPtr);
   return objPtr->Clone();
}

void* WsfScriptSA_ProcessorClass::Create(const UtScriptContext& aContext)
{
   return new WsfSA_Processor(*WsfScriptContext::GetSCENARIO(aContext));
}

void WsfScriptSA_ProcessorClass::Destroy(void* aObjectPtr)
{
   WsfSA_Processor* objPtr = static_cast<WsfSA_Processor*>(aObjectPtr);
   delete objPtr;
}

UtScriptContext* WsfScriptSA_ProcessorClass::GetContext(void* aObjectPtr)
{
   UtScriptContext* contextPtr   = nullptr;
   WsfSA_Processor* processorPtr = static_cast<WsfSA_Processor*>(aObjectPtr);
   if (processorPtr != nullptr)
   {
      contextPtr = &(processorPtr->GetScriptContext().GetContext());
   }
   return contextPtr;
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, Risk, 0, "double", "")
{
   double risk = aObjectPtr->Assess()->Risk();
   aReturnVal.SetDouble(risk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SelfRisk, 0, "double", "")
{
   double risk = aObjectPtr->Assess()->SelfRisk();
   aReturnVal.SetDouble(risk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FlightRisk, 0, "double", "")
{
   double risk = aObjectPtr->Assess()->FlightRisk();
   aReturnVal.SetDouble(risk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PackageRisk, 0, "double", "")
{
   double risk = aObjectPtr->Assess()->PackageRisk();
   aReturnVal.SetDouble(risk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MissionRisk, 0, "double", "")
{
   double risk = aObjectPtr->Assess()->MissionRisk();
   aReturnVal.SetDouble(risk);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, Defensiveness, 0, "double", "")
{
   double defensiveness = aObjectPtr->Assess()->Defensiveness();
   aReturnVal.SetDouble(defensiveness);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, Urgency, 0, "double", "")
{
   double urgency = aObjectPtr->Assess()->Urgency();
   aReturnVal.SetDouble(urgency);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedAssets, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptAssetsPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& assets = aObjectPtr->Perceive()->PerceivedAssets(SIMULATION->GetSimTime());
   for (auto& asset : assets)
   {
      scriptAssetsPtr->emplace_back(new UtScriptRef(asset, classPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedAircraftAssets, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptAssetsPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& assets =
      aObjectPtr->Perceive()->PerceivedAircraftAssets(SIMULATION->GetSimTime());
   for (auto& asset : assets)
   {
      scriptAssetsPtr->emplace_back(new UtScriptRef(asset, classPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedMissileAssets, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptAssetsPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& assets =
      aObjectPtr->Perceive()->PerceivedMissileAssets(SIMULATION->GetSimTime());
   for (auto& asset : assets)
   {
      scriptAssetsPtr->emplace_back(new UtScriptRef(asset, classPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptAssetsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetImportant, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkAssetAsImportant(platformPtr->GetIndex());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetImportant_2, 1, "void", "WsfSA_EntityPerception")
{
   WsfSA_EntityPerception* assetPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (assetPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkAssetAsImportant(assetPtr->GetEntityPlatformIndex());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetTypeImportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->Perceive()->MarkAssetTypeAsImportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetTypeUnimportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->Perceive()->MarkAssetTypeAsUnimportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetUnimportant, 1, "void", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkAssetAsUnimportant(platformPtr->GetIndex());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetUnimportant_2, 1, "void", "WsfSA_EntityPerception")
{
   WsfSA_EntityPerception* assetPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (assetPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkAssetAsUnimportant(assetPtr->GetEntityPlatformIndex());
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestAsset, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestAsset(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestAircraftAsset, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestAircraftAsset(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestMissileAsset, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestMissileAsset(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MaxAssetLoad, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Perceive()->MaxAssetLoad());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetMaxAssetLoad, 1, "void", "int")
{
   int type = aVarArgs[0].GetInt();
   aObjectPtr->Perceive()->SetMaxAssetLoad(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetMaxAssetLoad, 0, "void", "")
{
   aObjectPtr->Perceive()->ResetMaxAssetLoad();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MaxThreatLoad, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Perceive()->MaxThreatLoad());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetMaxThreatLoad, 1, "void", "int")
{
   int type = aVarArgs[0].GetInt();
   aObjectPtr->Perceive()->SetMaxThreatLoad(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetMaxThreatLoad, 0, "void", "")
{
   aObjectPtr->Perceive()->ResetMaxThreatLoad();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MaxPrioritizedThreats, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Assess()->MaxPrioritizedThreats());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetMaxPrioritizedThreats, 1, "void", "int")
{
   int type = aVarArgs[0].GetInt();
   aObjectPtr->Assess()->SetMaxPrioritizedThreats(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetMaxPrioritizedThreats, 0, "void", "")
{
   aObjectPtr->Assess()->ResetMaxPrioritizedThreats();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MaxPrioritizedTargets, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->Assess()->MaxPrioritizedTargets());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetMaxPrioritizedTargets, 1, "void", "int")
{
   int type = aVarArgs[0].GetInt();
   aObjectPtr->Assess()->SetMaxPrioritizedTargets(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetMaxPrioritizedTargets, 0, "void", "")
{
   aObjectPtr->Assess()->ResetMaxPrioritizedTargets();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedBogies, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& bogieList =
      aObjectPtr->Perceive()->PerceivedBogies(SIMULATION->GetSimTime());
   for (auto& bogie : bogieList)
   {
      // Ensure that only bogies that are focused are allowed to be seen by the user.
      if (bogie->GetParentGroup() == nullptr || bogie->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bogie, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedAircraftBogies, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& bogieList =
      aObjectPtr->Perceive()->PerceivedAircraftBogies(SIMULATION->GetSimTime());
   for (auto& bogie : bogieList)
   {
      // Ensure that only bogies that are focused are allowed to be seen by the user.
      if (bogie->GetParentGroup() == nullptr || bogie->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bogie, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedMissileBogies, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& bogieList =
      aObjectPtr->Perceive()->PerceivedMissileBogies(SIMULATION->GetSimTime());
   for (auto& bogie : bogieList)
   {
      // Ensure that only bogies that are focused are allowed to be seen by the user.
      if (bogie->GetParentGroup() == nullptr || bogie->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bogie, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedBandits, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& banditList =
      aObjectPtr->Perceive()->PerceivedBandits(SIMULATION->GetSimTime());
   for (auto& bandit : banditList)
   {
      // Ensure that only bandits that are focused are allowed to be seen by the user.
      if (bandit->GetParentGroup() == nullptr || bandit->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bandit, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PerceivedAircraftBandits,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& banditList =
      aObjectPtr->Perceive()->PerceivedAircraftBandits(SIMULATION->GetSimTime());
   for (auto& bandit : banditList)
   {
      // Ensure that only bandits that are focused are allowed to be seen by the user.
      if (bandit->GetParentGroup() == nullptr || bandit->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bandit, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedMissileBandits, 0, "Array<WsfSA_EntityPerception>", "")
{
   std::vector<UtScriptData>* scriptTracksPtr = new std::vector<UtScriptData>();      // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   const std::vector<WsfSA_EntityPerception*>& banditList =
      aObjectPtr->Perceive()->PerceivedMissileBandits(SIMULATION->GetSimTime());
   for (auto& bandit : banditList)
   {
      // Ensure that only bandits that are focused are allowed to be seen by the user.
      if (bandit->GetParentGroup() == nullptr || bandit->GetParentGroup()->GetFocus())
      {
         scriptTracksPtr->emplace_back(new UtScriptRef(bandit, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptTracksPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetThreatImportant, 1, "void", "WsfLocalTrack")
{
   WsfLocalTrack* trackPtr = static_cast<WsfLocalTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (trackPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkThreatAsImportant(*trackPtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetThreatTypeImportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->Perceive()->MarkThreatTypeAsImportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetThreatTypeUnimportant, 1, "void", "string")
{
   std::string type = aVarArgs[0].GetString();
   aObjectPtr->Perceive()->MarkThreatTypeAsUnimportant(type);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetThreatUnimportant, 1, "void", "WsfLocalTrack")
{
   WsfLocalTrack* trackPtr = static_cast<WsfLocalTrack*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (trackPtr != nullptr)
   {
      aObjectPtr->Perceive()->MarkThreatAsUnimportant(*trackPtr);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestBogie, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestBogie();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestAircraftBogie, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestAircraftBogie();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestMissileBogie, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestMissileBogie();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestBandit, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestBandit();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestAircraftBandit, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestAircraftBandit();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, NearestMissileBandit, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Perceive()->NearestMissileBandit();
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PrioritizedThreatItems, 0, "Array<WsfSA_PerceivedItem>", "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();              // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_PerceivedItem"); // Type of object in array
   auto&          threats         = aObjectPtr->Assess()->GetPrioritizedThreatItems();
   for (auto& threat : threats)
   {
      if (threat.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedThreatEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          threats         = aObjectPtr->Assess()->GetPrioritizedThreatEntities();
   for (auto& threat : threats)
   {
      if (threat.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedAircraftThreatEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          threats         = aObjectPtr->Assess()->GetPrioritizedAircraftThreatEntities();
   for (auto& threat : threats)
   {
      if (threat.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedMissileThreatEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          threats         = aObjectPtr->Assess()->GetPrioritizedMissileThreatEntities();
   for (auto& threat : threats)
   {
      if (threat.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PrioritizedThreatGroups, 0, "Array<WsfSA_Group>", "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();      // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   auto&          threats         = aObjectPtr->Assess()->GetPrioritizedThreatGroups();
   for (auto& threat : threats)
   {
      if (threat.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, HighestThreat, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->HighestThreatEntity(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, HighestThreatAircraft, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->HighestThreatAircraft(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, HighestThreatMissile, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->HighestThreatMissile(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, HighestThreatGroup, 0, "WsfSA_Group", "")
{
   WsfSA_Group* assetPtr = aObjectPtr->Assess()->HighestThreatGroup(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PrioritizedTargetItems, 0, "Array<WsfSA_PerceivedItem>", "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();              // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_PerceivedItem"); // Type of object in array
   auto&          targets         = aObjectPtr->Assess()->GetPrioritizedTargetItems();
   for (auto& target : targets)
   {
      if (target.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(target.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedTargetEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          targets         = aObjectPtr->Assess()->GetPrioritizedTargetEntities();
   for (auto& target : targets)
   {
      if (target.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(target.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedAircraftTargetEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          targets         = aObjectPtr->Assess()->GetPrioritizedAircraftTargetEntities();
   for (auto& target : targets)
   {
      if (target.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(target.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        PrioritizedMissileTargetEntities,
                        0,
                        "Array<WsfSA_EntityPerception>",
                        "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();                 // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_EntityPerception"); // Type of object in array
   auto&          targets         = aObjectPtr->Assess()->GetPrioritizedMissileTargetEntities();
   for (auto& target : targets)
   {
      if (target.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(target.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PrioritizedTargetGroups, 0, "Array<WsfSA_Group>", "")
{
   auto           scriptGroupsPtr = ut::make_unique<ut::script::DataList>();      // This array is returned
   UtScriptClass* classPtr        = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   auto&          targets         = aObjectPtr->Assess()->GetPrioritizedTargetGroups();
   for (auto& target : targets)
   {
      if (target.IsValid())
      {
         scriptGroupsPtr->emplace_back(UtScriptRef::Ref(target.Get(), classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, BestTarget, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->BestTargetEntity(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, BestTargetAircraft, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->BestTargetAircraft(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, BestTargetMissile, 0, "WsfSA_EntityPerception", "")
{
   WsfSA_EntityPerception* assetPtr = aObjectPtr->Assess()->BestTargetMissile(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, BestTargetGroup, 0, "WsfSA_Group", "")
{
   WsfSA_Group* assetPtr = aObjectPtr->Assess()->BestTargetGroup(SIMULATION->GetSimTime());
   aReturnVal.SetPointer(new UtScriptRef(assetPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedThreatItems, 0, "Array<WsfSA_PerceivedItem>", "")
{
   std::vector<UtScriptData>* scriptGroupsPtr = new std::vector<UtScriptData>();   // This array is returned
   UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_PerceivedItem"); // Type of object in array
   std::vector<WsfSA_PerceivedItem*> threatList = aObjectPtr->Assess()->PerceivedThreatItems(SIMULATION->GetSimTime());
   for (auto& threat : threatList)
   {
      if (threat->GetItemType() == WsfSA_PerceivedItem::ENTITY)
      {
         // Ensure that only threat entities that are focused are allowed to be seen by the user.
         auto entity = dynamic_cast<WsfSA_EntityPerception*>(threat);
         if (entity->GetParentGroup() == nullptr || entity->GetParentGroup()->GetFocus())
         {
            scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat, classPtr));
         }
      }
      else
      {
         // Unfocused groups are counted as PIs.
         auto group = dynamic_cast<WsfSA_Group*>(threat);
         if (!group->GetFocus())
         {
            scriptGroupsPtr->emplace_back(UtScriptRef::Ref(threat, classPtr));
         }
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedGroups, 0, "Array<WsfSA_Group>", "")
{
   std::vector<UtScriptData>*    scriptGroupsPtr = new std::vector<UtScriptData>();       // This array is returned
   UtScriptClass*                classPtr = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   const std::list<WsfSA_Group*> groups   = aObjectPtr->Assess()->PerceivedGroups();
   for (auto& group : groups)
   {
      scriptGroupsPtr->emplace_back(new UtScriptRef(group, classPtr));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionInTime,
                        2,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionInTime(aVarArgs[0].GetDouble(), *entityPtr));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionForward,
                        2,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionForward(aVarArgs[0].GetDouble(), *entityPtr));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionTurnToHeading,
                        4,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double, double")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionTurnToHeading(aVarArgs[0].GetDouble(),
                                                                                 *entityPtr,
                                                                                 aVarArgs[2].GetDouble(),
                                                                                 aVarArgs[3].GetDouble()));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionGoToPoint,
                        4,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, WsfGeoPoint, double")
{
   auto entityPtr   = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());
   auto targetPoint = static_cast<WsfGeoPoint*>(aVarArgs[2].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionGoToPoint(aVarArgs[0].GetDouble(),
                                                                             *entityPtr,
                                                                             *targetPoint,
                                                                             aVarArgs[3].GetDouble()));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionLevelTurnLeft,
                        3,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double")
{
   auto entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());
   auto newPoint  = ut::clone(aObjectPtr->Predict()->ProjectPositionLevelTurn(aVarArgs[0].GetDouble(),
                                                                             *entityPtr,
                                                                             aVarArgs[2].GetDouble(),
                                                                             TurnDirection::cLEFT));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionLevelTurnRight,
                        3,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double")
{
   auto entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());
   auto newPoint  = ut::clone(aObjectPtr->Predict()->ProjectPositionLevelTurn(aVarArgs[0].GetDouble(),
                                                                             *entityPtr,
                                                                             aVarArgs[2].GetDouble(),
                                                                             TurnDirection::cRIGHT));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionSliceToHeading,
                        5,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double, double, double")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionRollAndPull(aVarArgs[0].GetDouble(),
                                                                               *entityPtr,
                                                                               aVarArgs[2].GetDouble(),
                                                                               aVarArgs[3].GetDouble(),
                                                                               aVarArgs[4].GetDouble()));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionSlice,
                        4,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double, double")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(aObjectPtr->Predict()->ProjectPositionRollAndPull(aVarArgs[0].GetDouble(),
                                                                               *entityPtr,
                                                                               aVarArgs[2].GetDouble(),
                                                                               aVarArgs[3].GetDouble()));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass,
                        WsfSA_Processor,
                        ProjectPositionSplitS,
                        3,
                        "WsfGeoPoint",
                        "double, WsfSA_EntityPerception, double")
{
   WsfSA_EntityPerception* entityPtr = static_cast<WsfSA_EntityPerception*>(aVarArgs[1].GetPointer()->GetAppObject());

   auto newPoint = ut::clone(
      aObjectPtr->Predict()->ProjectPositionSplitS(aVarArgs[0].GetDouble(), *entityPtr, aVarArgs[2].GetDouble()));

   aReturnVal.SetPointer(new UtScriptRef(newPoint.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetGroup, 1, "WsfSA_Group", "string")
{
   std::string  groupName = aVarArgs[0].GetString();
   WsfSA_Group* groupPtr  = aObjectPtr->Assess()->GetGroup(groupName);
   aReturnVal.SetPointer(new UtScriptRef(groupPtr, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, RenameGroup, 2, "bool", "WsfSA_Group, string")
{
   auto        group     = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::string groupName = aVarArgs[1].GetString();
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      aReturnVal.SetBool(aObjectPtr->Perceive()->RenameGroup(group, groupName));
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MergeGroups, 1, "WsfSA_Group", "Array<WsfSA_Group>")
{
   std::vector<UtScriptData>* input = static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::list<WsfSA_Group*>    groups;
   for (auto& data : *input)
   {
      auto group = static_cast<WsfSA_Group*>(data.GetPointer()->GetAppObject());
      if (group && data.GetPointer()->IsValid())
      {
         groups.emplace_back(static_cast<WsfSA_Group*>(group));
      }
      else
      {
         groups.emplace_back(nullptr);
      }
   }
   auto ret = aObjectPtr->Perceive()->MergeGroups(groups, true);
   aReturnVal.SetPointer(new UtScriptRef(ret, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SplitGroup, 3, "Array<WsfSA_Group>", "WsfSA_Group, int, string")
{
   auto                       group           = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<UtScriptData>* scriptGroupsPtr = new std::vector<UtScriptData>(); // This array is returned
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      const auto newGroups =
         aObjectPtr->Perceive()->SplitGroup(group,
                                            aVarArgs[1].GetInt(),
                                            WsfSA_GroupUtils::StringToSplitCriteria(aVarArgs[2].GetString()),
                                            true);
      if (newGroups.first)
      {
         // If one of them is valid, then they both are.
         UtScriptClass* classPtr = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
         scriptGroupsPtr->emplace_back(new UtScriptRef(newGroups.first, classPtr));
         scriptGroupsPtr->emplace_back(new UtScriptRef(newGroups.second, classPtr));
      }
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, CreateGroup, 1, "WsfSA_Group", "Array<WsfSA_EntityPerception>")
{
   std::vector<UtScriptData>* input = static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::list<WsfSA_EntityPerception*> entities;
   for (auto& entity : *input)
   {
      entities.emplace_back(static_cast<WsfSA_EntityPerception*>(entity.GetPointer()->GetAppObject()));
   }
   auto ret = aObjectPtr->Perceive()->CreateGroup(entities, true);
   aReturnVal.SetPointer(new UtScriptRef(ret, aReturnClassPtr));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, DisbandGroup, 1, "bool", "WsfSA_Group")
{
   auto group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool ret   = false;
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      ret = aObjectPtr->Perceive()->DisbandGroup(group, true);
   }
   aReturnVal.SetBool(ret);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, TransferEntity, 2, "bool", "WsfSA_EntityPerception, WsfSA_Group")
{
   auto entity = static_cast<WsfSA_EntityPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   auto group  = static_cast<WsfSA_Group*>(aVarArgs[1].GetPointer()->GetAppObject());
   bool ret    = false;
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      ret = aObjectPtr->Perceive()->TransferEntity(entity, group, true);
   }
   aReturnVal.SetBool(ret);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, RemoveEntityFromGroup, 1, "bool", "WsfSA_EntityPerception")
{
   auto entity = static_cast<WsfSA_EntityPerception*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool ret    = false;
   if (entity && aVarArgs[0].GetPointer()->IsValid())
   {
      ret = aObjectPtr->Perceive()->RemoveEntityFromGroup(entity, true);
   }
   aReturnVal.SetBool(ret);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetFocus, 2, "bool", "WsfSA_Group, bool")
{
   WsfSA_Group* group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         focus = aVarArgs[1].GetBool();
   bool         ret   = false;
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      ret = aObjectPtr->Perceive()->SetFocus(group, focus, true);
   }
   aReturnVal.SetBool(ret);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetGroupImportant, 1, "void", "WsfSA_Group")
{
   auto group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      aObjectPtr->Perceive()->SetImportant(group, true);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetGroupUnimportant, 1, "void", "WsfSA_Group")
{
   auto group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      aObjectPtr->Perceive()->SetImportant(group, false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetFocus, 1, "bool", "WsfSA_Group")
{
   WsfSA_Group* group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   bool         ret   = false;
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      ret = aObjectPtr->Perceive()->GetFocus(group);
   }
   aReturnVal.SetBool(ret);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetGroupImportance, 1, "bool", "WsfSA_Group")
{
   auto group = static_cast<WsfSA_Group*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (group && aVarArgs[0].GetPointer()->IsValid())
   {
      aReturnVal.SetBool(aObjectPtr->Perceive()->GetImportant(group));
   }
   else
   {
      aReturnVal.SetBool(false);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FocusedGroups, 0, "Array<WsfSA_Group>", "")
{
   std::vector<UtScriptData>*    scriptGroupsPtr = new std::vector<UtScriptData>();       // This array is returned
   UtScriptClass*                classPtr = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   const std::list<WsfSA_Group*> groups   = aObjectPtr->Assess()->FocusedGroups();
   for (auto& group : groups)
   {
      scriptGroupsPtr->emplace_back(UtScriptData(new UtScriptRef(group, classPtr)));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, UnfocusedGroups, 0, "Array<WsfSA_Group>", "")
{
   std::vector<UtScriptData>*    scriptGroupsPtr = new std::vector<UtScriptData>();       // This array is returned
   UtScriptClass*                classPtr = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   const std::list<WsfSA_Group*> groups   = aObjectPtr->Assess()->UnfocusedGroups();
   for (auto& group : groups)
   {
      scriptGroupsPtr->emplace_back(UtScriptData(new UtScriptRef(group, classPtr)));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SortedGroups, 1, "Array<WsfSA_Group>", "string")
{
   std::vector<UtScriptData>* scriptGroupsPtr = new std::vector<UtScriptData>();              // This array is returned
   UtScriptClass*             classPtr        = aContext.GetTypes()->GetClass("WsfSA_Group"); // Type of object in array
   std::vector<WsfSA_Group*>  groups;
   aObjectPtr->Assess()->SortedGroups(WsfSA_GroupUtils::StringToGroupSorting(aVarArgs[0].GetString()), groups);
   for (auto& group : groups)
   {
      scriptGroupsPtr->emplace_back(UtScriptData(new UtScriptRef(group, classPtr)));
   }
   aReturnVal.SetPointer(new UtScriptRef(scriptGroupsPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, CalculatePerceivedItemCount, 1, "int", "Array<WsfSA_PerceivedItem>")
{
   std::vector<UtScriptData>* input = static_cast<std::vector<UtScriptData>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::list<WsfSA_PerceivedItem*> items;
   for (auto& item : *input)
   {
      items.emplace_back(static_cast<WsfSA_EntityPerception*>(item.GetPointer()->GetAppObject()));
   }
   aReturnVal.SetInt(WsfSA_GroupUtils::CalculateNumPIs(items));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedItemCount, 0, "int", "")
{
   aReturnVal.SetInt(WsfSA_GroupUtils::CalculateNumPIs(aObjectPtr->GetData()->mCurrentlyPerceivedGroups));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceivedThreatItemLimit, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetData()->mMaxThreatLoad);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AddExtraCognitiveLoading, 1, "void", "double")
{
   double duration = aVarArgs[0].GetDouble();
   aObjectPtr->Perceive()->AddExtraCognitiveLoading(duration);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetSelectedWeapon, 0, "string", "")
{
   std::string weaponName = aObjectPtr->GetSelectedWeapon();
   aReturnVal.SetString(weaponName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetSelectedWeapon, 1, "void", "string")
{
   std::string weaponName = aVarArgs[0].GetString();
   aObjectPtr->SetSelectedWeapon(weaponName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetMasterArm, 1, "void", "bool")
{
   bool armSetting = aVarArgs[0].GetBool();
   aObjectPtr->SetMasterArm(armSetting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetMasterArm, 0, "bool", "")
{
   bool armSetting = aObjectPtr->GetMasterArm();
   aReturnVal.SetBool(armSetting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, WeaponBayDoorsAreOpen, 0, "bool", "")
{
   bool condition = aObjectPtr->WeaponBayDoorsAreOpen();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, OpenWeaponBayDoors, 0, "void", "")
{
   aObjectPtr->Perceive()->SetWeaponBayDoorsAreOpen(true);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, CloseWeaponBayDoors, 0, "void", "")
{
   aObjectPtr->Perceive()->SetWeaponBayDoorsAreOpen(false);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SupportingWeapon, 0, "bool", "")
{
   bool supporting = aObjectPtr->Assess()->SupportingWeapon();
   aReturnVal.SetBool(supporting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ShootCueActive, 0, "bool", "")
{
   bool condition = aObjectPtr->ShootCueActive();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ShootCueBlinking, 0, "bool", "")
{
   bool condition = aObjectPtr->ShootCueBlinking();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MasterWarningActive, 0, "bool", "")
{
   bool condition = aObjectPtr->MasterWarningActive();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MasterCautionActive, 0, "bool", "")
{
   bool condition = aObjectPtr->MasterCautionActive();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, JammingDetected, 0, "bool", "")
{
   bool condition = aObjectPtr->JammingDetected();
   aReturnVal.SetBool(condition);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, QuantityOfChaff, 0, "int", "")
{
   int num = aObjectPtr->QuantityOfChaff();
   aReturnVal.SetInt(num);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, QuantityOfFlares, 0, "int", "")
{
   int num = aObjectPtr->QuantityOfFlares();
   aReturnVal.SetInt(num);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, QuantityOfDecoys, 0, "int", "")
{
   int num = aObjectPtr->QuantityOfDecoys();
   aReturnVal.SetInt(num);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, DispenseChaff, 0, "void", "")
{
   aObjectPtr->DispenseChaff(TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, DispenseFlare, 0, "void", "")
{
   aObjectPtr->DispenseFlare(TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, DispenseDecoy, 0, "void", "")
{
   aObjectPtr->DispenseDecoy(TIME_NOW);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, RadarEmitting, 0, "bool", "")
{
   bool emitting = aObjectPtr->RadarEmitting();
   aReturnVal.SetBool(emitting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, JammerEmitting, 0, "bool", "")
{
   bool emitting = aObjectPtr->JammerEmitting();
   aReturnVal.SetBool(emitting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, OtherSystemsEmitting, 0, "bool", "")
{
   bool emitting = aObjectPtr->OtherSystemsEmitting();
   aReturnVal.SetBool(emitting);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FuelStateBingoReached, 0, "bool", "")
{
   bool bingoReached = aObjectPtr->FuelStateBingoReached();
   aReturnVal.SetBool(bingoReached);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FuelStateJokerReached, 0, "bool", "")
{
   bool jokerReached = aObjectPtr->FuelStateJokerReached();
   aReturnVal.SetBool(jokerReached);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FuelStateNormalized, 0, "double", "")
{
   double range = aObjectPtr->FuelStateNormalized();
   aReturnVal.SetDouble(range);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, TimeToJoker, 0, "double", "")
{
   double range = aObjectPtr->TimeToJoker();
   aReturnVal.SetDouble(range);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, TimeToBingo, 0, "double", "")
{
   double range = aObjectPtr->TimeToBingo();
   aReturnVal.SetDouble(range);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, TimeToEmpty, 0, "double", "")
{
   double range = aObjectPtr->TimeToEmpty();
   aReturnVal.SetDouble(range);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ExpectedDetectionRangeAgainstTarget, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double range = aObjectPtr->Assess()->ExpectedDetectionRangeAgainstTarget(platformPtr->GetIndex());
      aReturnVal.SetDouble(range);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ExpectedDetectionRangeByTarget, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double range = aObjectPtr->Assess()->ExpectedDetectionRangeByTarget(platformPtr->GetIndex());
      aReturnVal.SetDouble(range);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ExpectedWezRangeAgainstTarget, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double range = aObjectPtr->Assess()->ExpectedWezRangeAgainstTarget(platformPtr->GetIndex());
      aReturnVal.SetDouble(range);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ExpectedWezRangeByTarget, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double range = aObjectPtr->Assess()->ExpectedWezRangeByTarget(platformPtr->GetIndex());
      aReturnVal.SetDouble(range);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ESM_TrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->ESM_TrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, MWS_TrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->MWS_TrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, RadarTrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->RadarTrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, IRST_TrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->IRST_TrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, DAS_TrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->DAS_TrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, FLIR_TrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->FLIR_TrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, EyeTrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->EyesTrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, PerceptionMasterTrackProcessor, 0, "string", "")
{
   std::string trackProcessorName = aObjectPtr->PerceptionMasterTrackProcessor();
   aReturnVal.SetString(trackProcessorName);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AngleOffTarget2D, 2, "double", "WsfPlatform, WsfPlatform")
{
   WsfPlatform* platformPtr1 = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* platformPtr2 = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   if ((platformPtr1 != nullptr) && (platformPtr2 != nullptr))
   {
      double range = aObjectPtr->Assess()->AngleOffTarget2D(*platformPtr1, *platformPtr2) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(range);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AngleOffTarget2DSelf, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double angle = aObjectPtr->Assess()->AngleOffTarget2D(*platformPtr) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AngleOffTarget3D, 2, "double", "WsfPlatform, WsfPlatform")
{
   WsfPlatform* platformPtr1 = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* platformPtr2 = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   if ((platformPtr1 != nullptr) && (platformPtr2 != nullptr))
   {
      double angle = aObjectPtr->Assess()->AngleOffTarget3D(*platformPtr1, *platformPtr2) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AngleOffTarget3DSelf, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double angle = aObjectPtr->Assess()->AngleOffTarget3D(*platformPtr) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AspectAngleForTarget2D, 2, "double", "WsfPlatform, WsfPlatform")
{
   WsfPlatform* platformPtr1 = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* platformPtr2 = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   if ((platformPtr1 != nullptr) && (platformPtr2 != nullptr))
   {
      double angle = aObjectPtr->Assess()->AspectAngleForTarget2D(*platformPtr1, *platformPtr2) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AspectAngleForTarget2DSelf, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double angle = aObjectPtr->Assess()->AspectAngleForTarget2D(*platformPtr) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AspectAngleForTarget3D, 2, "double", "WsfPlatform, WsfPlatform")
{
   WsfPlatform* platformPtr1 = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   WsfPlatform* platformPtr2 = static_cast<WsfPlatform*>(aVarArgs[1].GetPointer()->GetAppObject());
   if ((platformPtr1 != nullptr) && (platformPtr2 != nullptr))
   {
      double angle = aObjectPtr->Assess()->AspectAngleForTarget3D(*platformPtr1, *platformPtr2) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, AspectAngleForTarget3DSelf, 1, "double", "WsfPlatform")
{
   WsfPlatform* platformPtr = static_cast<WsfPlatform*>(aVarArgs[0].GetPointer()->GetAppObject());
   if (platformPtr != nullptr)
   {
      double angle = aObjectPtr->Assess()->AspectAngleForTarget3D(*platformPtr) * UtMath::cDEG_PER_RAD;
      aReturnVal.SetDouble(angle);
   }
   else
   {
      aReturnVal.SetDouble(0.0);
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetUpdateInterval, 1, "double", "string")
{
   double                                   returnVal     = -1.0; // init for failure
   std::string                              updateTypeStr = aVarArgs[0].GetString();
   WsfSA_Processor::UpdateType              updateTypeEnum; // for single update type
   std::vector<WsfSA_Processor::UpdateType> updateTypeVec;  // for grouped update types

   if (WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(updateTypeStr, updateTypeEnum))
   {
      returnVal = aObjectPtr->GetUpdateInterval(updateTypeEnum);
   }
   else if (WsfScriptSA_ProcessorClass::GetGroupedTypes(updateTypeStr, updateTypeVec))
   {
      // check to make sure all of the values are the same
      returnVal = aObjectPtr->GetUpdateInterval(updateTypeVec[0]); // expected common value
      for (auto updateType : updateTypeVec)
      {
         if (aObjectPtr->GetUpdateInterval(updateType) != returnVal)
         {
            ut::log::error() << "Mismatch among group settings in call to WSF_SA_PROCESSOR::GetUpdateInterval("
                             << updateTypeStr << "); returning -1.";
            returnVal = -1.0;
            break;
         }
      }
   }
   else
   {
      ut::log::error() << "Invalid aUpdateIntervalName value in call to WSF_SA_PROCESSOR::GetUpdateInterval("
                       << updateTypeStr << "); returning -1.";
   }
   aReturnVal.SetDouble(returnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetUpdateInterval, 2, "bool", "string, double")
{
   bool                                     success        = false; // init for failure
   std::string                              updateTypeStr  = aVarArgs[0].GetString();
   std::double_t                            updateInterval = aVarArgs[1].GetDouble();
   WsfSA_Processor::UpdateType              updateTypeEnum; // for single update type
   std::vector<WsfSA_Processor::UpdateType> updateTypeVec;  // for grouped update types

   if (WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(updateTypeStr, updateTypeEnum))
   {
      success = aObjectPtr->SetUpdateInterval(updateTypeEnum, updateInterval);
      if (!success)
      {
         ut::log::error() << "Invalid aUpdateIntervalSeconds in call to WSF_SA_PROCESSOR::SetUpdateInterval("
                          << updateTypeStr << ", " << updateInterval << ").";
      }
   }
   else if (WsfScriptSA_ProcessorClass::GetGroupedTypes(updateTypeStr, updateTypeVec))
   {
      for (auto updateType : updateTypeVec)
      {
         success = aObjectPtr->SetUpdateInterval(updateType, updateInterval);
         if (!success)
         {
            ut::log::error() << "Invalid aUpdateIntervalSeconds in call to WSF_SA_PROCESSOR::SetUpdateInterval("
                             << updateTypeStr << ", " << updateInterval << ").";
            break; // stop on first instance of invalid updateInterval since all calls will fail
         }
      }
   }
   else
   {
      ut::log::error() << "Invalid aUpdateIntervalName value in call to WSF_SA_PROCESSOR::SetUpdateInterval("
                       << updateTypeStr << ").";
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetUpdateInterval, 1, "void", "string")
{
   std::string                              updateTypeStr = aVarArgs[0].GetString();
   WsfSA_Processor::UpdateType              updateTypeEnum; // for single update type
   std::vector<WsfSA_Processor::UpdateType> updateTypeVec;  // for grouped update types

   if (WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(updateTypeStr, updateTypeEnum))
   {
      aObjectPtr->ResetUpdateInterval(updateTypeEnum);
   }
   else if (WsfScriptSA_ProcessorClass::GetGroupedTypes(updateTypeStr, updateTypeVec))
   {
      for (auto updateType : updateTypeVec)
      {
         aObjectPtr->ResetUpdateInterval(updateType);
      }
   }
   else
   {
      ut::log::error() << "Invalid aUpdateIntervalName value in call to WSF_SA_PROCESSOR::ResetUpdateInterval("
                       << updateTypeStr << ").";
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetAssetDataPurgeLifetime, 0, "double", "")
{
   double assetPurgeLifetime = aObjectPtr->GetAssetDataPurgeLifetime();
   aReturnVal.SetDouble(assetPurgeLifetime);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetAssetDataPurgeLifetime, 1, "bool", "double")
{
   std::double_t assetPurgeLifetime = aVarArgs[0].GetDouble();
   bool          success            = aObjectPtr->SetAssetDataPurgeLifetime(assetPurgeLifetime);
   if (!success)
   {
      ut::log::error() << "Invalid aAssetPurgeLifetime in call to WSF_SA_PROCESSOR::SetAssetDataPurgeLifetime("
                       << assetPurgeLifetime << ").";
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, ResetAssetDataPurgeLifetime, 0, "void", "")
{
   aObjectPtr->ResetAssetDataPurgeLifetime();
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetStartupInterval, 1, "double", "string")
{
   double                                   returnVal     = -1.0; // init for failure
   std::string                              updateTypeStr = aVarArgs[0].GetString();
   WsfSA_Processor::UpdateType              updateTypeEnum; // for single update type
   std::vector<WsfSA_Processor::UpdateType> updateTypeVec;  // for grouped update types

   if (WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(updateTypeStr, updateTypeEnum))
   {
      returnVal = aObjectPtr->GetStartupInterval(updateTypeEnum);
   }
   else if (WsfScriptSA_ProcessorClass::GetGroupedTypes(updateTypeStr, updateTypeVec))
   {
      // check to make sure all of the values are the same
      returnVal = aObjectPtr->GetStartupInterval(updateTypeVec[0]); // expected common value
      for (auto updateType : updateTypeVec)
      {
         if (aObjectPtr->GetStartupInterval(updateType) != returnVal)
         {
            ut::log::error() << "Mismatch among group settings in call to WSF_SA_PROCESSOR::GetStartupInterval("
                             << updateTypeStr << "); returning -1.";
            returnVal = -1.0;
            break;
         }
      }
   }
   else
   {
      ut::log::error() << "Invalid aUpdateIntervalName value in call to WSF_SA_PROCESSOR::GetStartupInterval("
                       << updateTypeStr << "); returning -1.";
   }
   aReturnVal.SetDouble(returnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetVisualPerceptionDelay, 0, "double", "")
{
   double returnVal = aObjectPtr->GetVisualPerceptionDelay();
   aReturnVal.SetDouble(returnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, GetDisplayPerceptionDelay, 0, "double", "")
{
   double returnVal = aObjectPtr->GetDisplayPerceptionDelay();
   aReturnVal.SetDouble(returnVal);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetVisualPerceptionDelay, 1, "bool", "double")
{
   std::double_t visualPerceptionDelay = aVarArgs[0].GetDouble();
   bool          success               = aObjectPtr->SetVisualPerceptionDelay(visualPerceptionDelay);
   if (!success)
   {
      ut::log::error() << "WSF_SA_PROCESSOR method SetVisualPerceptionDelay() called with invalid setting = "
                       << visualPerceptionDelay;
   }
   aReturnVal.SetBool(success);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptSA_ProcessorClass, WsfSA_Processor, SetDisplayPerceptionDelay, 1, "bool", "double")
{
   std::double_t displayPerceptionDelay = aVarArgs[0].GetDouble();
   bool          success                = aObjectPtr->SetDisplayPerceptionDelay(displayPerceptionDelay);
   if (!success)
   {
      ut::log::error() << "WSF_SA_PROCESSOR method SetDisplayPerceptionDelay() called with invalid setting = "
                       << displayPerceptionDelay;
   }
   aReturnVal.SetBool(success);
}
bool WsfScriptSA_ProcessorClass::GetUpdateTypeEnum(const std::string&           aUpdateTypeStr,
                                                   WsfSA_Processor::UpdateType& aUpdateType)
{
   bool success = true; // init for success

   // Note: the updateTypeStr parameter matches the corresponding update interval command for scenario input
   if (aUpdateTypeStr == "report_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cSTATUS;
   }
   else if (aUpdateTypeStr == "engagement_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cENGAGEMENT_DATA;
   }
   else if (aUpdateTypeStr == "flight_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cFLIGHT_DATA;
   }
   else if (aUpdateTypeStr == "fuel_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cFUEL_DATA;
   }
   else if (aUpdateTypeStr == "nav_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cNAV_DATA;
   }
   else if (aUpdateTypeStr == "flight_controls_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cCONTROLS_DATA;
   }
   else if (aUpdateTypeStr == "weapons_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cWEAPONS_DATA;
   }
   else if (aUpdateTypeStr == "track_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cTRACK_DATA;
   }
   else if (aUpdateTypeStr == "asset_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cASSETS_DATA;
   }
   else if (aUpdateTypeStr == "perceived_item_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA;
   }
   else if (aUpdateTypeStr == "prioritized_item_data_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA;
   }
   else if (aUpdateTypeStr == "perceived_item_calculation_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION;
   }
   else if (aUpdateTypeStr == "prioritized_item_calculation_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION;
   }
   else if (aUpdateTypeStr == "behavior_calculation_update_interval")
   {
      aUpdateType = WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION;
   }
   else
   {
      success = false;
   }
   return success;
}

bool WsfScriptSA_ProcessorClass::GetGroupedTypes(const std::string&                        aUpdateGroupStr,
                                                 std::vector<WsfSA_Processor::UpdateType>& aGroupUpdateTypesVec)
{
   bool success = true; // init for success

   if (aUpdateGroupStr == "cognitive_update_interval")
   {
      aGroupUpdateTypesVec = WsfSA_Processor::GetCognitiveUpdateGroup();
   }
   else if (aUpdateGroupStr == "platform_update_interval")
   {
      aGroupUpdateTypesVec = WsfSA_Processor::GetPlatformUpdateGroup();
   }
   else if (aUpdateGroupStr == "universal_update_interval")
   {
      aGroupUpdateTypesVec = WsfSA_Processor::GetUniversalUpdateGroup();
   }
   else
   {
      success = false;
   }
   return success;
}
