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

#include "WsfSA_Processor.hpp"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <math.h>

#include "UtAtmosphere.hpp"
#include "UtDCM.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtRelativeGeometry.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptEnvironment.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfAirCombatEventPipe.hpp"
#include "WsfAirCombatObserver.hpp"
#include "WsfAirMover.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfBrawlerFuel.hpp"
#include "WsfBrawlerMover.hpp"
#include "WsfExplicitWeapon.hpp"
#include "WsfFuel.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfIntercept.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMover.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfPlatform.hpp"
#include "WsfRadarSensor.hpp"
#include "WsfSA_Assess.hpp"
#include "WsfSA_EntityMessage.hpp"
#include "WsfSA_EntityPerception.hpp"
#include "WsfSA_Group.hpp"
#include "WsfSA_Perceive.hpp"
#include "WsfSA_Predict.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackListFwd.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackProcessor.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageHandler.hpp"
#include "script/WsfScriptStateMachine.hpp"

WsfSA_Processor::WsfSA_Processor(const WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfSA_Processor", "PROCESSOR")
   , mPlatform(nullptr)
   , mMoverPresent(false)
   , mP6DofMover(nullptr)
   , mSixDOF_Mover(nullptr)
   , mBrawlerMover(nullptr)
   , mBrawlerFuel(nullptr)
   , mAirMover(nullptr)
   , mAirMoverFuel(nullptr)
   , mMasterTrackProcessor(nullptr)
{
   mAssessPtr.reset(new WsfSA_Assess(this, &mData));
   mPerceivePtr.reset(new WsfSA_Perceive(this, &mData));
   mPredictPtr.reset(new WsfSA_Predict(this, &mData));

   // Initialize update event management data
   UpdateMgmtRecord updateRec;
   updateRec.activeInterval  = 0;
   updateRec.startupInterval = 0;
   updateRec.lastUpdateTime  = 0;
   updateRec.nextUpdateTime  = 0;
   updateRec.manualModeLock  = false;
   for (size_t i = 0; i < cUPDATE_TYPE_COUNT; ++i)
   {
      mUpdateMgmtData.push_back(updateRec);
   }
}

WsfSA_Processor::~WsfSA_Processor()
{
   // Clear raw pointers from mData
   std::map<size_t, WsfSA_EntityPerception*>::iterator it;
   for (it = mData.mReceivedAssetStatus.begin(); it != mData.mReceivedAssetStatus.end(); ++it)
   {
      delete it->second;
   }
   mData.mReceivedAssetStatus.clear();

   std::vector<WsfSA_EntityPerception*>::iterator pt;
   for (pt = mData.mPerceivedAssets.begin(); pt != mData.mPerceivedAssets.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedAssets.clear();

   for (pt = mData.mPerceivedAircraftAssets.begin(); pt != mData.mPerceivedAircraftAssets.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedAircraftAssets.clear();

   for (pt = mData.mPerceivedMissileAssets.begin(); pt != mData.mPerceivedMissileAssets.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedMissileAssets.clear();

   for (pt = mData.mPerceivedBogies.begin(); pt != mData.mPerceivedBogies.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedBogies.clear();

   for (pt = mData.mPerceivedAircraftBogies.begin(); pt != mData.mPerceivedAircraftBogies.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedAircraftBogies.clear();

   for (pt = mData.mPerceivedMissileBogies.begin(); pt != mData.mPerceivedMissileBogies.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedMissileBogies.clear();

   for (pt = mData.mPerceivedBandits.begin(); pt != mData.mPerceivedBandits.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedBandits.clear();

   for (pt = mData.mPerceivedAircraftBandits.begin(); pt != mData.mPerceivedAircraftBandits.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedAircraftBandits.clear();

   for (pt = mData.mPerceivedMissileBandits.begin(); pt != mData.mPerceivedMissileBandits.end(); ++pt)
   {
      delete (*pt);
   }
   mData.mPerceivedMissileBandits.clear();

   for (auto& group : mData.mCurrentlyPerceivedGroups)
   {
      WsfSA_Group* saGroup = group;
      delete saGroup;
   }
   mData.mCurrentlyPerceivedGroups.clear();
}

WsfSA_Processor::WsfSA_Processor(const WsfSA_Processor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mCallbacks(aSrc.mCallbacks)
   , mPlatform(aSrc.mPlatform)
   , mMoverPresent(aSrc.mMoverPresent)
   , mP6DofMover(aSrc.mP6DofMover)
   , mSixDOF_Mover(aSrc.mSixDOF_Mover)
   , mBrawlerMover(aSrc.mBrawlerMover)
   , mBrawlerFuel(aSrc.mBrawlerFuel)
   , mAirMover(aSrc.mAirMover)
   , mAirMoverFuel(aSrc.mAirMoverFuel)
   , mMasterTrackProcessor(aSrc.mMasterTrackProcessor)
   , mTrackManagerDataList(aSrc.mTrackManagerDataList)
   , mFilterAssetsFromTracks(aSrc.mFilterAssetsFromTracks)
   , mWeaponsDataSummary(aSrc.mWeaponsDataSummary)
   , mInitialWeaponsQuantity(aSrc.mInitialWeaponsQuantity)
   , mPerceivePtr(new WsfSA_Perceive(*aSrc.mPerceivePtr))
   , mAssessPtr(new WsfSA_Assess(*aSrc.mAssessPtr))
   , mPredictPtr(new WsfSA_Predict(*aSrc.mPredictPtr))
   , mEnemySidesList(aSrc.mEnemySidesList)
   , mFriendlySidesList(aSrc.mFriendlySidesList)
   , mNeutralSidesList(aSrc.mNeutralSidesList)
   , mEnemyTypesList(aSrc.mEnemyTypesList)
   , mFriendlyTypesList(aSrc.mFriendlyTypesList)
   , mNeutralTypesList(aSrc.mNeutralTypesList)
   , mMissileTypesList(aSrc.mMissileTypesList)
   , mUseIFF(aSrc.mUseIFF)
   , mUseSimpleIdByType(aSrc.mUseSimpleIdByType)
   , mMaxRangeForPerceivedAssets(aSrc.mMaxRangeForPerceivedAssets)
   , mMaxRangeForPerceivedBogiesAndBandits(aSrc.mMaxRangeForPerceivedBogiesAndBandits)
   , mUseMaxRangeFilterForPerceivedAssets(aSrc.mUseMaxRangeFilterForPerceivedAssets)
   , mUseMaxRangeFilterForPerceivedBogiesAndBandits(aSrc.mUseMaxRangeFilterForPerceivedBogiesAndBandits)
   , mMaxRangeForEngagementData(aSrc.mMaxRangeForEngagementData)
   , mUseMaxRangeForEngagementData(aSrc.mUseMaxRangeForEngagementData)
   , mAssumedRangeForAngleOnlyTargets(aSrc.mAssumedRangeForAngleOnlyTargets)
   , mFilteredPlatformIdList(aSrc.mFilteredPlatformIdList)
   , mFilterRequiresSameSide(aSrc.mFilterRequiresSameSide)
   , mFilterRequiresNotSameSide(aSrc.mFilterRequiresNotSameSide)
   , mFilterRequiresAirDomain(aSrc.mFilterRequiresAirDomain)
   , mFilterRequiresNotAirDomain(aSrc.mFilterRequiresNotAirDomain)
   , mFilterRequiresLandOrSurfaceDomain(aSrc.mFilterRequiresLandOrSurfaceDomain)
   , mFilterRequiresNotSubsurfaceDomain(aSrc.mFilterRequiresNotSubsurfaceDomain)
   , mFilterRequiresNotSpaceDomain(aSrc.mFilterRequiresNotSpaceDomain)
   , mFilterRequiresSA_Processor(aSrc.mFilterRequiresSA_Processor)
   , mMissileSpeedAnyAlt(aSrc.mMissileSpeedAnyAlt)
   , mMissileAltAnySpeed(aSrc.mMissileAltAnySpeed)
   , mMissileSpeedWithAlt(aSrc.mMissileSpeedWithAlt)
   , mMissileAltWithSpeed(aSrc.mMissileAltWithSpeed)
   , mMissileNoseAngle(aSrc.mMissileNoseAngle)
   , mMissileTimeToIntercept(aSrc.mMissileTimeToIntercept)
   , mMissileDistance(aSrc.mMissileDistance)
   , mMissileSpeedAnyAltValue(aSrc.mMissileSpeedAnyAltValue)
   , mMissileAltAnySpeedValue(aSrc.mMissileAltAnySpeedValue)
   , mMissileSpeedWithAltValue(aSrc.mMissileSpeedWithAltValue)
   , mMissileAltWithSpeedValue(aSrc.mMissileAltWithSpeedValue)
   , mMissileNoseAngleValue(aSrc.mMissileNoseAngleValue)
   , mMissileTimeToInterceptValue(aSrc.mMissileTimeToInterceptValue)
   , mMissileDistanceValue(aSrc.mMissileDistanceValue)
   , mFlightId(aSrc.mFlightId)
   , mIdFlag(aSrc.mIdFlag)
   , mBingoFuel_lbs(aSrc.mBingoFuel_lbs)
   , mJokerFuel_lbs(aSrc.mJokerFuel_lbs)
   , mAircraftKinematicsData(aSrc.mAircraftKinematicsData)
   , mFuelSystemData(aSrc.mFuelSystemData)
   , mNavigationDataSummary(aSrc.mNavigationDataSummary)
   , mFlightControlsDataSummary(aSrc.mFlightControlsDataSummary)
   , mESM_TrackProcessor(aSrc.mESM_TrackProcessor)
   , mMWS_TrackProcessor(aSrc.mMWS_TrackProcessor)
   , mRadarTrackProcessor(aSrc.mRadarTrackProcessor)
   , mIRST_TrackProcessor(aSrc.mIRST_TrackProcessor)
   , mDAS_TrackProcessor(aSrc.mDAS_TrackProcessor)
   , mFLIR_TrackProcessor(aSrc.mFLIR_TrackProcessor)
   , mEyesTrackProcessor(aSrc.mEyesTrackProcessor)
   , mPerceptionMasterTrackProcessorStr(aSrc.mPerceptionMasterTrackProcessorStr)
   , mPerceptionMasterTrackProcessorPtr(aSrc.mPerceptionMasterTrackProcessorPtr)
   , mUsingPerceptionMasterTrackProcessor(aSrc.mUsingPerceptionMasterTrackProcessor)
   , mRadarEmitting(aSrc.mRadarEmitting)
   , mJammerEmitting(aSrc.mJammerEmitting)
   , mOtherSystemsEmitting(aSrc.mOtherSystemsEmitting)
   , mAssetDataPurgeLifetime(aSrc.mAssetDataPurgeLifetime)
   , mStartupAssetDataPurgeLifetime(aSrc.mStartupAssetDataPurgeLifetime)
   , mDisplayPerceptionDelay(aSrc.mDisplayPerceptionDelay)
   , mVisualPerceptionDelay(aSrc.mVisualPerceptionDelay)
   , mUpdateMgmtData(aSrc.mUpdateMgmtData)
   , mData(aSrc.mData)
   , mUpdatedScriptContext(aSrc.mUpdatedScriptContext)
   , mUseDebugOutput(aSrc.mUseDebugOutput)
   , mDebugPlatformName(aSrc.mDebugPlatformName)
{
   // Be sure to set *this* Sa Processor
   mAssessPtr->SetSAP(this, &this->mData);
   mPerceivePtr->SetSAP(this, &this->mData);
   mPredictPtr->SetSAP(this, &this->mData);
}

void WsfSA_Processor::CallScript(double aSimTime, const std::string& aScriptName)
{
   if (mPlatform != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList dataList;
      mPlatform->ExecuteScript(aSimTime, aScriptName, scriptRetVal, dataList);
   }
}

double WsfSA_Processor::CallScriptReturnDouble(double aSimTime, const std::string& aScriptName)
{
   if (mPlatform != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList dataList;
      mPlatform->ExecuteScript(aSimTime, aScriptName, scriptRetVal, dataList);

      return scriptRetVal.GetDouble();
   }

   return 0.0;
}

void WsfSA_Processor::CheckForFrequencyUnits(double& aValue, UtInput& aInput)
{
   std::string units;
   aInput.ReadValue(aValue);
   aInput.ReadValue(units);
   UtStringUtil::ToLower(units);
   UtInput::ValueType valueType = UtInput::GetValueTypeOf(units);
   // If the user input a frequency instead of seconds, convert it for them
   if (valueType == UtInput::ValueType::cFREQUENCY)
   {
      // If our value us not 0, convert it to seconds.
      if (aValue != 0.0)
      {
         aValue = (1.0 / aValue);
      }
   }
}

void WsfSA_Processor::PlatformAdded(double aSimTime, WsfPlatform* aPlatform)
{
   if (aPlatform != nullptr)
   {
      if (mFilterRequiresSameSide)
      {
         if (aPlatform->GetSide() != mPlatform->GetSide())
         {
            return;
         }
      }

      if (mFilterRequiresNotSameSide)
      {
         if (aPlatform->GetSide() == mPlatform->GetSide())
         {
            return;
         }
      }

      if (mFilterRequiresAirDomain)
      {
         if (aPlatform->GetSpatialDomain() != WSF_SPATIAL_DOMAIN_AIR)
         {
            return;
         }
      }

      if (mFilterRequiresNotAirDomain)
      {
         if (aPlatform->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_AIR)
         {
            return;
         }
      }

      if (mFilterRequiresLandOrSurfaceDomain)
      {
         if ((aPlatform->GetSpatialDomain() != WSF_SPATIAL_DOMAIN_LAND) ||
             (aPlatform->GetSpatialDomain() != WSF_SPATIAL_DOMAIN_SURFACE))
         {
            return;
         }
      }

      if (mFilterRequiresNotSubsurfaceDomain)
      {
         if (aPlatform->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SUBSURFACE)
         {
            return;
         }
      }

      if (mFilterRequiresNotSpaceDomain)
      {
         if (aPlatform->GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE)
         {
            return;
         }
      }

      if (mFilterRequiresSA_Processor)
      {
         if (!WsfSA_Processor::PlatformContainsSA_Processor(aPlatform))
         {
            return;
         }
      }
      // Add to the platform list (this will check for duplicates)
      mFilteredPlatformIdList.emplace_back(aPlatform->GetIndex());
   }
}

void WsfSA_Processor::PlatformDeleted(WsfPlatform* aPlatform)
{
   size_t count = 0;
   for (auto item : mFilteredPlatformIdList)
   {
      if (item == aPlatform->GetIndex())
      {
         mFilteredPlatformIdList.erase(mFilteredPlatformIdList.begin() + count);
         return; // Since there will be only one platform with that id, we're done
      }
      ++count;
   }
}

std::string WsfSA_Processor::GetSelectedWeapon() const
{
   return mPerceivePtr->SelectedWeaponName();
}

const WsfExplicitWeapon* WsfSA_Processor::SelectedWeapon() const
{
   return mPerceivePtr->SelectedWeapon();
}

int WsfSA_Processor::QuantityOfSelectedWeapon() const
{
   return mPerceivePtr->QuantityOfSelectedWeapon();
}

bool WsfSA_Processor::SetSelectedWeapon(std::string aWeaponName)
{
   return mPerceivePtr->SetSelectedWeapon(aWeaponName);
}

const std::vector<WsfExplicitWeapon*> WsfSA_Processor::GetWeaponList() const
{
   return mPerceivePtr->GetWeaponList();
}

void WsfSA_Processor::SetMasterArm(bool aArmed)
{
   mPerceivePtr->SetMasterArm(aArmed);
}

bool WsfSA_Processor::GetMasterArm() const
{
   return mPerceivePtr->GetMasterArm();
}

bool WsfSA_Processor::MasterWarningActive() const
{
   return mPerceivePtr->MasterWarningActive();
}

bool WsfSA_Processor::MasterCautionActive() const
{
   return mPerceivePtr->MasterCautionActive();
}

bool WsfSA_Processor::WeaponBayDoorsAreOpen() const
{
   return mPerceivePtr->WeaponBayDoorsAreOpen();
}

bool WsfSA_Processor::SupportingWeapon() const
{
   return mAssessPtr->SupportingWeapon();
}

bool WsfSA_Processor::ShootCueActive() const
{
   return mPerceivePtr->ShootCueActive();
}

bool WsfSA_Processor::ShootCueBlinking() const
{
   return mPerceivePtr->ShootCueBlinking();
}

bool WsfSA_Processor::JammingDetected() const
{
   return mPerceivePtr->JammingDetected();
}

bool WsfSA_Processor::EngagedTargetValid() const
{
   return mPerceivePtr->EngagedTargetValid();
}

WsfSA_Processor::SA_EngagedTarget WsfSA_Processor::GetEngagedTarget() const
{
   return mPerceivePtr->GetEngagedTarget();
}

void WsfSA_Processor::SetEngagedTarget(SA_EngagedTarget aTarget)
{
   mPerceivePtr->SetEngagedTarget(aTarget);
}

int WsfSA_Processor::QuantityOfChaff() const
{
   return mPerceivePtr->QuantityOfChaff();
}

int WsfSA_Processor::QuantityOfFlares() const
{
   return mPerceivePtr->QuantityOfFlares();
}

int WsfSA_Processor::QuantityOfDecoys() const
{
   return mPerceivePtr->QuantityOfDecoys();
}

bool WsfSA_Processor::DispenseChaff(double aSimTime)
{
   return mPerceivePtr->DispenseChaff(aSimTime);
}
bool WsfSA_Processor::DispenseFlare(double aSimTime)
{
   return mPerceivePtr->DispenseFlare(aSimTime);
}
bool WsfSA_Processor::DispenseDecoy(double aSimTime)
{
   return mPerceivePtr->DispenseDecoy(aSimTime);
}

bool WsfSA_Processor::PlatformContainsSA_Processor(WsfPlatform* aPlatform)
{
   if (aPlatform == nullptr)
   {
      return false;
   }

   // Loop through components that are sensors
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aPlatform); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_SA_PROCESSOR"))
      {
         return true;
      }
   }

   return false;
}

const WsfSA_Processor* WsfSA_Processor::GetPlatformSA_Processor(const WsfPlatform* aPlatform)
{
   if (aPlatform == nullptr)
   {
      return nullptr;
   }

   // Loop through components that are sensors
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*aPlatform); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_SA_PROCESSOR"))
      {
         WsfSA_Processor* saProcessor = static_cast<WsfSA_Processor*>(*iter);

         return saProcessor;
      }
   }

   return nullptr;
}

bool WsfSA_Processor::IsEnemySide(const std::string& aSide) const
{
   // Check if aType is present in list
   if (std::find(mEnemySidesList.begin(), mEnemySidesList.end(), aSide) != mEnemySidesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

bool WsfSA_Processor::IsFriendlySide(const std::string& aSide) const
{
   // First check same side
   if (mPlatform->GetSide() == aSide)
   {
      return true;
   }

   // Check if aType is present in list
   if (std::find(mFriendlySidesList.begin(), mFriendlySidesList.end(), aSide) != mFriendlySidesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

bool WsfSA_Processor::IsNeutralSide(const std::string& aSide) const
{
   // Check if aType is present in list
   if (std::find(mNeutralSidesList.begin(), mNeutralSidesList.end(), aSide) != mNeutralSidesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

bool WsfSA_Processor::IsEnemyType(const std::string& aType) const
{
   // Check if aType is present in list
   if (std::find(mEnemyTypesList.begin(), mEnemyTypesList.end(), aType) != mEnemyTypesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

bool WsfSA_Processor::IsFriendlyType(const std::string& aType) const
{
   // Check if aType is present in list
   if (std::find(mFriendlyTypesList.begin(), mFriendlyTypesList.end(), aType) != mFriendlyTypesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

bool WsfSA_Processor::IsNeutralType(const std::string& aType) const
{
   // Check if aType is present in list
   if (std::find(mNeutralTypesList.begin(), mNeutralTypesList.end(), aType) != mNeutralTypesList.end())
   {
      // Found it
      return true;
   }

   return false;
}
bool WsfSA_Processor::IsMissileType(const std::string& aType) const
{
   // Check if aType is present in list
   if (std::find(mMissileTypesList.begin(), mMissileTypesList.end(), aType) != mMissileTypesList.end())
   {
      // Found it
      return true;
   }

   return false;
}

const WsfTrackProcessor* WsfSA_Processor::GetMWS_Processor() const
{
   // Loop through all of the track processors
   for (auto& trackProcessor : mTrackProcessorList)
   {
      if (mMWS_TrackProcessor == trackProcessor->GetName())
      {
         return trackProcessor;
      }
   }

   return nullptr;
}

bool WsfSA_Processor::PlatformInTrackListMWS(const WsfPlatform* aPlatform) const
{
   // If there is no MWS track processor, return false
   if (mMWS_TrackProcessor.empty())
   {
      return false;
   }

   // Get the MWS track processor
   const WsfTrackProcessor* trkProcessor = GetMWS_Processor();

   if (trkProcessor == nullptr)
   {
      return false;
   }

   // This function will return true if the specified (truth) platform is in the track list
   // of the Missile Warning System (MWS) track processor. Otherwise, it will return false.

   // Loop through the tracks to find the specified platform
   WsfTrackManager&   trackMgr  = trkProcessor->GetTrackManager();
   WsfLocalTrackList& trkList   = trackMgr.GetLocalTrackList();
   unsigned int       numTracks = trkList.GetTrackCount();
   for (unsigned int i = 0; i < numTracks; ++i)
   {
      // Track must be valid (non-null track pointer)
      const WsfLocalTrack* trk = trkList.GetTrackEntry(i);
      if (trk != nullptr)
      {
         if (trk->GetTargetIndex() == aPlatform->GetIndex())
         {
            return true; // Platform is a match, return true
         }
      }
   }

   return false; // No platform found, return false
}

WsfLocalTrackList* WsfSA_Processor::GetPerceptionMasterTrackProcessorTrackList() const
{
   // If there is no PMTP, return null
   if (!mUsingPerceptionMasterTrackProcessor)
   {
      return nullptr;
   }

   // Get the PMTP
   WsfTrackProcessor* PerceptionMasterTrackProcessor = GetPerceptionMasterTrackProcessor();
   if (PerceptionMasterTrackProcessor == nullptr)
   {
      return nullptr;
   }

   return &(PerceptionMasterTrackProcessor->GetTrackManager().GetTrackList());
}

UtScriptClass* WsfSA_Processor::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return new WsfScriptSA_ProcessorClass(aClassName, aScriptTypesPtr);
}

WsfSA_Processor* WsfSA_Processor::CreateSA_Processor(double aSimTime, WsfPlatform& aPlatform)
{
   WsfSA_Processor* processorPtr = new WsfSA_Processor(aPlatform.GetScenario());
   processorPtr->SetName(aPlatform.GetName() + "_default_perception_processor");

   aPlatform.AddComponent(processorPtr);

   // Must initialize since they may be created post simulation initialization
   processorPtr->PreInitialize(aPlatform.GetSimTime());
   processorPtr->Initialize(aPlatform.GetSimTime());

   return processorPtr;
}

bool WsfSA_Processor::EntityIsInAssetGroup(const WsfSA_EntityPerception* aEntity) const
{
   return mPerceivePtr->EntityIsInAssetGroup(aEntity);
}

bool WsfSA_Processor::EntityIsInAssetGroup(size_t aEntityIndex) const
{
   return mPerceivePtr->EntityIsInAssetGroup(aEntityIndex);
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Processor::GetPlatformPerceivedAssets(const WsfPlatform& aPlatform,
                                                                                        double             aSimTime)
{
   return GetSA_Processor(aPlatform)->mPerceivePtr->PerceivedAssets(aSimTime);
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Processor::GetPlatformPerceivedBogies(const WsfPlatform& aPlatform,
                                                                                        double             aSimTime)
{
   return GetSA_Processor(aPlatform)->mPerceivePtr->PerceivedBogies(aSimTime);
}

const std::vector<WsfSA_EntityPerception*>& WsfSA_Processor::GetPlatformPerceivedBandits(const WsfPlatform& aPlatform,
                                                                                         double             aSimTime)
{
   return GetSA_Processor(aPlatform)->mPerceivePtr->PerceivedBandits(aSimTime);
}

WsfSA_Processor* WsfSA_Processor::GetSA_Processor(const WsfPlatform& aPlatform)
{
   // Attempt to find the first instance of a WsfSA_Processor.
   WsfSA_Processor* processorPtr(nullptr);
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(aPlatform); !iter.AtEnd(); ++iter)
   {
      processorPtr = dynamic_cast<WsfSA_Processor*>(*iter);
      if (processorPtr != nullptr)
      {
         break;
      }
   }

   return processorPtr;
}

bool WsfSA_Processor::Initialize(double aSimTime)
{
   // Set the platform for this processor
   mPlatform = GetPlatform();

   WsfMover* mover = mPlatform->GetMover();
   if (mover != nullptr)
   {
      if (mover->GetType() == "WSF_P6DOF_MOVER")
      {
         mP6DofMover   = static_cast<WsfP6DOF_Mover*>(mover);
         mMoverPresent = true;
      }
      else if (mover->IsA_TypeOf("WSF_SIX_DOF_MOVER"))
      {
         mSixDOF_Mover = static_cast<wsf::six_dof::Mover*>(mover);
         mMoverPresent = true;
      }
      else if (mover->GetType() == "WSF_BRAWLER_MOVER")
      {
         mBrawlerMover = static_cast<WsfBrawlerMover*>(mover);
         mMoverPresent = true;

         WsfFuel* fuel = mPlatform->GetFuel();
         if (fuel->GetType() == "WSF_BRAWLER_FUEL")
         {
            mBrawlerFuel = static_cast<WsfBrawlerFuel*>(fuel);
         }
      }
      else if (mover->GetType() == "WSF_AIR_MOVER")
      {
         mAirMover     = static_cast<WsfAirMover*>(mover);
         mMoverPresent = true;

         WsfFuel* fuel = mPlatform->GetFuel();
         mAirMoverFuel = static_cast<WsfBrawlerFuel*>(fuel);
      }
      else
      {
         mMoverPresent = false; // No valid mover is present
      }
   }
   else
   {
      mMoverPresent = false; // No valid mover is present
   }

   // Check for an uninitialized asset purge lifetime
   if (mAssetDataPurgeLifetime < std::numeric_limits<double>::epsilon())
   {
      if (mUpdateMgmtData[cASSETS_DATA].startupInterval < std::numeric_limits<double>::epsilon())
      {
         // Use default of 5 seconds
         mAssetDataPurgeLifetime = 5.0;
      }
      else
      {
         // Use a default of 2.5x the update interval
         mAssetDataPurgeLifetime = 2.5 * mUpdateMgmtData[cASSETS_DATA].startupInterval;
      }
   }
   mStartupAssetDataPurgeLifetime = mAssetDataPurgeLifetime; // remember startup value

   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   for (size_t i = 0; i < mUpdateMgmtData.size(); i++)
   {
      if (mUpdateMgmtData[i].startupInterval > 0.0)
      {
         double updateInterval      = WsfProcessor::GetUpdateInterval();
         double roundedStartupValue = RoundToBaseRateMultiple(mUpdateMgmtData[i].startupInterval, updateInterval);
         roundedStartupValue        = std::max(roundedStartupValue, updateInterval); // don't allow rounding to zero

         if (abs(roundedStartupValue - mUpdateMgmtData[i].startupInterval) > (updateInterval / 100.0))
         {
            std::string typeString = GetUpdateTypeString(static_cast<UpdateType>(i));
            ut::log::warning() << "WSF_SA_PROCESSOR update interval (" << typeString << ") ";
            ut::log::warning() << mUpdateMgmtData[i].startupInterval << " was rounded to " << roundedStartupValue;
         }
         mUpdateMgmtData[i].startupInterval = roundedStartupValue;
         mUpdateMgmtData[i].activeInterval  = mUpdateMgmtData[i].startupInterval;
         mUpdateMgmtData[i].nextUpdateTime  = aSimTime + mUpdateMgmtData[i].activeInterval;
      }
   }

   // Get the track manager
   mMasterTrackProcessor = &(mPlatform->GetTrackManager());

   if (mMasterTrackProcessor == nullptr)
   {
      std::string errorMsg =
         mPlatform->GetName() + " has a WsfAirCombatAssessementProcessor, but lacks a track manager!";
      throw UtException(errorMsg);
   }

   // Get the list of track processors
   for (WsfComponentList::RoleIterator<WsfProcessor> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_TRACK_PROCESSOR"))
      {
         WsfTrackProcessor* processor = static_cast<WsfTrackProcessor*>(*iter);

         mTrackProcessorList.push_back(processor);
      }
   }

   // Get the Master Perception Track Processor, if specified
   if (mUsingPerceptionMasterTrackProcessor)
   {
      for (auto& trackProcessor : mTrackProcessorList)
      {
         if (mPerceptionMasterTrackProcessorStr == trackProcessor->GetName())
         {
            mPerceptionMasterTrackProcessorPtr = trackProcessor;
            break;
         }
      }
   }

   if (mUsingPerceptionMasterTrackProcessor && mPerceptionMasterTrackProcessorPtr != nullptr)
   {
      mCallbacks.Add(
         mPerceptionMasterTrackProcessorPtr->GetTrackManager().LocalTrackDropped.Connect(&WsfSA_Processor::LocalTrackDropped,
                                                                                         this));
   }
   else
   {
      mCallbacks.Add(mMasterTrackProcessor->LocalTrackDropped.Connect(&WsfSA_Processor::LocalTrackDropped, this));
   }

   mCallbacks.Add(mMasterTrackProcessor->TrackManagerChanged.Connect(&WsfSA_Processor::TrackManagerChanged, this));

   // Get the initial number of explicit weapons
   mInitialWeaponsQuantity = 0;
   for (WsfComponentList::RoleIterator<WsfWeapon> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      if ((*iter)->IsA_TypeOf("WSF_EXPLICIT_WEAPON"))
      {
         mInitialWeaponsQuantity +=
            static_cast<int>((*iter)->GetQuantityRemaining() + 0.01); // Add a bit to ensure proper rounding
      }
   }
   mWeaponsDataSummary.initialNumWeapons = static_cast<uint16_t>(mInitialWeaponsQuantity);

   // Initialize the perception module
   mPerceivePtr->Initialize(aSimTime);

   // Initialize the assess module
   mAssessPtr->Initialize(aSimTime);

   // Initialize the predict module
   mPredictPtr->Initialize(aSimTime);

   return ok;
}

bool WsfSA_Processor::Initialize2(double aSimTime)
{
   // Obtain initial platform-centric data to avoid triggering conditions
   // based on initial parameter values. E.g., triggering bingo/joker lights
   // because fuel data hasn't yet been updated from the default value of 0.

   // Push initial flight data
   if (mUpdateMgmtData[cFLIGHT_DATA].activeInterval > 0)
   {
      UpdateFlightKinematicsData(aSimTime);
   }

   // Push initial controls data
   if (mUpdateMgmtData[cCONTROLS_DATA].activeInterval > 0)
   {
      UpdateFlightControlsData(aSimTime);
   }

   // Push initial fuel data
   if (mUpdateMgmtData[cFUEL_DATA].activeInterval > 0)
   {
      UpdateFuelData(aSimTime);
   }

   // Push initial nav data
   if (mUpdateMgmtData[cNAV_DATA].activeInterval > 0)
   {
      UpdateNavData(aSimTime);
   }

   // Do not attempt to update pieces where a custom script might be invoked.
   // For platforms present at simulation start, these updates would execute
   // these scripts before any simulation frames have been carried out.

   return WsfScriptProcessor::Initialize2(aSimTime);
}

void WsfSA_Processor::LocalTrackDropped(double aSimTime, const WsfLocalTrack* aLocalTrackPtr)
{
   mPerceivePtr->LocalTrackDropped(aSimTime, aLocalTrackPtr);
}

bool WsfSA_Processor::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command = aInput.GetCommand();

   if (command == "report_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cSTATUS].startupInterval, UtInput::cTIME);
   }
   else if (command == "engagement_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cENGAGEMENT_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "flight_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cFLIGHT_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "fuel_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cFUEL_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "nav_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cNAV_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "flight_controls_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cCONTROLS_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "weapons_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cWEAPONS_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "track_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cTRACK_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "asset_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cASSETS_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "asset_purge_lifetime")
   {
      aInput.ReadValueOfType(mAssetDataPurgeLifetime, UtInput::cTIME);
   }
   else if (command == "perceived_item_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cPERCEIVED_ITEM_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "prioritized_item_data_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cPRIORITIZED_ITEM_DATA].startupInterval, UtInput::cTIME);
   }
   else if (command == "perceived_item_calculation_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cPERCEIVED_ITEM_CALCULATION].startupInterval, UtInput::cTIME);
   }
   else if (command == "prioritized_item_calculation_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cPRIORITIZED_ITEM_CALCULATION].startupInterval, UtInput::cTIME);
   }
   else if (command == "behavior_calculation_update_interval")
   {
      aInput.ReadValueOfType(mUpdateMgmtData[cBEHAVIOR_CALCULATION].startupInterval, UtInput::cTIME);
   }
   else if (command == "cognitive_update_interval")
   {
      double readInput;
      aInput.ReadValueOfType(readInput, UtInput::cTIME);
      std::vector<UpdateType> updateTypesVec = GetCognitiveUpdateGroup();

      for (auto updateType : updateTypesVec)
      {
         mUpdateMgmtData[updateType].startupInterval = readInput;
      }
   }
   else if (command == "platform_update_interval")
   {
      double readInput;
      aInput.ReadValueOfType(readInput, UtInput::cTIME);
      std::vector<UpdateType> updateTypesVec = GetPlatformUpdateGroup();

      for (auto updateType : updateTypesVec)
      {
         mUpdateMgmtData[updateType].startupInterval = readInput;
      }
   }
   else if (command == "universal_update_interval")
   {
      double readInput;
      aInput.ReadValueOfType(readInput, UtInput::cTIME);
      std::vector<UpdateType> updateTypesVec = GetUniversalUpdateGroup();

      for (auto updateType : updateTypesVec)
      {
         mUpdateMgmtData[updateType].startupInterval = readInput;
      }
   }
   else if (command == "display_perception_delay")
   {
      aInput.ReadValueOfType(mDisplayPerceptionDelay, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mDisplayPerceptionDelay, 0.0);
   }
   else if (command == "visual_perception_delay")
   {
      aInput.ReadValueOfType(mVisualPerceptionDelay, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mVisualPerceptionDelay, 0.0);
   }
   else if (command == "enemy_side")
   {
      std::string sideString;
      aInput.ReadValue(sideString);
      mEnemySidesList.emplace_back(sideString);
   }
   else if (command == "friendly_side")
   {
      std::string sideString;
      aInput.ReadValue(sideString);
      mFriendlySidesList.emplace_back(sideString);
   }
   else if (command == "neutral_side")
   {
      std::string sideString;
      aInput.ReadValue(sideString);
      mNeutralSidesList.emplace_back(sideString);
   }
   else if (command == "enemy_type")
   {
      std::string typeString;
      aInput.ReadValue(typeString);
      mEnemyTypesList.emplace_back(typeString);
   }
   else if (command == "friendly_type")
   {
      std::string typeString;
      aInput.ReadValue(typeString);
      mFriendlyTypesList.emplace_back(typeString);
   }
   else if (command == "neutral_type")
   {
      std::string sideString;
      aInput.ReadValue(sideString);
      mNeutralTypesList.emplace_back(sideString);
   }
   else if (command == "missile_type")
   {
      std::string typeString;
      aInput.ReadValue(typeString);
      mMissileTypesList.emplace_back(typeString);
   }
   else if (command == "use_iff_id")
   {
      aInput.ReadValue(mUseIFF);
   }
   else if (command == "use_simple_id_by_type")
   {
      aInput.ReadValue(mUseSimpleIdByType);
   }
   else if (command == "max_range_for_perceived_assets")
   {
      aInput.ReadValueOfType(mMaxRangeForPerceivedAssets, UtInput::ValueType::cLENGTH);
      mUseMaxRangeFilterForPerceivedAssets = true;
   }
   else if (command == "max_range_for_perceived_bogies_and_bandits")
   {
      aInput.ReadValueOfType(mMaxRangeForPerceivedBogiesAndBandits, UtInput::ValueType::cLENGTH);
      mUseMaxRangeFilterForPerceivedBogiesAndBandits = true;
   }
   else if (command == "max_range_for_engagement_data")
   {
      aInput.ReadValueOfType(mMaxRangeForEngagementData, UtInput::ValueType::cLENGTH);
      mUseMaxRangeForEngagementData = true;
   }
   else if (command == "assumed_range_for_angle_only_targets")
   {
      aInput.ReadValueOfType(mAssumedRangeForAngleOnlyTargets, UtInput::ValueType::cLENGTH);
   }
   else if (command == "filter_requires_same_side")
   {
      aInput.ReadValue(mFilterRequiresSameSide);
   }
   else if (command == "filter_requires_not_same_side")
   {
      aInput.ReadValue(mFilterRequiresNotSameSide);
   }
   else if (command == "filter_requires_air_domain")
   {
      aInput.ReadValue(mFilterRequiresAirDomain);
   }
   else if (command == "filter_requires_not_air_domain")
   {
      aInput.ReadValue(mFilterRequiresNotAirDomain);
   }
   else if (command == "filter_requires_land_or_surface_domain")
   {
      aInput.ReadValue(mFilterRequiresLandOrSurfaceDomain);
   }
   else if (command == "filter_requires_not_subsurface_domain")
   {
      aInput.ReadValue(mFilterRequiresNotSubsurfaceDomain);
   }
   else if (command == "filter_requires_not_space_domain")
   {
      aInput.ReadValue(mFilterRequiresNotSpaceDomain);
   }
   else if (command == "filter_requires_sa_processor")
   {
      aInput.ReadValue(mFilterRequiresSA_Processor);
   }
   else if (command == "missile_speed_any_alt")
   {
      double data_mps;
      aInput.ReadValueOfType(data_mps, UtInput::cSPEED);

      mMissileSpeedAnyAlt      = true;
      mMissileSpeedAnyAltValue = data_mps;
   }
   else if (command == "missile_alt_any_speed")
   {
      double data_m;
      aInput.ReadValueOfType(data_m, UtInput::cLENGTH);

      mMissileAltAnySpeed      = true;
      mMissileAltAnySpeedValue = data_m;
   }
   else if (command == "missile_speed_with_alt")
   {
      double data_mps;
      aInput.ReadValueOfType(data_mps, UtInput::cSPEED);

      mMissileSpeedWithAlt      = true;
      mMissileSpeedWithAltValue = data_mps;
   }
   else if (command == "missile_alt_with_speed")
   {
      double data_m;
      aInput.ReadValueOfType(data_m, UtInput::cLENGTH);

      mMissileAltWithSpeed      = true;
      mMissileAltWithSpeedValue = data_m;
   }
   else if (command == "missile_nose_angle")
   {
      double data_rad;
      aInput.ReadValueOfType(data_rad, UtInput::cANGLE);

      mMissileNoseAngle      = true;
      mMissileNoseAngleValue = data_rad;
   }
   else if (command == "missile_time_to_intercept")
   {
      double data_sec;
      aInput.ReadValueOfType(data_sec, UtInput::cTIME);

      mMissileTimeToIntercept      = true;
      mMissileTimeToInterceptValue = data_sec;
   }
   else if (command == "missile_distance")
   {
      double data_m;
      aInput.ReadValueOfType(data_m, UtInput::cLENGTH);

      mMissileDistance      = true;
      mMissileDistanceValue = data_m;
   }
   else if (command == "flight_id")
   {
      int idNumber = 0;
      aInput.ReadValue(idNumber);
      if (idNumber < 0)
      {
         idNumber = 0;
      }
      mFlightId = static_cast<std::uint16_t>(idNumber);
   }
   else if (command == "id_flag")
   {
      aInput.ReadValue(mIdFlag);
   }
   else if (command == "bingo_fuel")
   {
      double qty_kg = 0;
      aInput.ReadValueOfType(qty_kg, UtInput::ValueType::cMASS);
      mBingoFuel_lbs            = qty_kg * UtMath::cLB_PER_KG;
      mFuelSystemData.bingo_lbs = static_cast<float>(mBingoFuel_lbs);
   }
   else if (command == "joker_fuel")
   {
      double qty_kg = 0;
      aInput.ReadValueOfType(qty_kg, UtInput::ValueType::cMASS);
      mJokerFuel_lbs            = qty_kg * UtMath::cLB_PER_KG;
      mFuelSystemData.joker_lbs = static_cast<float>(mJokerFuel_lbs);
   }
   else if (command == "filter_assets_from_tracks")
   {
      aInput.ReadValue(mFilterAssetsFromTracks);
   }
   else if (command == "esm_track_processor")
   {
      aInput.ReadValue(mESM_TrackProcessor);
   }
   else if (command == "mws_track_processor")
   {
      aInput.ReadValue(mMWS_TrackProcessor);
   }
   else if (command == "radar_track_processor")
   {
      aInput.ReadValue(mRadarTrackProcessor);
   }
   else if (command == "irst_track_processor")
   {
      aInput.ReadValue(mIRST_TrackProcessor);
   }
   else if (command == "das_track_processor")
   {
      aInput.ReadValue(mDAS_TrackProcessor);
   }
   else if (command == "flir_track_processor")
   {
      aInput.ReadValue(mFLIR_TrackProcessor);
   }
   else if (command == "eyes_track_processor")
   {
      aInput.ReadValue(mEyesTrackProcessor);
   }
   else if (command == "perception_master_track_processor")
   {
      aInput.ReadValue(mPerceptionMasterTrackProcessorStr);

      if (!mPerceptionMasterTrackProcessorStr.empty())
      {
         mUsingPerceptionMasterTrackProcessor = true;
      }
   }
   else if (mPerceivePtr->ProcessInput(aInput)) // Perceive script commands
   {
      myCommand = true;
   }
   else if (mAssessPtr->ProcessInput(aInput)) // Assess script commands
   {
      myCommand = true;
   }
   else if (WsfScriptProcessor::ProcessInput(aInput)) // Handle external_link / reports_to
   {
      myCommand = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

bool WsfSA_Processor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   return mPerceivePtr->ProcessMessage(aSimTime, aMessage);
}

bool WsfSA_Processor::ScriptExists(const std::string& aScriptName)
{
   if (mPlatform != nullptr)
   {
      mPlatform->GetScriptContext();
      if (mPlatform->HasScript(aScriptName))
      {
         // Call the script
         return true;
      }
   }
   return false;
}

#undef SendMessage
void WsfSA_Processor::SendMessage(double aSimTime, const WsfSA_EntityMessage& aStatusMessage)
{
   mExternalLinks.SendMessage(aSimTime, aStatusMessage);
}

void WsfSA_Processor::TrackManagerChanged(double aSimTime, WsfTrackManager* aNewManagerPtr)
{
   if (aNewManagerPtr != nullptr)
   {
      mMasterTrackProcessor = aNewManagerPtr;
   }
}

void WsfSA_Processor::Update(double aSimTime)
{
   mUpdatedScriptContext = false; // Reset the flag, which may be set in UpdateBehaviorCalculation()

   // The various SA Processor sub-updates (update intervals) are processed on every base rate tick except for T=0
   if (aSimTime > 0.0)
   {
      HandleSA_ProcessorUpdates(aSimTime);
      ++mEventCount; // Count update events, but not the first one at T=0
   }

   // TODO -- Possibly remove this later, but it is helpful for debugging
   if (mUseDebugOutput)
   {
      DebugTestOutput(aSimTime);
   }

   // Update the script context if it was not already updated in UpdateBehaviorCalculation,
   // which may get called in HandleSA_ProcessorUpdates (above)
   if (!mUpdatedScriptContext)
   {
      mContextPtr->Update(aSimTime); // Call this for "on_update" script
   }
}

double WsfSA_Processor::TryUpdateFlightKinematics(double aSimTime)
{
   // Perform the update
   UpdateFlightKinematics(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cFLIGHT_DATA));
}

double WsfSA_Processor::TryUpdateFuel(double aSimTime)
{
   // Perform the update
   UpdateFuel(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cFUEL_DATA));
}

double WsfSA_Processor::TryUpdateNav(double aSimTime)
{
   // Perform the update
   UpdateNav(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cNAV_DATA));
}

double WsfSA_Processor::TryUpdateFlightControls(double aSimTime)
{
   // Perform the update
   UpdateFlightControls(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cCONTROLS_DATA));
}

double WsfSA_Processor::TryUpdateWeapons(double aSimTime)
{
   // Perform the update
   UpdateWeapons(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cWEAPONS_DATA));
}

double WsfSA_Processor::TryUpdateTracks(double aSimTime)
{
   // Perform the update
   UpdateTracks(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cTRACK_DATA));
}

double WsfSA_Processor::TryUpdateBehaviorCalculation(double aSimTime)
{
   // Perform the update
   UpdateBehaviorCalculation(aSimTime);

   return (aSimTime + GetUpdateInterval(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION));
}

void WsfSA_Processor::UpdateFlightKinematics(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cFLIGHT_DATA))
   {
      // Only update the data if there is a mover, else return
      if (!mMoverPresent)
      {
         return;
      }

      // Update the data
      UpdateFlightKinematicsData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendFlightKinematicsData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cFLIGHT_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateFuel(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cFUEL_DATA))
   {
      // Only update the data if there is a mover, else return
      if (!mMoverPresent)
      {
         return;
      }

      // Update the data
      UpdateFuelData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendFuelData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cFUEL_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateNav(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cNAV_DATA))
   {
      // Only update the data if there is a mover, else return
      if (!mMoverPresent)
      {
         return;
      }

      // Update the data
      UpdateNavData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendNavData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cNAV_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateFlightControls(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cCONTROLS_DATA))
   {
      // Only update the data if there is a mover, else return
      if (!mMoverPresent)
      {
         return;
      }

      // Update the data
      UpdateFlightControlsData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendFlightControlsData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cCONTROLS_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateWeapons(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cWEAPONS_DATA))
   {
      // Update the data
      UpdateWeaponsData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendWeaponsData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cWEAPONS_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateTracks(double aSimTime)
{
   if (aSimTime > GetLastUpdateTime(WsfSA_Processor::UpdateType::cTRACK_DATA))
   {
      // Update the data
      UpdateTrackData(aSimTime);

      // Send the data
      WsfObserver::AirCombatSendTrackData(GetSimulation())(aSimTime, *GetPlatform(), *this);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cTRACK_DATA, aSimTime);
   }
}

void WsfSA_Processor::UpdateFlightKinematicsData(double aSimTime)
{
   UtAtmosphere& atm      = GetSimulation()->GetAtmosphere();
   WsfPlatform*  platform = GetPlatform();
   if (platform != nullptr)
   {
      mAircraftKinematicsData.contrailing = atm.IsWithinContrailAltitudeBand(platform->GetAltitude());
   }
   else
   {
      mAircraftKinematicsData.contrailing = false;
   }

   if (mP6DofMover != nullptr)
   {
      P6DofVehicle* p6dofVehicle = mP6DofMover->GetP6DofVehicle();
      if (p6dofVehicle != nullptr)
      {
         mAircraftKinematicsData.lat_deg    = p6dofVehicle->GetLat();
         mAircraftKinematicsData.lon_deg    = p6dofVehicle->GetLon();
         mAircraftKinematicsData.kcas       = static_cast<float>(p6dofVehicle->GetKCAS());
         mAircraftKinematicsData.ktas       = static_cast<float>(p6dofVehicle->GetKTAS());
         mAircraftKinematicsData.mach       = static_cast<float>(p6dofVehicle->GetMach());
         mAircraftKinematicsData.altBaro_ft = static_cast<float>(p6dofVehicle->GetAltitudeMSL_ft());
         mAircraftKinematicsData.altRadar_ft =
            static_cast<float>(p6dofVehicle->GetAltitudeMSL_ft()); // This should later use RAD_ALT
         mAircraftKinematicsData.vertSpd_fpm   = static_cast<float>(p6dofVehicle->GetVertSpeed_fpm());
         mAircraftKinematicsData.alpha_deg     = static_cast<float>(p6dofVehicle->GetAlpha_deg());
         mAircraftKinematicsData.beta_deg      = static_cast<float>(p6dofVehicle->GetBeta_deg());
         mAircraftKinematicsData.gLoad         = static_cast<float>(mP6DofMover->GetGLoad());
         mAircraftKinematicsData.gAvail        = static_cast<float>(p6dofVehicle->MaxPotentialManeuverGLoad());
         mAircraftKinematicsData.heading_deg   = static_cast<float>(p6dofVehicle->GetHeading_deg());
         mAircraftKinematicsData.pitch_deg     = static_cast<float>(p6dofVehicle->GetPitch_deg());
         mAircraftKinematicsData.roll_deg      = static_cast<float>(p6dofVehicle->GetRoll_deg());
         mAircraftKinematicsData.gX            = static_cast<float>(p6dofVehicle->GetNx_g());
         mAircraftKinematicsData.gY            = static_cast<float>(p6dofVehicle->GetNy_g());
         mAircraftKinematicsData.pilotType     = mP6DofMover->GetActivePilot();
         mAircraftKinematicsData.afterburnerOn = mP6DofMover->GetAfterburnerOn();
         mAircraftKinematicsData.stallWarning  = p6dofVehicle->StallWarningAlert();
         mAircraftKinematicsData.alphaValid    = true;
         mAircraftKinematicsData.betaValid     = true;
         mAircraftKinematicsData.kcasValid     = true;
         mAircraftKinematicsData.nxValid       = true;
         mAircraftKinematicsData.nyValid       = true;
         mAircraftKinematicsData.nzValid       = true;
         mAircraftKinematicsData.gAvailValid   = true;

         return;
      }
   }
   else if (mSixDOF_Mover != nullptr)
   {
      mAircraftKinematicsData.lat_deg    = mSixDOF_Mover->GetLatitude_deg();
      mAircraftKinematicsData.lon_deg    = mSixDOF_Mover->GetLongitude_deg();
      mAircraftKinematicsData.kcas       = static_cast<float>(mSixDOF_Mover->GetKCAS());
      mAircraftKinematicsData.ktas       = static_cast<float>(mSixDOF_Mover->GetKTAS());
      mAircraftKinematicsData.mach       = static_cast<float>(mSixDOF_Mover->GetMach());
      mAircraftKinematicsData.altBaro_ft = static_cast<float>(mSixDOF_Mover->GetAltitudeMSL_ft());
      mAircraftKinematicsData.altRadar_ft =
         static_cast<float>(mSixDOF_Mover->GetAltitudeMSL_ft()); // This should later use RAD_ALT
      mAircraftKinematicsData.vertSpd_fpm   = static_cast<float>(mSixDOF_Mover->GetVertSpeed_fpm());
      mAircraftKinematicsData.alpha_deg     = static_cast<float>(mSixDOF_Mover->GetAlpha_deg());
      mAircraftKinematicsData.beta_deg      = static_cast<float>(mSixDOF_Mover->GetBeta_deg());
      mAircraftKinematicsData.gLoad         = static_cast<float>(mSixDOF_Mover->GetGLoad());
      mAircraftKinematicsData.gAvail        = static_cast<float>(mSixDOF_Mover->MaxPotentialManeuverGLoad());
      mAircraftKinematicsData.heading_deg   = static_cast<float>(mSixDOF_Mover->GetHeading_deg());
      mAircraftKinematicsData.pitch_deg     = static_cast<float>(mSixDOF_Mover->GetPitch_deg());
      mAircraftKinematicsData.roll_deg      = static_cast<float>(mSixDOF_Mover->GetRoll_deg());
      mAircraftKinematicsData.gX            = static_cast<float>(mSixDOF_Mover->GetNx_g());
      mAircraftKinematicsData.gY            = static_cast<float>(mSixDOF_Mover->GetNy_g());
      mAircraftKinematicsData.pilotType     = mSixDOF_Mover->GetActivePilot();
      mAircraftKinematicsData.afterburnerOn = mSixDOF_Mover->GetAfterburnerOn();
      mAircraftKinematicsData.stallWarning  = mSixDOF_Mover->StallWarningAlert();
      mAircraftKinematicsData.alphaValid    = true;
      mAircraftKinematicsData.betaValid     = true;
      mAircraftKinematicsData.kcasValid     = true;
      mAircraftKinematicsData.nxValid       = true;
      mAircraftKinematicsData.nyValid       = true;
      mAircraftKinematicsData.nzValid       = true;
      mAircraftKinematicsData.gAvailValid   = true;

      return;
   }
   else if (mBrawlerMover != nullptr)
   {
      double lat   = 0.0;
      double lon   = 0.0;
      double alt_m = 0.0;
      GetPlatform()->GetLocationLLA(lat, lon, alt_m);

      double speed_mps = GetPlatform()->GetSpeed();
      double ktas      = speed_mps * UtMath::cNM_PER_M * 3600.0;
      double alt_ft    = alt_m * UtMath::cFT_PER_M;

      double velNED[3] = {0.0, 0.0, 0.0};
      GetPlatform()->GetVelocityNED(velNED);
      double vertSpd_fpm = -velNED[2] * UtMath::cFT_PER_M * 60.0;

      double alpha_deg                    = mBrawlerMover->Alpha() * UtMath::cDEG_PER_RAD;
      double maxAlpha_deg                 = mBrawlerMover->aoamx * UtMath::cDEG_PER_RAD;
      double assumedStallWarningAngle_deg = maxAlpha_deg - 2.0;

      bool stallWarning = false;
      if (alpha_deg > assumedStallWarningAngle_deg)
      {
         stallWarning = true;
      }

      double accelECS[3] = {0.0, 0.0, 0.0};
      GetPlatform()->GetAccelerationECS(accelECS);
      double nX = accelECS[0] / 9.81;
      double nY = accelECS[1] / 9.81;
      double nZ = accelECS[2] / 9.81;

      double heading_rad = 0.0;
      double pitch_rad   = 0.0;
      double roll_rad    = 0.0;
      GetPlatform()->GetOrientationNED(heading_rad, pitch_rad, roll_rad);

      mAircraftKinematicsData.lat_deg = lat;
      mAircraftKinematicsData.lon_deg = lon;
      mAircraftKinematicsData.kcas = static_cast<float>(mBrawlerMover->CalibratedAirSpeed() * UtMath::cNM_PER_FT * 3600.0);
      mAircraftKinematicsData.ktas         = static_cast<float>(ktas);
      mAircraftKinematicsData.mach         = static_cast<float>(mBrawlerMover->Mach());
      mAircraftKinematicsData.altBaro_ft   = static_cast<float>(alt_ft);
      mAircraftKinematicsData.altRadar_ft  = static_cast<float>(alt_ft);
      mAircraftKinematicsData.vertSpd_fpm  = static_cast<float>(vertSpd_fpm);
      mAircraftKinematicsData.alpha_deg    = static_cast<float>(alpha_deg);
      mAircraftKinematicsData.beta_deg     = 0.0f;
      mAircraftKinematicsData.gLoad        = static_cast<float>(-nZ);
      mAircraftKinematicsData.gAvail       = 0.0f;
      mAircraftKinematicsData.heading_deg  = static_cast<float>(heading_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.pitch_deg    = static_cast<float>(pitch_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.roll_deg     = static_cast<float>(roll_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.gX           = static_cast<float>(nX);
      mAircraftKinematicsData.gY           = static_cast<float>(nY);
      mAircraftKinematicsData.pilotType    = "";
      mAircraftKinematicsData.stallWarning = stallWarning;
      mAircraftKinematicsData.alphaValid   = true;
      mAircraftKinematicsData.betaValid    = false;
      mAircraftKinematicsData.kcasValid    = true;
      mAircraftKinematicsData.nxValid      = true;
      mAircraftKinematicsData.nyValid      = true;
      mAircraftKinematicsData.nzValid      = true;
      mAircraftKinematicsData.gAvailValid  = false;

      // Note: Brawler uses the following throttle values: 1 = idle, 2 = mil, 3 = ab
      if (mBrawlerMover->GetThrottlePosition() > 2.0)
      {
         mAircraftKinematicsData.afterburnerOn = true;
      }
      else
      {
         mAircraftKinematicsData.afterburnerOn = false;
      }

      return;
   }
   else if (mAirMover != nullptr)
   {
      double lat   = 0.0;
      double lon   = 0.0;
      double alt_m = 0.0;
      GetPlatform()->GetLocationLLA(lat, lon, alt_m);

      double speed_mps = GetPlatform()->GetSpeed();
      double ktas      = speed_mps * UtMath::cNM_PER_M * 3600.0;
      double alt_ft    = alt_m * UtMath::cFT_PER_M;

      double velNED[3] = {0.0, 0.0, 0.0};
      GetPlatform()->GetVelocityNED(velNED);
      double vertSpd_fpm = -velNED[2] * UtMath::cFT_PER_M * 60.0;

      double heading_rad = 0.0;
      double pitch_rad   = 0.0;
      double roll_rad    = 0.0;
      GetPlatform()->GetOrientationNED(heading_rad, pitch_rad, roll_rad);

      mAircraftKinematicsData.lat_deg       = lat;
      mAircraftKinematicsData.lon_deg       = lon;
      mAircraftKinematicsData.kcas          = 0.0f;
      mAircraftKinematicsData.ktas          = static_cast<float>(ktas);
      mAircraftKinematicsData.mach          = 0.0f;
      mAircraftKinematicsData.altBaro_ft    = static_cast<float>(alt_ft);
      mAircraftKinematicsData.altRadar_ft   = static_cast<float>(alt_ft);
      mAircraftKinematicsData.vertSpd_fpm   = static_cast<float>(vertSpd_fpm);
      mAircraftKinematicsData.alpha_deg     = 0.0f;
      mAircraftKinematicsData.beta_deg      = 0.0f;
      mAircraftKinematicsData.gLoad         = 0.0f;
      mAircraftKinematicsData.gAvail        = 0.0f;
      mAircraftKinematicsData.heading_deg   = static_cast<float>(heading_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.pitch_deg     = static_cast<float>(pitch_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.roll_deg      = static_cast<float>(roll_rad * UtMath::cDEG_PER_RAD);
      mAircraftKinematicsData.gX            = 0.0f;
      mAircraftKinematicsData.gY            = 0.0f;
      mAircraftKinematicsData.pilotType     = "";
      mAircraftKinematicsData.afterburnerOn = false;
      mAircraftKinematicsData.stallWarning  = false;
      mAircraftKinematicsData.alphaValid    = false;
      mAircraftKinematicsData.betaValid     = false;
      mAircraftKinematicsData.kcasValid     = false;
      mAircraftKinematicsData.nxValid       = false;
      mAircraftKinematicsData.nyValid       = false;
      mAircraftKinematicsData.nzValid       = false;
      mAircraftKinematicsData.gAvailValid   = false;

      return;
   }

   // No valid mover -- zero-out the data
   mAircraftKinematicsData.lat_deg       = 0.0;
   mAircraftKinematicsData.lon_deg       = 0.0;
   mAircraftKinematicsData.kcas          = 0.0f;
   mAircraftKinematicsData.ktas          = 0.0f;
   mAircraftKinematicsData.mach          = 0.0f;
   mAircraftKinematicsData.altBaro_ft    = 0.0f;
   mAircraftKinematicsData.altRadar_ft   = 0.0f;
   mAircraftKinematicsData.vertSpd_fpm   = 0.0f;
   mAircraftKinematicsData.alpha_deg     = 0.0f;
   mAircraftKinematicsData.beta_deg      = 0.0f;
   mAircraftKinematicsData.gLoad         = 0.0f;
   mAircraftKinematicsData.gAvail        = 0.0f;
   mAircraftKinematicsData.heading_deg   = 0.0f;
   mAircraftKinematicsData.pitch_deg     = 0.0f;
   mAircraftKinematicsData.roll_deg      = 0.0f;
   mAircraftKinematicsData.gX            = 0.0f;
   mAircraftKinematicsData.gY            = 0.0f;
   mAircraftKinematicsData.pilotType     = "";
   mAircraftKinematicsData.afterburnerOn = false;
   mAircraftKinematicsData.stallWarning  = false;
   mAircraftKinematicsData.alphaValid    = false;
   mAircraftKinematicsData.betaValid     = false;
   mAircraftKinematicsData.kcasValid     = false;
   mAircraftKinematicsData.nxValid       = false;
   mAircraftKinematicsData.nyValid       = false;
   mAircraftKinematicsData.nzValid       = false;
   mAircraftKinematicsData.gAvailValid   = false;
}

void WsfSA_Processor::UpdateFuelData(double aSimTime)
{
   if (mP6DofMover != nullptr)
   {
      P6DofVehicle* p6dofVehicle = mP6DofMover->GetP6DofVehicle();
      if (p6dofVehicle != nullptr)
      {
         // Verify we have a fuel system if we have a non-zero capacity of internal fuel
         if (p6dofVehicle->GetInternalFuelTankCapacity_lbs() > 0)
         {
            mFuelSystemData.fuelInternal_lbs = static_cast<float>(p6dofVehicle->GetCurrentInternalFuelTankQuantity_lbs());
            mFuelSystemData.fuelExternal_lbs = static_cast<float>(p6dofVehicle->GetCurrentExternalFuelTankQuantity_lbs());
            mFuelSystemData.fuelCapInt_lbs   = static_cast<float>(p6dofVehicle->GetInternalFuelTankCapacity_lbs());
            mFuelSystemData.fuelCapExt_lbs   = static_cast<float>(p6dofVehicle->GetExternalFuelTankCapacity_lbs());
            mFuelSystemData.grossWgt_lbs     = static_cast<float>(p6dofVehicle->GetCurrentWeight_lbs());

            if (mJokerFuel_lbs > 0.0)
            {
               mFuelSystemData.joker_lbs = static_cast<float>(mJokerFuel_lbs);
            }
            else
            {
               mFuelSystemData.joker_lbs = static_cast<float>(mP6DofMover->GetJokerFuelState() * UtMath::cLB_PER_KG);
            }

            if (mBingoFuel_lbs > 0.0)
            {
               mFuelSystemData.bingo_lbs = static_cast<float>(mBingoFuel_lbs);
            }
            else
            {
               mFuelSystemData.bingo_lbs = static_cast<float>(mP6DofMover->GetBingoFuelState() * UtMath::cLB_PER_KG);
            }

            mFuelSystemData.fuelFlow_pph = static_cast<float>(p6dofVehicle->GetTotalFuelFlow_pph());
            mFuelSystemData.numExtTanks  = static_cast<uint8_t>(p6dofVehicle->GetNumberOfExternalTanks());

            if (mFuelSystemData.numExtTanks > 0)
            {
               mFuelSystemData.dropTanksPresent = true;
            }
            else
            {
               mFuelSystemData.dropTanksPresent = false;
            }

            mFuelSystemData.normalizedFuelQty =
               (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) / mFuelSystemData.fuelCapInt_lbs;
            if (mFuelSystemData.normalizedFuelQty > 1.0)
            {
               mFuelSystemData.normalizedFuelQty = 1.0;
            }

            mFuelSystemData.fuelValid = true;

            return;
         }
      }
   }
   else if (mSixDOF_Mover != nullptr)
   {
      // Verify we have a fuel system if we have a non-zero capacity of internal fuel
      if (mSixDOF_Mover->GetInternalFuelTankCapacity_lbs() > 0)
      {
         mFuelSystemData.fuelInternal_lbs = static_cast<float>(mSixDOF_Mover->GetCurrentInternalFuelTankQuantity_lbs());
         mFuelSystemData.fuelExternal_lbs = static_cast<float>(mSixDOF_Mover->GetCurrentExternalFuelTankQuantity_lbs());
         mFuelSystemData.fuelCapInt_lbs   = static_cast<float>(mSixDOF_Mover->GetInternalFuelTankCapacity_lbs());
         mFuelSystemData.fuelCapExt_lbs   = static_cast<float>(mSixDOF_Mover->GetExternalFuelTankCapacity_lbs());
         mFuelSystemData.grossWgt_lbs     = static_cast<float>(mSixDOF_Mover->GetCurrentWeight_lbs());

         if (mJokerFuel_lbs > 0.0)
         {
            mFuelSystemData.joker_lbs = static_cast<float>(mJokerFuel_lbs);
         }
         else
         {
            mFuelSystemData.joker_lbs = static_cast<float>(mSixDOF_Mover->GetJokerFuelState() * UtMath::cLB_PER_KG);
         }

         if (mBingoFuel_lbs > 0.0)
         {
            mFuelSystemData.bingo_lbs = static_cast<float>(mBingoFuel_lbs);
         }
         else
         {
            mFuelSystemData.bingo_lbs = static_cast<float>(mSixDOF_Mover->GetBingoFuelState() * UtMath::cLB_PER_KG);
         }

         mFuelSystemData.fuelFlow_pph = static_cast<float>(mSixDOF_Mover->GetTotalFuelFlow_pph());
         mFuelSystemData.numExtTanks  = static_cast<uint8_t>(mSixDOF_Mover->GetNumberOfExternalTanks());

         if (mFuelSystemData.numExtTanks > 0)
         {
            mFuelSystemData.dropTanksPresent = true;
         }
         else
         {
            mFuelSystemData.dropTanksPresent = false;
         }

         mFuelSystemData.normalizedFuelQty =
            (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) / mFuelSystemData.fuelCapInt_lbs;
         if (mFuelSystemData.normalizedFuelQty > 1.0)
         {
            mFuelSystemData.normalizedFuelQty = 1.0;
         }

         mFuelSystemData.fuelValid = true;

         return;
      }
   }
   else if ((mBrawlerMover != nullptr) && (mBrawlerFuel != nullptr))
   {
      mFuelSystemData.fuelInternal_lbs = static_cast<float>(mBrawlerFuel->GetQuantityRemaining() * UtMath::cLB_PER_KG);
      mFuelSystemData.fuelExternal_lbs = 0.0f;
      mFuelSystemData.fuelCapInt_lbs   = static_cast<float>(mBrawlerFuel->GetInitialQuantity() * UtMath::cLB_PER_KG);
      mFuelSystemData.fuelCapExt_lbs   = 0.0f;
      mFuelSystemData.grossWgt_lbs =
         static_cast<float>(mBrawlerMover->AircraftMass() * UtMath::cKG_PER_SLUG * UtMath::cLB_PER_KG);

      if (mJokerFuel_lbs > 0.0)
      {
         mFuelSystemData.joker_lbs = static_cast<float>(mJokerFuel_lbs);
      }
      else
      {
         mFuelSystemData.joker_lbs = static_cast<float>(mBrawlerFuel->GetBingoQuantity() * UtMath::cLB_PER_KG);
      }

      if (mBingoFuel_lbs > 0.0)
      {
         mFuelSystemData.bingo_lbs = static_cast<float>(mBingoFuel_lbs);
      }
      else
      {
         mFuelSystemData.bingo_lbs = static_cast<float>(mBrawlerFuel->GetBingoQuantity() * UtMath::cLB_PER_KG);
      }

      mFuelSystemData.fuelFlow_pph = static_cast<float>(mBrawlerFuel->GetConsumptionRate() * UtMath::cLB_PER_KG * 3600.0);
      mFuelSystemData.numExtTanks      = 0;
      mFuelSystemData.dropTanksPresent = false;

      mFuelSystemData.normalizedFuelQty =
         (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) / mFuelSystemData.fuelCapInt_lbs;
      if (mFuelSystemData.normalizedFuelQty > 1.0)
      {
         mFuelSystemData.normalizedFuelQty = 1.0;
      }

      mFuelSystemData.fuelValid = true;

      return;
   }
   else if ((mAirMover != nullptr) && (mAirMoverFuel != nullptr))
   {
      mFuelSystemData.fuelInternal_lbs = static_cast<float>(mAirMoverFuel->GetQuantityRemaining() * UtMath::cLB_PER_KG);
      mFuelSystemData.fuelExternal_lbs = 0.0f;
      mFuelSystemData.fuelCapInt_lbs   = static_cast<float>(mAirMoverFuel->GetInitialQuantity() * UtMath::cLB_PER_KG);
      mFuelSystemData.fuelCapExt_lbs   = 0.0f;
      mFuelSystemData.grossWgt_lbs     = static_cast<float>(GetPlatform()->GetEmptyMass() * UtMath::cLB_PER_KG +
                                                        mAirMoverFuel->GetQuantityRemaining() * UtMath::cLB_PER_KG);

      if (mJokerFuel_lbs > 0.0)
      {
         mFuelSystemData.joker_lbs = static_cast<float>(mJokerFuel_lbs);
      }
      else
      {
         mFuelSystemData.joker_lbs = static_cast<float>(mAirMoverFuel->GetBingoQuantity() * UtMath::cLB_PER_KG);
      }

      if (mBingoFuel_lbs > 0.0)
      {
         mFuelSystemData.bingo_lbs = static_cast<float>(mBingoFuel_lbs);
      }
      else
      {
         mFuelSystemData.bingo_lbs = static_cast<float>(mAirMoverFuel->GetBingoQuantity() * UtMath::cLB_PER_KG);
      }

      mFuelSystemData.fuelFlow_pph = static_cast<float>(mAirMoverFuel->GetConsumptionRate() * UtMath::cLB_PER_KG * 3600.0);
      mFuelSystemData.numExtTanks      = 0;
      mFuelSystemData.dropTanksPresent = false;

      mFuelSystemData.normalizedFuelQty =
         (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) / mFuelSystemData.fuelCapInt_lbs;
      if (mFuelSystemData.normalizedFuelQty > 1.0)
      {
         mFuelSystemData.normalizedFuelQty = 1.0;
      }

      mFuelSystemData.fuelValid = true;

      return;
   }

   // Unknown mover type or no fuel system present
   mFuelSystemData.fuelInternal_lbs  = 0.0f;
   mFuelSystemData.fuelExternal_lbs  = 0.0f;
   mFuelSystemData.fuelCapInt_lbs    = 0.0f;
   mFuelSystemData.fuelCapExt_lbs    = 0.0f;
   mFuelSystemData.grossWgt_lbs      = 0.0f;
   mFuelSystemData.joker_lbs         = 0.0f;
   mFuelSystemData.bingo_lbs         = 0.0f;
   mFuelSystemData.fuelFlow_pph      = 0.0f;
   mFuelSystemData.normalizedFuelQty = 0.0f;
   mFuelSystemData.numExtTanks       = 0;
   mFuelSystemData.dropTanksPresent  = false;
   mFuelSystemData.fuelValid         = false;
}

void WsfSA_Processor::UpdateNavData(double aSimTime)
{
   // Get the mover from the platform
   WsfMover* platformMover = GetPlatform()->GetMover();
   if (platformMover == nullptr)
   {
      // Setup empty data since there is no route and/or no waypoints
      mNavigationDataSummary.dataValid             = false;
      mNavigationDataSummary.numWaypoints          = 0;
      mNavigationDataSummary.currentWaypoint       = 0;
      mNavigationDataSummary.headingBug_deg        = 0.0f;
      mNavigationDataSummary.distanceToWaypoint_nm = 0.0f;
      mNavigationDataSummary.timeToWaypoint_sec    = 0.0f;
      mNavigationDataSummary.waypoints.clear();

      return;
   }

   // Get the route from the mover
   const WsfRoute* route = platformMover->GetRoute();

   if (route != nullptr)
   {
      route->GetSize();
      size_t numWp = static_cast<size_t>(route->GetSize());

      // Load the waypoint list
      std::vector<SA_NavigationLatLon> wpList;
      const std::vector<WsfWaypoint>   wpVector = route->GetWaypoints();
      for (auto& wp : wpVector)
      {
         SA_NavigationLatLon wayPoint;
         wayPoint.lat_deg = static_cast<float>(wp.GetLat());
         wayPoint.lon_deg = static_cast<float>(wp.GetLon());
         wpList.push_back(wayPoint);
      }

      if (numWp > 0)
      {
         // Get the current waypoint
         size_t wayPtIndex = static_cast<size_t>(platformMover->RoutePointIndex());

         // Verify correct limits
         if (wayPtIndex < 0)
         {
            wayPtIndex = 0;
         }
         if (wayPtIndex >= numWp)
         {
            wayPtIndex = numWp - 1;
         }

         // Get the lat/lon of the current waypoint
         double wpLat = wpList.at(wayPtIndex).lat_deg;
         double wpLon = wpList.at(wayPtIndex).lon_deg;

         // Gte the platform's location (LLA)
         double platLat = 0.0;
         double platLon = 0.0;
         double platAlt = 0.0;
         GetPlatform()->GetLocationLLA(platLat, platLon, platAlt);

         // Calc the heading and distance to waypoint
         double heading_deg = 0.0;
         double distance_m  = 0.0;
         UtSphericalEarth::GreatCircleHeadingAndDistance(platLat, platLon, wpLat, wpLon, heading_deg, distance_m);

         // Calc the time to waypoint
         double velNED[3];
         GetPlatform()->GetVelocityNED(velNED);
         double speed          = sqrt(velNED[0] * velNED[0] + velNED[1] * velNED[1]);
         double timeToWaypoint = distance_m / speed;

         // Clear old list and set new waypoints
         mNavigationDataSummary.waypoints.clear();
         for (auto& wp : wpList)
         {
            SA_NavigationLatLon tmpWaypt;
            tmpWaypt.lat_deg = wp.lat_deg;
            tmpWaypt.lon_deg = wp.lon_deg;
            mNavigationDataSummary.waypoints.push_back(tmpWaypt);
         }

         mNavigationDataSummary.dataValid             = true;
         mNavigationDataSummary.numWaypoints          = static_cast<uint8_t>(numWp);
         mNavigationDataSummary.currentWaypoint       = static_cast<uint8_t>(wayPtIndex);
         mNavigationDataSummary.headingBug_deg        = static_cast<float>(heading_deg);
         mNavigationDataSummary.distanceToWaypoint_nm = static_cast<float>(distance_m * UtMath::cNM_PER_M);
         mNavigationDataSummary.timeToWaypoint_sec    = static_cast<float>(timeToWaypoint);

         return;
      }
   }

   // Setup empty data since there is no route and/or no waypoints
   mNavigationDataSummary.dataValid             = false;
   mNavigationDataSummary.numWaypoints          = 0;
   mNavigationDataSummary.currentWaypoint       = 0;
   mNavigationDataSummary.headingBug_deg        = 0.0f;
   mNavigationDataSummary.distanceToWaypoint_nm = 0.0f;
   mNavigationDataSummary.timeToWaypoint_sec    = 0.0f;
   mNavigationDataSummary.waypoints.clear();
}

void WsfSA_Processor::UpdateFlightControlsData(double aSimTime)
{
   if (mP6DofMover != nullptr)
   {
      P6DofVehicle* p6dofVehicle = mP6DofMover->GetP6DofVehicle();

      if (p6dofVehicle != nullptr)
      {
         mFlightControlsDataSummary.stickRgtInput        = static_cast<float>(mP6DofMover->GetStickRightInput());
         mFlightControlsDataSummary.stickBackInput       = static_cast<float>(mP6DofMover->GetStickBackInput());
         mFlightControlsDataSummary.rudderRgtInput       = static_cast<float>(mP6DofMover->GetRudderRightInput());
         mFlightControlsDataSummary.speedBrakeInput      = static_cast<float>(mP6DofMover->GetSpeedBrakeInput());
         mFlightControlsDataSummary.throttleInput        = static_cast<float>(mP6DofMover->GetThrottleInput());
         mFlightControlsDataSummary.aileronRgtNormalized = static_cast<float>(mP6DofMover->GetNormalizedAileronRight());
         mFlightControlsDataSummary.aileronLftNormalized = static_cast<float>(mP6DofMover->GetNormalizedAileronLeft());
         mFlightControlsDataSummary.flapRgtNormalized    = static_cast<float>(mP6DofMover->GetNormalizedFlapRight());
         mFlightControlsDataSummary.flapLftNormalized    = static_cast<float>(mP6DofMover->GetNormalizedFlapLeft());
         mFlightControlsDataSummary.stabilizerRgtNormalized =
            static_cast<float>(mP6DofMover->GetNormalizedStabilizerRight());
         mFlightControlsDataSummary.stabilizerLftNormalized =
            static_cast<float>(mP6DofMover->GetNormalizedStabilizerLeft());
         mFlightControlsDataSummary.rudderRgtNormalized  = static_cast<float>(mP6DofMover->GetNormalizedRudderRight());
         mFlightControlsDataSummary.rudderLftNormalized  = static_cast<float>(mP6DofMover->GetNormalizedRudderLeft());
         mFlightControlsDataSummary.rudderNormalized     = static_cast<float>(mP6DofMover->GetNormalizedRudder());
         mFlightControlsDataSummary.speedBrakeNormalized = static_cast<float>(mP6DofMover->GetNormalizedSpeedBrake());
         mFlightControlsDataSummary.thrustNormalized     = static_cast<float>(p6dofVehicle->GetNormalizedTotalThrust());
         mFlightControlsDataSummary.landingGear          = static_cast<float>(mP6DofMover->GetNormalizedLandingGear());

         mFlightControlsDataSummary.throttleValid = p6dofVehicle->ThrottleValid();

         if (p6dofVehicle->AileronLeftValid())
         {
            mFlightControlsDataSummary.aileronsValid = true;
         }
         else
         {
            mFlightControlsDataSummary.aileronsValid = false;
         }

         if (p6dofVehicle->FlapLeftValid())
         {
            mFlightControlsDataSummary.flapsValid = true;
         }
         else
         {
            mFlightControlsDataSummary.flapsValid = false;
         }

         if (p6dofVehicle->RudderLeftValid())
         {
            mFlightControlsDataSummary.twinRuddersValid = true;
         }
         else
         {
            mFlightControlsDataSummary.twinRuddersValid = false;
         }

         if (p6dofVehicle->RudderValid())
         {
            mFlightControlsDataSummary.singleRudderValid = true;
         }
         else
         {
            mFlightControlsDataSummary.singleRudderValid = false;
         }

         return;
      }
   }
   else if (mSixDOF_Mover != nullptr)
   {
      mFlightControlsDataSummary.stickRgtInput        = static_cast<float>(mSixDOF_Mover->GetStickRightInput());
      mFlightControlsDataSummary.stickBackInput       = static_cast<float>(mSixDOF_Mover->GetStickBackInput());
      mFlightControlsDataSummary.rudderRgtInput       = static_cast<float>(mSixDOF_Mover->GetRudderRightInput());
      mFlightControlsDataSummary.speedBrakeInput      = static_cast<float>(mSixDOF_Mover->GetSpeedBrakeInput());
      mFlightControlsDataSummary.throttleInput        = static_cast<float>(mSixDOF_Mover->GetThrottleInput());
      mFlightControlsDataSummary.thrustNormalized     = static_cast<float>(mSixDOF_Mover->GetNormalizedTotalThrust());
      mFlightControlsDataSummary.aileronRgtNormalized = static_cast<float>(mSixDOF_Mover->GetNormalizedAileronRight());
      mFlightControlsDataSummary.aileronLftNormalized = static_cast<float>(mSixDOF_Mover->GetNormalizedAileronLeft());
      mFlightControlsDataSummary.flapRgtNormalized    = static_cast<float>(mSixDOF_Mover->GetNormalizedFlapRight());
      mFlightControlsDataSummary.flapLftNormalized    = static_cast<float>(mSixDOF_Mover->GetNormalizedFlapLeft());
      mFlightControlsDataSummary.stabilizerRgtNormalized =
         static_cast<float>(mSixDOF_Mover->GetNormalizedStabilizerRight());
      mFlightControlsDataSummary.stabilizerLftNormalized =
         static_cast<float>(mSixDOF_Mover->GetNormalizedStabilizerLeft());
      mFlightControlsDataSummary.rudderRgtNormalized  = static_cast<float>(mSixDOF_Mover->GetNormalizedRudderRight());
      mFlightControlsDataSummary.rudderLftNormalized  = static_cast<float>(mSixDOF_Mover->GetNormalizedRudderLeft());
      mFlightControlsDataSummary.rudderNormalized     = static_cast<float>(mSixDOF_Mover->GetNormalizedRudder());
      mFlightControlsDataSummary.speedBrakeNormalized = static_cast<float>(mSixDOF_Mover->GetNormalizedSpeedBrake());
      mFlightControlsDataSummary.landingGear          = static_cast<float>(mSixDOF_Mover->GetNormalizedLandingGear());

      mFlightControlsDataSummary.throttleValid = mSixDOF_Mover->ThrottleValid();

      if (mSixDOF_Mover->AileronLeftValid())
      {
         mFlightControlsDataSummary.aileronsValid = true;
      }
      else
      {
         mFlightControlsDataSummary.aileronsValid = false;
      }

      if (mSixDOF_Mover->FlapLeftValid())
      {
         mFlightControlsDataSummary.flapsValid = true;
      }
      else
      {
         mFlightControlsDataSummary.flapsValid = false;
      }

      if (mSixDOF_Mover->RudderLeftValid())
      {
         mFlightControlsDataSummary.twinRuddersValid = true;
      }
      else
      {
         mFlightControlsDataSummary.twinRuddersValid = false;
      }

      if (mSixDOF_Mover->RudderValid())
      {
         mFlightControlsDataSummary.singleRudderValid = true;
      }
      else
      {
         mFlightControlsDataSummary.singleRudderValid = false;
      }

      return;
   }
   else if (mBrawlerMover != nullptr)
   {
      // Note:  Brawler provides some throttle insight, but no other controls insight
      //        It also uses the following values: 1 = idle, 2 = mil, 3 = ab

      mFlightControlsDataSummary.stickRgtInput        = 0.0f;
      mFlightControlsDataSummary.stickBackInput       = 0.0f;
      mFlightControlsDataSummary.rudderRgtInput       = 0.0f;
      mFlightControlsDataSummary.speedBrakeInput      = 0.0f;
      mFlightControlsDataSummary.throttleInput        = static_cast<float>(mBrawlerMover->GetThrottlePosition() - 1.0);
      mFlightControlsDataSummary.aileronRgtNormalized = 0.0f;
      mFlightControlsDataSummary.aileronLftNormalized = 0.0f;
      mFlightControlsDataSummary.flapRgtNormalized    = 0.0f;
      mFlightControlsDataSummary.flapLftNormalized    = 0.0f;
      mFlightControlsDataSummary.stabilizerRgtNormalized = 0.0f;
      mFlightControlsDataSummary.stabilizerLftNormalized = 0.0f;
      mFlightControlsDataSummary.rudderRgtNormalized     = 0.0f;
      mFlightControlsDataSummary.rudderLftNormalized     = 0.0f;
      mFlightControlsDataSummary.rudderNormalized        = 0.0f;
      mFlightControlsDataSummary.speedBrakeNormalized    = 0.0f;
      mFlightControlsDataSummary.thrustNormalized        = static_cast<float>(mBrawlerMover->Throttle() - 1.0);
      mFlightControlsDataSummary.landingGear             = 0.0f;
      mFlightControlsDataSummary.throttleValid           = true;
      mFlightControlsDataSummary.aileronsValid           = false;
      mFlightControlsDataSummary.flapsValid              = false;
      mFlightControlsDataSummary.twinRuddersValid        = false;
      mFlightControlsDataSummary.singleRudderValid       = false;

      return;
   }

   // All other movers lack controls
   mFlightControlsDataSummary.stickRgtInput           = 0.0f;
   mFlightControlsDataSummary.stickBackInput          = 0.0f;
   mFlightControlsDataSummary.rudderRgtInput          = 0.0f;
   mFlightControlsDataSummary.speedBrakeInput         = 0.0f;
   mFlightControlsDataSummary.throttleInput           = 0.0f;
   mFlightControlsDataSummary.aileronRgtNormalized    = 0.0f;
   mFlightControlsDataSummary.aileronLftNormalized    = 0.0f;
   mFlightControlsDataSummary.flapRgtNormalized       = 0.0f;
   mFlightControlsDataSummary.flapLftNormalized       = 0.0f;
   mFlightControlsDataSummary.stabilizerRgtNormalized = 0.0f;
   mFlightControlsDataSummary.stabilizerLftNormalized = 0.0f;
   mFlightControlsDataSummary.rudderRgtNormalized     = 0.0f;
   mFlightControlsDataSummary.rudderLftNormalized     = 0.0f;
   mFlightControlsDataSummary.rudderNormalized        = 0.0f;
   mFlightControlsDataSummary.speedBrakeNormalized    = 0.0f;
   mFlightControlsDataSummary.thrustNormalized        = 0.0f;
   mFlightControlsDataSummary.landingGear             = 0.0f;
   mFlightControlsDataSummary.throttleValid           = false;
   mFlightControlsDataSummary.aileronsValid           = false;
   mFlightControlsDataSummary.flapsValid              = false;
   mFlightControlsDataSummary.twinRuddersValid        = false;
   mFlightControlsDataSummary.singleRudderValid       = false;
}

void WsfSA_Processor::UpdateWeaponsData(double aSimTime)
{
   // List of available weapons
   mWeaponsDataSummary.weaponList.clear();
   const std::vector<WsfExplicitWeapon*> weaponList = mPerceivePtr->GetWeaponList();
   for (auto& wpn : weaponList)
   {
      // Setup a temp weapon item
      SA_WeaponNameQtyPair tmpWeapon;
      tmpWeapon.weaponName = wpn->GetName();
      // We add a slight amount extra to avoid a round-down
      unsigned char num  = static_cast<unsigned char>(wpn->GetQuantityRemaining() + 0.01);
      tmpWeapon.quantity = num;

      // Add to the list
      mWeaponsDataSummary.weaponList.push_back(tmpWeapon);
   }

   // Selected weapon
   unsigned char            numWeapons     = 0;
   const WsfExplicitWeapon* selectedWeapon = mPerceivePtr->SelectedWeapon();
   if (selectedWeapon != nullptr)
   {
      // We add a slight amount extra to avoid a round-down
      numWeapons = static_cast<unsigned char>(selectedWeapon->GetQuantityRemaining() + 0.01);

      mWeaponsDataSummary.selectedWeapon    = selectedWeapon->GetName();
      mWeaponsDataSummary.numSelectedWeapon = numWeapons;
   }
   else
   {
      mWeaponsDataSummary.selectedWeapon    = "";
      mWeaponsDataSummary.numSelectedWeapon = 0;
   }

   // Engaged target
   // TODO -- Use a specified track to set the engaged target data
   SA_EngagedTarget tmpEngagedTarget;
   tmpEngagedTarget.isValid          = false;
   tmpEngagedTarget.platformIndex    = 0;
   tmpEngagedTarget.rangeMax         = 0.0f;
   tmpEngagedTarget.rangeNe          = 0.0f;
   tmpEngagedTarget.rangeMin         = 0.0f;
   tmpEngagedTarget.rangeCurrent     = 0.0f;
   tmpEngagedTarget.rangeClosureRate = 0.0f;
   tmpEngagedTarget.timeToIntercept  = 0.0f;
   tmpEngagedTarget.timeToActive     = 0.0f;
   tmpEngagedTarget.targetKTAS       = 0.0f;
   tmpEngagedTarget.targetAspect     = 0.0f;
   tmpEngagedTarget.targetAz         = 0.0f;
   tmpEngagedTarget.targetEl         = 0.0f;
   SetEngagedTarget(tmpEngagedTarget);

   mWeaponsDataSummary.engagedTargetList.clear();
   if (EngagedTargetValid())
   {
      // Get the engaged target data from the SA Processor
      SA_EngagedTarget engagedTarget = GetEngagedTarget();

      // Load the engaged target
      SA_WeaponEngagedTarget tgtData;
      tgtData.targetIndex          = static_cast<unsigned int>(engagedTarget.platformIndex);
      tgtData.rangeMax_nm          = static_cast<float>(engagedTarget.rangeMax * UtMath::cNM_PER_M);
      tgtData.rangeMax_nm          = static_cast<float>(engagedTarget.rangeMax * UtMath::cNM_PER_M);
      tgtData.rangeNe_nm           = static_cast<float>(engagedTarget.rangeNe * UtMath::cNM_PER_M);
      tgtData.rangeMin_nm          = static_cast<float>(engagedTarget.rangeMin * UtMath::cNM_PER_M);
      tgtData.rangeCurrent_nm      = static_cast<float>(engagedTarget.rangeCurrent * UtMath::cNM_PER_M);
      tgtData.rangeClosureRate_kts = static_cast<float>(engagedTarget.rangeClosureRate * UtMath::cNM_PER_M * 3600.0);
      tgtData.timeToIntercept_sec  = static_cast<float>(engagedTarget.timeToIntercept);
      tgtData.timeToActive_sec     = static_cast<float>(engagedTarget.timeToActive);
      tgtData.targetKTAS           = static_cast<float>(engagedTarget.targetKTAS);
      tgtData.targetAspect_deg     = static_cast<float>(engagedTarget.targetAspect * UtMath::cDEG_PER_RAD);
      tgtData.targetAz_deg         = static_cast<float>(engagedTarget.targetAz * UtMath::cDEG_PER_RAD);
      tgtData.targetEl_deg         = static_cast<float>(engagedTarget.targetEl * UtMath::cDEG_PER_RAD);

      // Set the data
      mWeaponsDataSummary.engagedTargetList.push_back(tgtData);
   }
   else
   {
      // Load the engaged target
      SA_WeaponEngagedTarget tgtData;
      tgtData.targetIndex          = 0;
      tgtData.rangeMax_nm          = 0.0f;
      tgtData.rangeNe_nm           = 0.0f;
      tgtData.rangeMin_nm          = 0.0f;
      tgtData.rangeCurrent_nm      = 0.0f;
      tgtData.rangeClosureRate_kts = 0.0f;
      tgtData.timeToIntercept_sec  = 0.0f;
      tgtData.timeToActive_sec     = 0.0f;
      tgtData.targetKTAS           = 0.0f;
      tgtData.targetAspect_deg     = 0.0f;
      tgtData.targetAz_deg         = 0.0f;
      tgtData.targetEl_deg         = 0.0f;

      // Set the data
      mWeaponsDataSummary.engagedTargetList.push_back(tgtData);
   }

   // Expendable Countermeasures
   mWeaponsDataSummary.numChaff  = (unsigned char)QuantityOfChaff();
   mWeaponsDataSummary.numFlares = (unsigned char)QuantityOfFlares();
   mWeaponsDataSummary.numDecoys = (unsigned char)QuantityOfDecoys();

   // Flags
   mWeaponsDataSummary.weaponBayDoorsOpen = WeaponBayDoorsAreOpen();
   mWeaponsDataSummary.supportingWeapon   = SupportingWeapon();
   mWeaponsDataSummary.masterWarning      = MasterWarningActive();
   mWeaponsDataSummary.masterCaution      = MasterCautionActive();
   mWeaponsDataSummary.shootCueActive     = ShootCueActive();
   mWeaponsDataSummary.shootCueBlink      = ShootCueBlinking();
   mWeaponsDataSummary.masterArmActive    = GetMasterArm();
   mWeaponsDataSummary.jammingDetected    = JammingDetected();

   // Update weapon support
   mAssessPtr->UpdateWeaponSupport(aSimTime);
}

void WsfSA_Processor::UpdateTrackData(double aSimTime)
{
   // Clear the current track managers and data
   mTrackManagerDataList.clear();

   // Loop through all of the track processors
   for (auto& trackProcessor : mTrackProcessorList)
   {
      // This is our temporary track manager data
      SA_TrackManagerData tmpTrackManager;

      // Set the track manager's name
      tmpTrackManager.name = trackProcessor->GetName();

      // Check for master processor
      bool isMasterTrackProcessor = trackProcessor->IsMasterTrackManager();
      if (isMasterTrackProcessor)
      {
         tmpTrackManager.isMasterProcessor = true;
      }
      else
      {
         tmpTrackManager.isMasterProcessor = false;
      }

      // Now, we need to load tracks
      WsfTrackManager&   trackMgr  = trackProcessor->GetTrackManager();
      WsfLocalTrackList& trkList   = trackMgr.GetLocalTrackList();
      unsigned int       numTracks = trkList.GetTrackCount();
      for (unsigned int i = 0; i < numTracks; ++i)
      {
         // Track must be valid (non-null track pointer)
         const WsfLocalTrack* trk = trkList.GetTrackEntry(i);
         if (trk != nullptr)
         {
            size_t targetIndex = trk->GetTargetIndex();

            // Target platform must be valid (non-null) or it must be a false target
            const WsfPlatform* tgtPlatform = GetSimulation()->GetPlatformByIndex(targetIndex);
            if ((tgtPlatform != nullptr) || (trk->IsFalseTarget()))
            {
               // Must not be the current (ownship) platform -- this filters-out ownship
               if (GetPlatform()->GetIndex() != targetIndex)
               {
                  // Filtering assets -- Assets should be filtered if mFilterAssetsFromTracks is true,
                  // but only for master track processor(s).
                  bool filterOutAssets = false;
                  if (isMasterTrackProcessor)
                  {
                     if (mFilterAssetsFromTracks && EntityIsInAssetGroup(targetIndex))
                     {
                        filterOutAssets = true;
                     }
                  }

                  // If using mFilterAssetsFromTracks, must not be an asset to proceed -- this may filter friends
                  if (!filterOutAssets)
                  {
                     // This is the current track for which we will add data
                     SA_TrackEntryData targetTrack;

                     targetTrack.targetPlatformIndex = static_cast<unsigned int>(targetIndex);
                     targetTrack.tgtPlatformName     = trk->GetTargetName();
                     targetTrack.tgtTypeName         = trk->GetTargetType();

                     // Set default missile to false, this may become true later/below
                     targetTrack.isMissile = false;

                     if (trk->LocationValid())
                     {
                        double lat, lon, alt;
                        trk->GetLocationLLA(lat, lon, alt);
                        targetTrack.lat_deg       = lat;
                        targetTrack.lon_deg       = lon;
                        targetTrack.altitude_ft   = static_cast<float>(alt * UtMath::cFT_PER_M);
                        targetTrack.latLonValid   = true;
                        targetTrack.altitudeValid = true;
                     }
                     else
                     {
                        targetTrack.latLonValid   = false;
                        targetTrack.altitudeValid = false;
                     }

                     if (trk->BearingValid())
                     {
                        targetTrack.bearing_deg  = static_cast<float>(trk->GetBearing() * UtMath::cDEG_PER_RAD);
                        targetTrack.bearingValid = true;
                     }
                     else
                     {
                        targetTrack.bearingValid = false;
                     }

                     if (trk->ElevationValid())
                     {
                        targetTrack.elevation_deg  = static_cast<float>(trk->GetElevation() * UtMath::cDEG_PER_RAD);
                        targetTrack.elevationValid = true;
                     }
                     else
                     {
                        targetTrack.elevationValid = false;
                     }

                     if (trk->RangeValid())
                     {
                        targetTrack.range_nm   = static_cast<float>(trk->GetRange() * UtMath::cNM_PER_M);
                        targetTrack.rangeValid = true;
                     }
                     else
                     {
                        targetTrack.rangeValid = false;
                     }

                     if (trk->RangeRateValid())
                     {
                        targetTrack.rangeRate_kts = static_cast<float>(trk->GetRangeRate() * UtMath::cNM_PER_M * 3600.0);
                        targetTrack.rangeRateValid = true;
                     }
                     else
                     {
                        targetTrack.rangeRateValid = false;
                     }

                     // Identification
                     bool idAssigned = false;

                     // Default id is bogie
                     targetTrack.identification = ID_BOGIE;

                     if (trk->TypeIdValid())
                     {
                        std::string trackType = trk->GetTypeId().GetString();

                        // Check for simple mode
                        if (UseSimpleIdByType())
                        {
                           // This provides a simulated, simple identification by type. Basically, when
                           // using the "simple" approach, we confirm that the track supports type ID,
                           // and then we check the target platform truth "side", even if the side data
                           // is not necessarily provided in the track.

                           if (tgtPlatform != nullptr)
                           {
                              std::string truthSide = tgtPlatform->GetSide();

                              if (IsEnemySide(truthSide))
                              {
                                 targetTrack.idTypeValid    = true;
                                 targetTrack.idString       = trk->GetTypeId().GetString();
                                 targetTrack.identification = ID_BANDIT;
                                 idAssigned                 = true;
                              }
                              else if (IsFriendlySide(truthSide))
                              {
                                 targetTrack.idTypeValid    = true;
                                 targetTrack.idString       = trk->GetTypeId().GetString();
                                 targetTrack.identification = ID_FRIENDLY;
                                 idAssigned                 = true;
                              }
                              else if (IsNeutralSide(truthSide))
                              {
                                 targetTrack.idTypeValid    = true;
                                 targetTrack.idString       = trk->GetTypeId().GetString();
                                 targetTrack.identification = ID_NEUTRAL;
                                 idAssigned                 = true;
                              }
                           }
                        }
                        else
                        {
                           if (IsEnemyType(trackType))
                           {
                              targetTrack.idTypeValid    = true;
                              targetTrack.idString       = trk->GetTypeId().GetString();
                              targetTrack.identification = ID_BANDIT;
                              idAssigned                 = true;
                           }
                           else if (IsFriendlyType(trackType))
                           {
                              targetTrack.idTypeValid    = true;
                              targetTrack.idString       = trk->GetTypeId().GetString();
                              targetTrack.identification = ID_FRIENDLY;
                              idAssigned                 = true;
                           }
                           else if (IsNeutralType(trackType))
                           {
                              targetTrack.idTypeValid    = true;
                              targetTrack.idString       = trk->GetTypeId().GetString();
                              targetTrack.identification = ID_NEUTRAL;
                              idAssigned                 = true;
                           }
                        }

                        // Check for missile. Note -- If the list of missile types is not populated
                        // (possibly because the simple type ID is being used), we will not be able
                        // to identify missiles by type.
                        if (IsMissileType(trackType))
                        {
                           targetTrack.isMissile = true;
                        }
                     }

                     // ID: side
                     if (!idAssigned && trk->SideIdValid())
                     {
                        if (IsEnemySide(trk->GetSideId().GetString()))
                        {
                           targetTrack.idSideValid    = true;
                           targetTrack.idString       = "SIDE-ENEMY";
                           targetTrack.identification = ID_BANDIT;
                           idAssigned                 = true;
                        }
                        else if (IsFriendlySide(trk->GetSideId().GetString()))
                        {
                           targetTrack.idSideValid    = true;
                           targetTrack.idString       = "SIDE-FRIEND";
                           targetTrack.identification = ID_FRIENDLY;
                           idAssigned                 = true;
                        }
                        else if (IsNeutralSide(trk->GetSideId().GetString()))
                        {
                           targetTrack.idSideValid    = true;
                           targetTrack.idString       = "SIDE-NEUTRAL";
                           targetTrack.identification = ID_NEUTRAL;
                           idAssigned                 = true;
                        }
                     }

                     // ID: IFF
                     if (!idAssigned && UseIFF() && trk->IFF_StatusValid())
                     {
                        if (trk->GetIFF_Status() == WsfTrack::cIFF_FOE)
                        {
                           targetTrack.idIffValid     = true;
                           targetTrack.idString       = "IFF-FOE";
                           targetTrack.identification = ID_BANDIT;
                           idAssigned                 = true;
                        }
                        else if (trk->GetIFF_Status() == WsfTrack::cIFF_FRIEND)
                        {
                           targetTrack.idIffValid     = true;
                           targetTrack.idString       = "IFF-FRIEND";
                           targetTrack.identification = ID_FRIENDLY;
                           idAssigned                 = true;
                        }
                        else if (trk->GetIFF_Status() == WsfTrack::cIFF_NEUTRAL)
                        {
                           targetTrack.idIffValid     = true;
                           targetTrack.idString       = "IFF-NEUTRAL";
                           targetTrack.identification = ID_NEUTRAL;
                           idAssigned                 = true;
                        }
                     }

                     // ID: Aux Data
                     if (!idAssigned && trk->HasAuxData())
                     {
                        const WsfAttributeContainer& auxData = trk->GetAuxData();

                        if (auxData.AttributeExists("id_aux_type"))
                        {
                           std::string typeString = auxData.GetString("id_aux_type");
                           targetTrack.idAuxValid = true;
                           if (IsEnemyType(typeString))
                           {
                              targetTrack.idString       = "ID-AUX-" + typeString;
                              targetTrack.identification = ID_BANDIT;
                              idAssigned                 = true;
                           }
                           else if (IsFriendlyType(typeString))
                           {
                              targetTrack.idString       = "ID-AUX-" + typeString;
                              targetTrack.identification = ID_FRIENDLY;
                              idAssigned                 = true;
                           }

                           if (IsMissileType(typeString))
                           {
                              targetTrack.isMissile = true;
                           }
                        }

                        if (!idAssigned)
                        {
                           if (auxData.AttributeExists("id_aux_friend"))
                           {
                              targetTrack.idAuxValid     = true;
                              targetTrack.idString       = "ID-AUX-FRIEND";
                              targetTrack.identification = ID_FRIENDLY;
                              idAssigned                 = true;
                           }
                           else if (auxData.AttributeExists("id_aux_foe"))
                           {
                              targetTrack.idAuxValid     = true;
                              targetTrack.idString       = "ID-AUX-FOE";
                              targetTrack.identification = ID_BANDIT;
                              idAssigned                 = true;
                           }
                           else if (auxData.AttributeExists("id_aux_neutral"))
                           {
                              targetTrack.idAuxValid     = true;
                              targetTrack.idString       = "ID-AUX-NEUTRAL";
                              targetTrack.identification = ID_NEUTRAL;
                              idAssigned                 = true;
                           }
                        }
                     }

                     // Assume target track data if Bearing/Elevation/Range or Lat/Lon/Altitude is available
                     if ((targetTrack.bearingValid && targetTrack.elevationValid && targetTrack.rangeValid) ||
                         (targetTrack.latLonValid && targetTrack.altitudeValid))
                     {
                        if (tgtPlatform != nullptr)
                        {
                           float speed_kts = static_cast<float>(tgtPlatform->GetSpeed() * UtMath::cNM_PER_M * 3600.0);

                           double heading_rad, pitch_rad, roll_rad;
                           tgtPlatform->GetOrientationNED(heading_rad, pitch_rad, roll_rad);
                           float heading_deg = static_cast<float>(heading_rad * UtMath::cDEG_PER_RAD);

                           float aspectAngle_deg = static_cast<float>(
                              WsfSA_Assess::AspectAngleForTarget2D(*GetPlatform(), *tgtPlatform) * UtMath::cDEG_PER_RAD);

                           targetTrack.targetSpeed_kts   = speed_kts;
                           targetTrack.targetHdg_deg     = heading_deg;
                           targetTrack.targetAspect_deg  = aspectAngle_deg;
                           targetTrack.targetSpeedValid  = true;
                           targetTrack.targetHdgValid    = true;
                           targetTrack.targetAspectValid = true;
                        }
                        else
                        {
                           targetTrack.targetSpeed_kts   = 0.0f;
                           targetTrack.targetHdg_deg     = 0.0f;
                           targetTrack.targetAspect_deg  = 0.0f;
                           targetTrack.targetSpeedValid  = false;
                           targetTrack.targetHdgValid    = false;
                           targetTrack.targetAspectValid = false;
                        }
                     }
                     else
                     {
                        targetTrack.targetSpeed_kts   = 0.0f;
                        targetTrack.targetHdg_deg     = 0.0f;
                        targetTrack.targetAspect_deg  = 0.0f;
                        targetTrack.targetSpeedValid  = false;
                        targetTrack.targetHdgValid    = false;
                        targetTrack.targetAspectValid = false;
                     }

                     // If the track is not perceived as a missile, check speed/alt parameters
                     if (!targetTrack.isMissile)
                     {
                        // Check for additional means of identifying a missile threat
                        bool possibleMissile = false;

                        // First, try to determine the speed of the track
                        bool   speedAvailable = false;
                        double speed_mps      = 0.0;
                        if (targetTrack.targetSpeedValid)
                        {
                           speedAvailable = true;
                           speed_mps      = targetTrack.targetSpeed_kts * UtMath::cMPS_PER_NMPH;
                        }
                        else if (targetTrack.rangeRateValid)
                        {
                           speedAvailable = true;
                           // Calc speed using range rate for speed approximation
                           speed_mps = std::abs(targetTrack.rangeRate_kts) * UtMath::cMPS_PER_NMPH;
                        }

                        // Next, try to determine the altitude of the track
                        bool   altAvailable = false;
                        double alt_m        = 0.0;
                        if (targetTrack.altitudeValid || (targetTrack.elevationValid && targetTrack.rangeValid))
                        {
                           altAvailable = true;
                           alt_m        = tgtPlatform->GetAltitude();
                        }

                        // Check for speed any alt
                        if (mMissileSpeedAnyAlt && speedAvailable)
                        {
                           if (speed_mps > mMissileSpeedAnyAltValue)
                           {
                              possibleMissile = true;
                           }
                        }

                        // Check for alt any speed
                        if (mMissileAltAnySpeed)
                        {
                           if (alt_m > mMissileAltAnySpeedValue)
                           {
                              possibleMissile = true;
                           }
                        }

                        // Check for speed and alt
                        if (mMissileSpeedWithAlt && mMissileAltWithSpeed && speedAvailable && altAvailable)
                        {
                           if ((speed_mps > mMissileSpeedWithAltValue) && (alt_m > mMissileAltWithSpeedValue))
                           {
                              possibleMissile = true;
                           }
                        }

                        // Check if a missile is possible
                        if (possibleMissile)
                        {
                           // We have a potential missile, so now check the filter-out options

                           // Check for angle off nose
                           if (mMissileNoseAngle)
                           {
                              if (targetTrack.latLonValid && targetTrack.targetHdgValid)
                              {
                                 double angle_rad = mAssessPtr->OffBoresightAngleForTarget(*tgtPlatform, *mPlatform);
                                 if (angle_rad > mMissileNoseAngleValue)
                                 {
                                    // Filter-out this possible missile, based on angle off nose
                                    possibleMissile = false;
                                 }
                              }
                           }

                           // Check for distance
                           if (possibleMissile && mMissileDistance)
                           {
                              if (targetTrack.latLonValid)
                              {
                                 if (tgtPlatform != nullptr)
                                 {
                                    auto   locTargetWCS   = tgtPlatform->GetLocationWCS();
                                    auto   locPlatformWCS = mPlatform->GetLocationWCS();
                                    double dist_m         = locTargetWCS.Subtract(locPlatformWCS).Magnitude();
                                    if (dist_m > mMissileDistanceValue)
                                    {
                                       // Filter-out this possible missile, based on distance
                                       possibleMissile = false;
                                    }
                                 }
                              }
                              else if (targetTrack.rangeValid)
                              {
                                 if (targetTrack.range_nm > mMissileDistanceValue)
                                 {
                                    // Filter-out this possible missile, based on distance
                                    possibleMissile = false;
                                 }
                              }
                           }

                           // Check for time to intercept
                           if (possibleMissile && mMissileTimeToIntercept)
                           {
                              if (targetTrack.targetSpeedValid &&
                                  (targetTrack.latLonValid || (targetTrack.bearing_deg && targetTrack.rangeValid)))
                              {
                                 // Get the missile speed, based on track data
                                 speed_mps = targetTrack.targetSpeed_kts * UtMath::cMPS_PER_NMPH;

                                 // Get the missile location in WCS
                                 double missileLocWCS[3];
                                 tgtPlatform->GetLocationWCS(missileLocWCS);

                                 // Get the platform location in WCS
                                 double platLocWCS[3];
                                 mPlatform->GetLocationWCS(platLocWCS);

                                 // Get platform velocity in WCS
                                 double platVelWCS[3];
                                 mPlatform->GetVelocityWCS(platVelWCS);

                                 double interceptLocWCS[3];
                                 double timeToIntercept_sec =
                                    WsfIntercept::Intercept(missileLocWCS, speed_mps, platLocWCS, platVelWCS, interceptLocWCS);

                                 if (timeToIntercept_sec > mMissileTimeToInterceptValue)
                                 {
                                    // Filter-out this possible missile, based on time to intercept
                                    possibleMissile = false;
                                 }
                              }
                           }
                        }

                        // If we still have a possible missile, so set the missile flag
                        if (possibleMissile)
                        {
                           targetTrack.isMissile = true;
                        }
                     }

                     // If the track is still not perceived as a missile, check the MWS track processor.
                     // Any track reported by the MWS is assumed to be a missile.
                     if (!targetTrack.isMissile && (tgtPlatform != nullptr))
                     {
                        if (PlatformInTrackListMWS(tgtPlatform))
                        {
                           targetTrack.isMissile = true;
                        }
                     }

                     // Only add the track if *some* geometric parameters are valid
                     if (targetTrack.latLonValid || targetTrack.altitudeValid || targetTrack.bearingValid ||
                         targetTrack.elevationValid || targetTrack.rangeValid)
                     {
                        // This is a "valid" track, so add it the track to the track list
                        tmpTrackManager.trackList.push_back(targetTrack);
                     }
                  }
               }
            }
         }
      }

      // Add this track manager to the list
      mTrackManagerDataList.push_back(tmpTrackManager);
   }
}

void WsfSA_Processor::UpdateBehaviorCalculation(double aSimTime)
{
   CalculateEmissions();
   CalculateObservables();
   mUpdatedScriptContext = true;
   mContextPtr->Update(aSimTime);
   if (mBehaviorTreePtr != nullptr)
   {
      mBehaviorTreePtr->Execute(aSimTime);
   }
   if (mAdvancedBehaviorTreePtr != nullptr)
   {
      mAdvancedBehaviorTreePtr->Tick(aSimTime);
   }
   if (mStateMachinePtr != nullptr)
   {
      mStateIndex = mStateMachinePtr->EvaluateState(mStateIndex);
   }
}

void WsfSA_Processor::CalculateObservables()
{
   UtAtmosphere& atm      = GetSimulation()->GetAtmosphere();
   WsfPlatform*  platform = GetPlatform();
   if (platform != nullptr)
   {
      mAircraftKinematicsData.contrailing = atm.IsWithinContrailAltitudeBand(platform->GetAltitude());
   }
   else
   {
      mAircraftKinematicsData.contrailing = false;
   }

   // P6DOF and SixDOF movers support afterburner
   if (mP6DofMover != nullptr)
   {
      mAircraftKinematicsData.afterburnerOn = mP6DofMover->GetAfterburnerOn();
   }
   else if (mSixDOF_Mover != nullptr)
   {
      mAircraftKinematicsData.afterburnerOn = mSixDOF_Mover->GetAfterburnerOn();
   }
   else if (mBrawlerMover != nullptr)
   {
      // Note: Brawler uses the following throttle values: 1 = idle, 2 = mil, 3 = ab
      if (mBrawlerMover->GetThrottlePosition() > 2.0)
      {
         mAircraftKinematicsData.afterburnerOn = true;
      }
      else
      {
         mAircraftKinematicsData.afterburnerOn = false;
      }
   }
   else
   {
      // Other movers, including AirMovers
      mAircraftKinematicsData.afterburnerOn = false;
   }
}

void WsfSA_Processor::CalculateEmissions()
{
   mRadarEmitting     = false;
   unsigned int count = GetPlatform()->GetComponentCount<WsfSensor>();
   for (unsigned int i = 0; i < count; ++i)
   {
      WsfSensor* sensorPtr = GetPlatform()->GetComponentEntry<WsfSensor>(i);

      // Check for actual radar
      WsfRadarSensor* radar = static_cast<WsfRadarSensor*>(sensorPtr);
      if (radar != nullptr)
      {
         if (radar->IsTurnedOn())
         {
            mRadarEmitting = true;
         }
      }

      // Check radar category
      if (sensorPtr->IsCategoryMember("radar"))
      {
         if (sensorPtr->IsTurnedOn())
         {
            mRadarEmitting = true;
         }
      }
   }

   mJammerEmitting = false;
   count           = GetPlatform()->GetComponentCount<WsfWeapon>();
   for (unsigned int i = 0; i < count; ++i)
   {
      WsfWeapon* weaponPtr = GetPlatform()->GetComponentEntry<WsfWeapon>(i);
      if (weaponPtr->IsCategoryMember("jammer"))
      {
         if (weaponPtr->IsTurnedOn())
         {
            mJammerEmitting = true;
         }
      }
   }

   // TODO -- Need to determine how to handle other systems
   mOtherSystemsEmitting = false;
}

bool WsfSA_Processor::FuelStateBingoReached() const
{
   double availableFuel_lbs = mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs;
   if (availableFuel_lbs > mFuelSystemData.bingo_lbs)
   {
      return false;
   }

   return true;
}

bool WsfSA_Processor::FuelStateJokerReached() const
{
   double availableFuel_lbs = mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs;
   if (availableFuel_lbs > mFuelSystemData.joker_lbs)
   {
      return false;
   }

   return true;
}

double WsfSA_Processor::FuelStateNormalized() const
{
   return mFuelSystemData.normalizedFuelQty;
}

double WsfSA_Processor::TimeToJoker() const
{
   double availableFuel_lbs =
      (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) - mFuelSystemData.joker_lbs;
   double time_sec = (availableFuel_lbs / mFuelSystemData.fuelFlow_pph) * 3600.0;

   return time_sec;
}

double WsfSA_Processor::TimeToBingo() const
{
   double availableFuel_lbs =
      (mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs) - mFuelSystemData.bingo_lbs;
   double time_sec = (availableFuel_lbs / mFuelSystemData.fuelFlow_pph) * 3600.0;

   return time_sec;
}

double WsfSA_Processor::TimeToEmpty() const
{
   double availableFuel_lbs = mFuelSystemData.fuelInternal_lbs + mFuelSystemData.fuelExternal_lbs;
   double time_sec          = (availableFuel_lbs / mFuelSystemData.fuelFlow_pph) * 3600.0;

   return time_sec;
}

bool WsfSA_Processor::SetUpdateInterval(UpdateType aUpdateType, double aUpdateInterval)
{
   if (aUpdateInterval < 0.0)
   {
      return false; // invalid parameter
   }
   else if (aUpdateInterval == 0.0)
   {
      // User is disabling this update type
      mUpdateMgmtData[aUpdateType].activeInterval = aUpdateInterval;
      return true;
   }

   double updateInterval        = WsfProcessor::GetUpdateInterval();
   double roundedUpdateInterval = RoundToBaseRateMultiple(aUpdateInterval, updateInterval);
   roundedUpdateInterval        = std::max(roundedUpdateInterval, updateInterval); // don't allow rounding to zero

   if (abs(roundedUpdateInterval - aUpdateInterval) > (updateInterval / 100.0))
   {
      std::string typeString = GetUpdateTypeString(aUpdateType);
      ut::log::warning() << "WSF_SA_PROCESSOR update interval (" << typeString << ") " << aUpdateInterval;
      ut::log::warning() << " was rounded to " << roundedUpdateInterval;
   }

   // Save setting and adjust schedule
   mUpdateMgmtData[aUpdateType].activeInterval = roundedUpdateInterval;
   mUpdateMgmtData[aUpdateType].nextUpdateTime =
      mUpdateMgmtData[aUpdateType].lastUpdateTime + mUpdateMgmtData[aUpdateType].activeInterval;
   return true;
}

void WsfSA_Processor::ResetUpdateInterval(UpdateType aUpdateType)
{
   // Restore startup setting and adjust schedule (even if startup setting is 0 and disables update)
   mUpdateMgmtData[aUpdateType].activeInterval = mUpdateMgmtData[aUpdateType].startupInterval;
   mUpdateMgmtData[aUpdateType].nextUpdateTime =
      mUpdateMgmtData[aUpdateType].lastUpdateTime + mUpdateMgmtData[aUpdateType].activeInterval;
}

bool WsfSA_Processor::SetLastUpdateTime(UpdateType aUpdateType, double aSimTime)
{
   if (aSimTime < 0.0)
   {
      return false; // invalid parameter
   }
   mUpdateMgmtData[aUpdateType].lastUpdateTime = aSimTime;
   return true;
}

bool WsfSA_Processor::SetNextUpdateTime(UpdateType aUpdateType, double aSimTime)
{
   if (aSimTime < 0.0)
   {
      return false; // invalid parameter
   }
   mUpdateMgmtData[aUpdateType].nextUpdateTime = aSimTime;
   return true;
}

std::string WsfSA_Processor::GetUpdateTypeString(const UpdateType aUpdateType)
{
   std::string result;

   switch (aUpdateType)
   {
   case UpdateType::cSTATUS:
      result = "report_interval";
      break;
   case UpdateType::cENGAGEMENT_DATA:
      result = "engagement_data_update_interval";
      break;
   case UpdateType::cFLIGHT_DATA:
      result = "flight_data_update_interval";
      break;
   case UpdateType::cFUEL_DATA:
      result = "fuel_data_update_interval";
      break;
   case UpdateType::cNAV_DATA:
      result = "nav_data_update_interval";
      break;
   case UpdateType::cCONTROLS_DATA:
      result = "flight_controls_data_update_interval";
      break;
   case UpdateType::cWEAPONS_DATA:
      result = "weapons_data_update_interval";
      break;
   case UpdateType::cTRACK_DATA:
      result = "track_data_update_interval";
      break;
   case UpdateType::cASSETS_DATA:
      result = "asset_data_update_interval";
      break;
   case UpdateType::cPERCEIVED_ITEM_DATA:
      result = "perceived_item_data_update_interval";
      break;
   case UpdateType::cPRIORITIZED_ITEM_DATA:
      result = "prioritized_item_data_update_interval";
      break;
   case UpdateType::cPERCEIVED_ITEM_CALCULATION:
      result = "perceived_item_calculation_update_interval";
      break;
   case UpdateType::cPRIORITIZED_ITEM_CALCULATION:
      result = "prioritized_item_calculation_update_interval";
      break;
   case UpdateType::cBEHAVIOR_CALCULATION:
      result = "behavior_calculation_update_interval";
      break;
   default:
      result = "UNKNOWN";
      break;
   }

   return result;
}

bool WsfSA_Processor::SetAssetDataPurgeLifetime(double aPurgeLifetime)
{
   if (aPurgeLifetime < 0.0)
   {
      return false; // invalid parameter
   }
   mAssetDataPurgeLifetime = aPurgeLifetime;
   return true;
}

bool WsfSA_Processor::SetVisualPerceptionDelay(double aDelayTime)
{
   if (aDelayTime < 0.0)
   {
      return false; // invalid parameter
   }

   mVisualPerceptionDelay = aDelayTime;
   return true;
}

bool WsfSA_Processor::SetDisplayPerceptionDelay(double aDelayTime)
{
   if (aDelayTime < 0.0)
   {
      return false; // invalid parameter
   }

   mDisplayPerceptionDelay = aDelayTime;
   return true;
}

WsfTrackProcessor* WsfSA_Processor::GetTrackProcessor(const std::string& aProcessorName)
{
   for (auto& it : mTrackProcessorList)
   {
      if (it->GetName() == aProcessorName)
      {
         return it;
      }
   }

   return nullptr;
}

double WsfSA_Processor::RoundToBaseRateMultiple(double aNumToRound, double aMultiple)
{
   if (aNumToRound <= 0.0)
   {
      return 0.0;
   }
   if (aMultiple <= 0.0)
   {
      return aNumToRound;
   }

   return trunc((aNumToRound + (aMultiple / 2)) / aMultiple) * aMultiple;
}

void WsfSA_Processor::HandleSA_ProcessorUpdates(double aSimTime)
{
   double updateTime = 0.0;

   //! There is a particular order in which these updates are checked/performed (data pipeline)
   //! The update intervals are checked at the base interval rate and each update type repeats at a multiple of the base
   //! interval. In order for a given type of update to be performed, there are two conditions:
   //!   1. Its update interval must be set to a positive number (otherwise, that update type is disabled)
   //!   2. Its next scheduled time is less than aSimTime

   // Note that we update the perceived items calculations first, followed by the priority items calculation, and then
   // update the "data" types
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION) <= aSimTime)
   {
      updateTime = mPerceivePtr->TryUpdatePerceivedItemCalculation(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_CALCULATION, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_CALCULATION) <= aSimTime)
   {
      updateTime = mAssessPtr->TryUpdatePrioritizedItemCalculation(aSimTime);
      SetNextUpdateTime(UpdateType::cPRIORITIZED_ITEM_CALCULATION, updateTime);
      SetLastUpdateTime(UpdateType::cPRIORITIZED_ITEM_CALCULATION, aSimTime);
   }
   if ((GetUpdateInterval(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION) > 0 &&
        GetNextUpdateTime(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION) <= aSimTime) ||
       GetUpdateInterval(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION) == 0)
   {
      updateTime = TryUpdateBehaviorCalculation(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cBEHAVIOR_CALCULATION, aSimTime);
   }

   // Now, we update the data types, after the calculations are complete

   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cFLIGHT_DATA) > 0 && HasExternalLinks() &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cFLIGHT_DATA) <= aSimTime)
   {
      updateTime = TryUpdateFlightKinematics(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cFLIGHT_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cFLIGHT_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cCONTROLS_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cCONTROLS_DATA) <= aSimTime)
   {
      updateTime = TryUpdateFlightControls(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cCONTROLS_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cCONTROLS_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cWEAPONS_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cWEAPONS_DATA) <= aSimTime)
   {
      updateTime = TryUpdateWeapons(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cWEAPONS_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cWEAPONS_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cFUEL_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cFUEL_DATA) <= aSimTime)
   {
      updateTime = TryUpdateFuel(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cFUEL_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cFUEL_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cNAV_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cNAV_DATA) <= aSimTime)
   {
      updateTime = TryUpdateNav(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cNAV_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cNAV_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cSTATUS) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cSTATUS) <= aSimTime)
   {
      updateTime = mPerceivePtr->TryUpdateStatus(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cSTATUS, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cSTATUS, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cASSETS_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA) <= aSimTime)
   {
      updateTime = mPerceivePtr->TryUpdateAssets(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cASSETS_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cTRACK_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cTRACK_DATA) <= aSimTime)
   {
      updateTime = TryUpdateTracks(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cTRACK_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cTRACK_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA) <= aSimTime)
   {
      updateTime = mPerceivePtr->TryUpdatePerceivedItemData(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cPERCEIVED_ITEM_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA) <= aSimTime)
   {
      updateTime = mAssessPtr->TryUpdateEngagements(aSimTime);
      SetNextUpdateTime(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA, updateTime);
      SetLastUpdateTime(WsfSA_Processor::UpdateType::cENGAGEMENT_DATA, aSimTime);
   }
   if (GetUpdateInterval(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA) > 0 &&
       GetNextUpdateTime(WsfSA_Processor::UpdateType::cPRIORITIZED_ITEM_DATA) <= aSimTime)
   {
      updateTime = mAssessPtr->TryUpdatePrioritizedItemData(aSimTime);
      SetNextUpdateTime(UpdateType::cPRIORITIZED_ITEM_DATA, updateTime);
      SetLastUpdateTime(UpdateType::cPRIORITIZED_ITEM_DATA, aSimTime);
   }
}

void WsfSA_Processor::DebugTestOutput(double aSimTime)
{
   if (GetPlatform()->GetName() == mDebugPlatformName)
   {
      auto out = ut::log::info() << "----------------------------------------------------------------------------";
      ut::log::info() << mDebugPlatformName << " SA Processor Assess::Update at T = " << aSimTime << " sec";

      size_t numAssets = mData.mPerceivedAssets.size();
      if (numAssets > 0)
      {
         out.AddNote() << "Perceived Assets (" << numAssets << ") :";

         for (auto& item : mData.mPerceivedAssets)
         {
            std::string name;
            size_t      index = item->GetEntityPlatformIndex();
            if (index > 0)
            {
               WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(index);
               if (platform != nullptr)
               {
                  name = platform->GetName();
               }
            }

            std::string idString;
            switch (item->GetIdentification())
            {
            case WsfSA_Processor::SA_Identification::ID_FRIENDLY:
               idString = "Friendly";
               break;
            case WsfSA_Processor::SA_Identification::ID_NEUTRAL:
               idString = "Neutral ";
               break;
            case WsfSA_Processor::SA_Identification::ID_BOGIE:
               idString = "Bogie   ";
               break;
            case WsfSA_Processor::SA_Identification::ID_BANDIT:
               idString = "Bandit  ";
               break;
            default:
               idString = "Unknown ";
            }

            std::string mslString = "A/C";
            if (item->GetIsMissile())
            {
               mslString = "MSL";
            }

            if (FlightId() == item->GetFlightId())
            {
               if (name.empty())
               {
                  out.AddNote() << "  Asset:  FLIGHT " << idString << " " << mslString << " "
                                << " ????";
               }
               else
               {
                  out.AddNote() << "  Asset:  FLIGHT " << idString << " " << mslString << " " << name;
               }
            }
            else
            {
               if (name.empty())
               {
                  out.AddNote() << "  Asset:  OTHER  " << idString << " " << mslString << " "
                                << " ????";
               }
               else
               {
                  out.AddNote() << "  Asset:  OTHER  " << idString << " " << mslString << " " << name;
               }
            }
         }
      }
      else
      {
         out.AddNote() << "Perceived Assets:  None";
      }

      size_t numBogies = mData.mPerceivedBogies.size();
      if (numBogies > 0)
      {
         out.AddNote() << "Perceived Bogies (" << numBogies << ") :";

         for (auto& item : mData.mPerceivedBogies)
         {
            std::string name;
            size_t      index = item->GetPerceivedIndex();
            if (index > 0)
            {
               WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(index);
               if (platform != nullptr)
               {
                  name = platform->GetName();

                  std::string idString;
                  switch (item->GetIdentification())
                  {
                  case WsfSA_Processor::SA_Identification::ID_FRIENDLY:
                     idString = "Friendly";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_NEUTRAL:
                     idString = "Neutral ";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_BOGIE:
                     idString = "Bogie   ";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_BANDIT:
                     idString = "Bandit  ";
                     break;
                  default:
                     idString = "Unknown ";
                  }

                  std::string mslString = "A/C";
                  if (item->GetIsMissile())
                  {
                     mslString = "MSL";
                  }

                  if (name.empty())
                  {
                     out.AddNote() << "  Bogie :  " << idString << " " << mslString << " (NO NAME)";
                  }
                  else
                  {
                     out.AddNote() << "  Bogie :  " << idString << " " << mslString << " " << name;
                  }
               }
            }
         }
      }
      else
      {
         out.AddNote() << "Perceived Bogies:  None:";
      }

      size_t numBandits = mData.mPerceivedBandits.size();
      if (numBandits > 0)
      {
         out.AddNote() << "Perceived Bandits (" << numBandits << ") :";

         for (auto& item : mData.mPerceivedBandits)
         {
            std::string name;
            size_t      index = item->GetPerceivedIndex();
            if (index > 0)
            {
               WsfPlatform* platform = GetSimulation()->GetPlatformByIndex(index);
               if (platform != nullptr)
               {
                  name = platform->GetName();

                  std::string idString;
                  switch (item->GetIdentification())
                  {
                  case WsfSA_Processor::SA_Identification::ID_FRIENDLY:
                     idString = "Friendly";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_NEUTRAL:
                     idString = "Neutral ";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_BOGIE:
                     idString = "Bogie   ";
                     break;
                  case WsfSA_Processor::SA_Identification::ID_BANDIT:
                     idString = "Bandit  ";
                     break;
                  default:
                     idString = "Unknown ";
                  }

                  std::string mslString = "A/C";
                  if (item->GetIsMissile())
                  {
                     mslString = "MSL";
                  }

                  if (name.empty())
                  {
                     out.AddNote() << "  Bandit:  " << idString << " " << mslString << " (NO NAME)";
                  }
                  else
                  {
                     out.AddNote() << "  Bandit:  " << idString << " " << mslString << " " << name;
                  }
               }
            }
         }
      }
      else
      {
         out.AddNote() << "Perceived Bandits:  None";
      }

      if (Assess()->CurrentlyTrackedPlatformNames().size() > 0)
      {
         out.AddNote() << "Tracked Platform Names (" << Assess()->CurrentlyTrackedPlatformNames().size() << "), including:";
         for (auto& tmpItem : Assess()->CurrentlyTrackedPlatformNames())
         {
            out.AddNote() << "  Track: " << tmpItem;
         }
      }
      else
      {
         out.AddNote() << "Tracked platforms (0):  NONE";
      }

      out.AddNote() << "Track Processors:";
      const std::vector<WsfSA_Processor::SA_TrackManagerData>& trackManagerList = GetTrackManagerList();
      for (auto& trackProcessor : trackManagerList)
      {
         if (trackProcessor.isMasterProcessor)
         {
            out.AddNote() << "  Master Track Processor(MTP) : " << trackProcessor.name;
         }
         else
         {
            out.AddNote() << "  Track Processor: " << trackProcessor.name;
         }

         for (auto& track : trackProcessor.trackList)
         {
            std::string idString;
            switch (track.identification)
            {
            case WsfSA_Processor::SA_Identification::ID_FRIENDLY:
               idString = "Friendly";
               break;
            case WsfSA_Processor::SA_Identification::ID_NEUTRAL:
               idString = "Neutral ";
               break;
            case WsfSA_Processor::SA_Identification::ID_BOGIE:
               idString = "Bogie   ";
               break;
            case WsfSA_Processor::SA_Identification::ID_BANDIT:
               idString = "Bandit  ";
               break;
            default:
               idString = "Unknown ";
            }

            if (track.isMissile)
            {
               out.AddNote() << "    Track: " << track.tgtPlatformName << " " << idString << " (MSL) " << track.tgtTypeName;
            }
            else
            {
               out.AddNote() << "    Track: " << track.tgtPlatformName << " " << idString << " (A/C) " << track.tgtTypeName;
            }
         }
      }

      if (Assess()->PerceivedGroups().size() > 0)
      {
         out.AddNote() << "Group Data (" << Assess()->PerceivedGroups().size() << " groups are present)";
         int count = 0;
         for (auto& group : Assess()->PerceivedGroups())
         {
            ++count;
            out.AddNote() << "  Group (" << count << ") Name = " << group->GetPerceivedName();
            out.AddNote() << "    Radius: " << group->GetRadiusCurrent_nm() << " nm";
            out.AddNote() << "    Entities: ";
            for (auto& entity : group->GetElementList())
            {
               out.AddNote() << "      " << entity->GetPerceivedName();
               if (entity->GetAngleOnly())
               {
                  out.AddNote() << "        TRACK:  POOR (Angle-Only)";
               }
               else
               {
                  out.AddNote() << "        TRACK:  GOOD (Location)";
               }

               if (entity->GetIdentification() == WsfSA_EntityPerception::FRIENDLY)
               {
                  out.AddNote() << "        ID:  FRIEND";
               }
               else if (entity->GetIdentification() == WsfSA_EntityPerception::BOGIE)
               {
                  out.AddNote() << "        ID:  BOGIE";
               }
               else if (entity->GetIdentification() == WsfSA_EntityPerception::BANDIT)
               {
                  out.AddNote() << "        ID:  BANDIT";
               }
               else if (entity->GetIdentification() == WsfSA_EntityPerception::NEUTRAL)
               {
                  out.AddNote() << "        ID:  NEUTRAL";
               }
               else if (entity->GetIdentification() == WsfSA_EntityPerception::UNKNOWN)
               {
                  out.AddNote() << "        ID:  UNKNOWN";
               }
               else
               {
                  out.AddNote() << "        ID:  ???????";
               }
            }
         }
      }
      else
      {
         out.AddNote() << "Group Data (NONE)";
      }

      if (Assess()->PrioritizedThreatNames().size() > 0)
      {
         out.AddNote() << "Prioritized Threat Names (" << Assess()->PrioritizedThreatNames().size() << "), including:";
         for (auto& tmpItem : Assess()->PrioritizedThreatNames())
         {
            out.AddNote() << "  P-Threat: " << tmpItem;
         }
      }
      else
      {
         out.AddNote() << "Prioritized Threat Names (0):  NONE";
      }

      if (Assess()->PrioritizedTargetNames().size() > 0)
      {
         out.AddNote() << "Prioritized Target Names (" << Assess()->PrioritizedTargetNames().size() << "), including:";
         for (auto& tmpItem : Assess()->PrioritizedTargetNames())
         {
            out.AddNote() << "  P-Target: " << tmpItem;
         }
      }
      else
      {
         out.AddNote() << "Prioritized Target Names (0):  NONE";
      }

      out.AddNote() << "Mission Task: " << Assess()->MissionTask();

      out.AddNote() << "Risk Assessment:";
      out.AddNote() << "  Risk:          " << Assess()->Risk();
      out.AddNote() << "  Self-Risk:     " << Assess()->SelfRisk();
      out.AddNote() << "  Flight-Risk:   " << Assess()->FlightRisk();
      out.AddNote() << "  Package-Risk:  " << Assess()->PackageRisk();
      out.AddNote() << "  Mission-Risk:  " << Assess()->MissionRisk();

      out.AddNote() << "Overall Defensiveness: " << Assess()->Defensiveness();

      out.AddNote() << "Overall Urgency: " << Assess()->Urgency();

      out.AddNote() << "Weapon Summary: ";
      out.AddNote() << "  Selected Weapon: " << Assess()->GetSelectedWeapon();

      out.AddNote() << "  Available Weapons: ";
      for (auto& item : mData.mWeaponList)
      {
         out.AddNote() << "    " << static_cast<int>(item->GetQuantityRemaining()) << "  " << item->GetName();
      }

      out.AddNote() << "Fuel Summary: ";
      out.AddNote() << "  Normalized Fuel: " << FuelStateNormalized() * 100.0 << "%";
      out.AddNote() << "  Time to Joker: " << TimeToJoker() / 60.0 << " minutes";

      out.AddNote() << "  Time to Joker: " << TimeToJoker() / 60.0 << " minutes";

      if (FuelStateJokerReached())
      {
         out.AddNote() << "      JOKER REACHED";
      }
      else
      {
         out.AddNote() << "    Joker not reached";
      }

      if (FuelStateBingoReached())
      {
         out.AddNote() << "      BINGO REACHED";
      }
      else
      {
         out.AddNote() << "    Bingo not reached";
      }

      out.AddNote() << "Other Data: ";

      if (AfterburnerOn())
      {
         out.AddNote() << "  Afterburner: **ON**";
      }
      else
      {
         out.AddNote() << "  Afterburner:  Off";
      }
   }
}
