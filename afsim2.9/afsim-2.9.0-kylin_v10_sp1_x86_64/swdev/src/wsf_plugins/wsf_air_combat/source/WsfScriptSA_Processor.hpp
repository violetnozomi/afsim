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

#ifndef WSFSCRIPT_SA_PROCESSOR_HPP
#define WSFSCRIPT_SA_PROCESSOR_HPP

#include "wsf_air_combat_export.h"

#include <string>

#include "WsfSA_Processor.hpp"
#include "script/WsfScriptProcessorClass.hpp"

class WSF_AIR_COMBAT_EXPORT WsfScriptSA_ProcessorClass : public WsfScriptProcessorClass
{
public:
   WsfScriptSA_ProcessorClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   void*            Clone(void* aObjectPtr) override;
   void*            Create(const UtScriptContext& aContext) override;
   void             Destroy(void* aObjectPtr) override;
   UtScriptContext* GetContext(void* aObjectPtr) override;

   // Assessment Methods
   UT_DECLARE_SCRIPT_METHOD(Risk);
   UT_DECLARE_SCRIPT_METHOD(SelfRisk);
   UT_DECLARE_SCRIPT_METHOD(FlightRisk);
   UT_DECLARE_SCRIPT_METHOD(PackageRisk);
   UT_DECLARE_SCRIPT_METHOD(MissionRisk);
   UT_DECLARE_SCRIPT_METHOD(Defensiveness);
   UT_DECLARE_SCRIPT_METHOD(Urgency);

   // Asset Methods
   UT_DECLARE_SCRIPT_METHOD(PerceivedAssets);
   UT_DECLARE_SCRIPT_METHOD(PerceivedAircraftAssets);
   UT_DECLARE_SCRIPT_METHOD(PerceivedMissileAssets);
   UT_DECLARE_SCRIPT_METHOD(SetAssetImportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetImportant_2);
   UT_DECLARE_SCRIPT_METHOD(SetAssetTypeImportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetTypeUnimportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetUnimportant);
   UT_DECLARE_SCRIPT_METHOD(SetAssetUnimportant_2);
   UT_DECLARE_SCRIPT_METHOD(NearestAsset);
   UT_DECLARE_SCRIPT_METHOD(NearestAircraftAsset);
   UT_DECLARE_SCRIPT_METHOD(NearestMissileAsset);

   UT_DECLARE_SCRIPT_METHOD(MaxAssetLoad);
   UT_DECLARE_SCRIPT_METHOD(SetMaxAssetLoad);
   UT_DECLARE_SCRIPT_METHOD(ResetMaxAssetLoad);
   UT_DECLARE_SCRIPT_METHOD(MaxThreatLoad);
   UT_DECLARE_SCRIPT_METHOD(SetMaxThreatLoad);
   UT_DECLARE_SCRIPT_METHOD(ResetMaxThreatLoad);
   UT_DECLARE_SCRIPT_METHOD(MaxPrioritizedThreats);
   UT_DECLARE_SCRIPT_METHOD(SetMaxPrioritizedThreats);
   UT_DECLARE_SCRIPT_METHOD(ResetMaxPrioritizedThreats);
   UT_DECLARE_SCRIPT_METHOD(MaxPrioritizedTargets);
   UT_DECLARE_SCRIPT_METHOD(SetMaxPrioritizedTargets);
   UT_DECLARE_SCRIPT_METHOD(ResetMaxPrioritizedTargets);

   // Bogie/Bandit Methods
   UT_DECLARE_SCRIPT_METHOD(PerceivedBogies);
   UT_DECLARE_SCRIPT_METHOD(PerceivedAircraftBogies);
   UT_DECLARE_SCRIPT_METHOD(PerceivedMissileBogies);
   UT_DECLARE_SCRIPT_METHOD(PerceivedBandits);
   UT_DECLARE_SCRIPT_METHOD(PerceivedAircraftBandits);
   UT_DECLARE_SCRIPT_METHOD(PerceivedMissileBandits);
   UT_DECLARE_SCRIPT_METHOD(SetThreatImportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatTypeImportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatTypeUnimportant);
   UT_DECLARE_SCRIPT_METHOD(SetThreatUnimportant);
   UT_DECLARE_SCRIPT_METHOD(NearestBogie);
   UT_DECLARE_SCRIPT_METHOD(NearestAircraftBogie);
   UT_DECLARE_SCRIPT_METHOD(NearestMissileBogie);
   UT_DECLARE_SCRIPT_METHOD(NearestBandit);
   UT_DECLARE_SCRIPT_METHOD(NearestAircraftBandit);
   UT_DECLARE_SCRIPT_METHOD(NearestMissileBandit);

   // Prioritized Threats Methods
   UT_DECLARE_SCRIPT_METHOD(PrioritizedThreatItems);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedThreatEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedAircraftThreatEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedMissileThreatEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedThreatGroups);
   UT_DECLARE_SCRIPT_METHOD(HighestThreat);
   UT_DECLARE_SCRIPT_METHOD(HighestThreatAircraft);
   UT_DECLARE_SCRIPT_METHOD(HighestThreatMissile);
   UT_DECLARE_SCRIPT_METHOD(HighestThreatGroup);

   // Prioritized Targets Methods
   UT_DECLARE_SCRIPT_METHOD(PrioritizedTargetItems);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedTargetEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedAircraftTargetEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedMissileTargetEntities);
   UT_DECLARE_SCRIPT_METHOD(PrioritizedTargetGroups);
   UT_DECLARE_SCRIPT_METHOD(BestTarget);
   UT_DECLARE_SCRIPT_METHOD(BestTargetAircraft);
   UT_DECLARE_SCRIPT_METHOD(BestTargetMissile);
   UT_DECLARE_SCRIPT_METHOD(BestTargetGroup);

   // Generic Prediction Methods
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionInTime);

   // Prediction Primitives
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionForward);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionTurnToHeading);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionGoToPoint);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionLevelTurnLeft);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionLevelTurnRight);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionSlice);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionSliceToHeading);
   UT_DECLARE_SCRIPT_METHOD(ProjectPositionSplitS);

   // Groups Methods
   UT_DECLARE_SCRIPT_METHOD(PerceivedThreatItems);
   UT_DECLARE_SCRIPT_METHOD(PerceivedGroups);
   UT_DECLARE_SCRIPT_METHOD(GetGroup);
   UT_DECLARE_SCRIPT_METHOD(RenameGroup);
   UT_DECLARE_SCRIPT_METHOD(MergeGroups);
   UT_DECLARE_SCRIPT_METHOD(SplitGroup);
   UT_DECLARE_SCRIPT_METHOD(CreateGroup);
   UT_DECLARE_SCRIPT_METHOD(DisbandGroup);
   UT_DECLARE_SCRIPT_METHOD(TransferEntity);
   UT_DECLARE_SCRIPT_METHOD(RemoveEntityFromGroup);
   UT_DECLARE_SCRIPT_METHOD(SetFocus);
   UT_DECLARE_SCRIPT_METHOD(SetGroupImportant);
   UT_DECLARE_SCRIPT_METHOD(SetGroupUnimportant);
   UT_DECLARE_SCRIPT_METHOD(GetFocus);
   UT_DECLARE_SCRIPT_METHOD(GetGroupImportance);
   UT_DECLARE_SCRIPT_METHOD(FocusedGroups);
   UT_DECLARE_SCRIPT_METHOD(UnfocusedGroups);
   UT_DECLARE_SCRIPT_METHOD(SortedGroups);
   UT_DECLARE_SCRIPT_METHOD(CalculatePerceivedItemCount);
   UT_DECLARE_SCRIPT_METHOD(PerceivedItemCount);
   UT_DECLARE_SCRIPT_METHOD(PerceivedThreatItemLimit);

   // Cognitive Loading
   UT_DECLARE_SCRIPT_METHOD(AddExtraCognitiveLoading);

   // Weapons Methods
   UT_DECLARE_SCRIPT_METHOD(GetSelectedWeapon);
   UT_DECLARE_SCRIPT_METHOD(SetSelectedWeapon);
   UT_DECLARE_SCRIPT_METHOD(SetMasterArm);
   UT_DECLARE_SCRIPT_METHOD(GetMasterArm);
   UT_DECLARE_SCRIPT_METHOD(WeaponBayDoorsAreOpen);
   UT_DECLARE_SCRIPT_METHOD(OpenWeaponBayDoors);
   UT_DECLARE_SCRIPT_METHOD(CloseWeaponBayDoors);
   UT_DECLARE_SCRIPT_METHOD(SupportingWeapon);
   UT_DECLARE_SCRIPT_METHOD(ShootCueActive);
   UT_DECLARE_SCRIPT_METHOD(ShootCueBlinking);

   // Warnings/Cautions
   UT_DECLARE_SCRIPT_METHOD(MasterWarningActive);
   UT_DECLARE_SCRIPT_METHOD(MasterCautionActive);
   UT_DECLARE_SCRIPT_METHOD(JammingDetected);

   // Expendable Countermeasures Methods
   UT_DECLARE_SCRIPT_METHOD(QuantityOfChaff);
   UT_DECLARE_SCRIPT_METHOD(QuantityOfFlares);
   UT_DECLARE_SCRIPT_METHOD(QuantityOfDecoys);
   UT_DECLARE_SCRIPT_METHOD(DispenseChaff);
   UT_DECLARE_SCRIPT_METHOD(DispenseFlare);
   UT_DECLARE_SCRIPT_METHOD(DispenseDecoy);

   // Emissions Methods
   UT_DECLARE_SCRIPT_METHOD(RadarEmitting);
   UT_DECLARE_SCRIPT_METHOD(JammerEmitting);
   UT_DECLARE_SCRIPT_METHOD(OtherSystemsEmitting);

   // Fuel Methods
   UT_DECLARE_SCRIPT_METHOD(FuelStateBingoReached);
   UT_DECLARE_SCRIPT_METHOD(FuelStateJokerReached);
   UT_DECLARE_SCRIPT_METHOD(FuelStateNormalized);
   UT_DECLARE_SCRIPT_METHOD(TimeToJoker);
   UT_DECLARE_SCRIPT_METHOD(TimeToBingo);
   UT_DECLARE_SCRIPT_METHOD(TimeToEmpty);

   // Detection Methods
   UT_DECLARE_SCRIPT_METHOD(ExpectedDetectionRangeAgainstTarget);
   UT_DECLARE_SCRIPT_METHOD(ExpectedDetectionRangeByTarget);

   // WEZ Methods
   UT_DECLARE_SCRIPT_METHOD(ExpectedWezRangeAgainstTarget);
   UT_DECLARE_SCRIPT_METHOD(ExpectedWezRangeByTarget);

   // Track Processor Methods
   UT_DECLARE_SCRIPT_METHOD(ESM_TrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(MWS_TrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(RadarTrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(IRST_TrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(DAS_TrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(FLIR_TrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(EyeTrackProcessor);
   UT_DECLARE_SCRIPT_METHOD(PerceptionMasterTrackProcessor);

   // Geometry Methods
   UT_DECLARE_SCRIPT_METHOD(AngleOffTarget2D);
   UT_DECLARE_SCRIPT_METHOD(AngleOffTarget2DSelf);
   UT_DECLARE_SCRIPT_METHOD(AngleOffTarget3D);
   UT_DECLARE_SCRIPT_METHOD(AngleOffTarget3DSelf);
   UT_DECLARE_SCRIPT_METHOD(AspectAngleForTarget2D);
   UT_DECLARE_SCRIPT_METHOD(AspectAngleForTarget2DSelf);
   UT_DECLARE_SCRIPT_METHOD(AspectAngleForTarget3D);
   UT_DECLARE_SCRIPT_METHOD(AspectAngleForTarget3DSelf);

   // Methods to Get/Set/Reset Update Intervals
   UT_DECLARE_SCRIPT_METHOD(GetUpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(SetUpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(ResetUpdateInterval);
   UT_DECLARE_SCRIPT_METHOD(GetAssetDataPurgeLifetime);
   UT_DECLARE_SCRIPT_METHOD(SetAssetDataPurgeLifetime);
   UT_DECLARE_SCRIPT_METHOD(ResetAssetDataPurgeLifetime);
   UT_DECLARE_SCRIPT_METHOD(GetStartupInterval);
   UT_DECLARE_SCRIPT_METHOD(GetVisualPerceptionDelay);
   UT_DECLARE_SCRIPT_METHOD(SetVisualPerceptionDelay);
   UT_DECLARE_SCRIPT_METHOD(GetDisplayPerceptionDelay);
   UT_DECLARE_SCRIPT_METHOD(SetDisplayPerceptionDelay);

   // TODO : Add accessors for threat/asset update interval & max loading

   /** Helper method to map a string parameter to corresponding WsfSA_Processor::UpdateType value
    *  @param aUpdateTypeStr  string parameter (same as corresponding scenario command for update intervals)
    *  @param aUpdateType     on success, populated with one of WsfSA_Processor::UpdateType
    */
   static bool GetUpdateTypeEnum(const std::string& aUpdateTypeStr, WsfSA_Processor::UpdateType& aUpdateType);

   /** Helper method to map a string parameter to corresponding grouped WsfSA_Processor::UpdateType values
    *  @param aUpdateGroupStr string parameter that determines which group is returned
    *  @param aGroupUpdateTypesVec vector that contains the strings for the grouped update types
    */
   static bool GetGroupedTypes(const std::string&                        aUpdateGroupStr,
                               std::vector<WsfSA_Processor::UpdateType>& aGroupUpdateTypesVec);
};

#endif
