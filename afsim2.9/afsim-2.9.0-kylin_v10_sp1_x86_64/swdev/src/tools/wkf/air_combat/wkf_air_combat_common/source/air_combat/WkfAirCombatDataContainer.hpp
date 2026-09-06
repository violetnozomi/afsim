// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFAIRCOMBATDATACONTAINER_HPP
#define WKFAIRCOMBATDATACONTAINER_HPP

#include "wkf_air_combat_common_export.h"

#include <map>
#include <vector>

#include <QObject>
#include <QString>

namespace wkf
{
struct KinematicData
{
   // Kinematic data
   double mAltitude_ft       = 0.0;
   double mVerticalSpeed_fpm = 0.0;
   double mKCAS              = 0.0;
   double mKTAS              = 0.0;
   double mMach              = 0.0;
   double mNx                = 0.0;
   double mAlpha_deg         = 0.0;
   double mGLoad             = 0.0;
   double mAvailG            = 0.0;
   double mHeading_deg       = 0.0;
   double mPitch_deg         = 0.0;
   double mRoll_deg          = 0.0;
};

struct FuelData
{
   // Fuel data
   double mCurrentInternalFuel_lbs  = 0.0;
   double mCurrentExternalFuel_lbs  = 0.0;
   double mInternalFuelCapacity_lbs = 0.0;
   double mExternalFuelCapacity_lbs = 0.0;
   double mTotalFuel_lbs            = 0.0;
   double mNormalizedFuelLevel      = 0.0;
   double mJokerFuel_lbs            = 0.0;
   double mBingoFuel_lbs            = 0.0;
   double mTimeToJoker_sec          = 0.0;
   bool   mJokerReached             = 0.0;
   bool   mBingoReached             = 0.0;
};

struct WeaponsData
{
   // Weapons data (maps name of weapon to quantity of weapon)
   std::map<std::string, int> mWeaponsQtyMap;
   int                        mInitialTotalWeaponCount = 0;
   std::string                mSelectedWeapon;
   bool                       mSupportingWeapon = false;
};

struct EmissionsData
{
   // Emissions data
   bool mRadarEmitting        = false;
   bool mJammerEmitting       = false;
   bool mOtherSystemsEmitting = false;
};

struct SignatureData
{
   // Signature data
   bool mAfterburnerOn      = false;
   bool mContrailing        = false;
   bool mWeaponBayDoorsOpen = false;
};

struct SA_Data
{
   // SA data
   std::string mMissionTask;
   uint8_t     mRiskAcceptable = 0; // LOW = 0, MEDIUM = 1, HIGH = 2
   double      mRiskCurrent    = 0.0;
   double      mDefensiveness  = 0.0;
   double      mUrgency        = 0.0;
   double      mSelfRisk       = 0.0;
   double      mFlightRisk     = 0.0;
   double      mPackageRisk    = 0.0;
   double      mMissionRisk    = 0.0;
};

struct PlatformData
{
   std::string mPlatformName;
   std::string mPlatformType;
   size_t      mPlatformIndex = 0;
};

struct EngagementData
{
   // Prioritized targets and threats
   std::vector<std::string> mPrioritizedTargetNames;
   std::vector<std::string> mPrioritizedThreatNames;

   // Platforms that are currently being tracked
   std::vector<std::string> mTrackedPlatformNames;

   // Engagement data
   std::map<std::string, float>       mAngleOffMap_deg;
   std::map<std::string, float>       mAspectAngleMap_deg;
   std::map<std::string, float>       mCurrentRangeMap_nm;
   std::map<std::string, float>       mCurrentDeltaAltMap_ft;
   std::map<std::string, float>       mTrackQuality;
   std::map<std::string, std::string> mTargetCID;
   std::map<std::string, uint8_t>     mTargetThreatLevel;
   std::map<std::string, float>       mRawDetectionAgainstTargetMap_nm;
   std::map<std::string, float>       mRawWEZAgainstTargetMap_nm;
   std::map<std::string, float>       mWEZMinRangeMap_nm;
   std::map<std::string, float>       mWEZMaxRangeMap_nm;
   std::map<std::string, float>       mWEZNoEscapeMap_nm;
   std::map<std::string, float>       mRiskMap;
   std::map<std::string, float>       mDefensivenessMap;
   std::map<std::string, float>       mUrgencyMap;
   std::map<std::string, bool>        mTargetLocked;
   std::map<std::string, bool>        mAcceptableWEZ;
};

struct AirCombatDataEngagementSummary
{
   PlatformData   platformData;
   KinematicData  kinematicData;
   FuelData       fuelData;
   WeaponsData    weaponsData;
   EmissionsData  emissionsData;
   SignatureData  signatureData;
   SA_Data        saData;
   EngagementData engagementData;
};

class WKF_AIR_COMBAT_COMMON_EXPORT DataContainer : public QObject
{
   Q_OBJECT

public:
   DataContainer() = default;
   DataContainer(const DataContainer& aSrc);

   const std::vector<std::string>& GetViablePlatformNames() const { return mViablePlatforms; }
   bool                            IsPlatformViable(const std::string& aPlatformName) const;
   void AddViablePlatform(const std::string& aPlatformName) { mViablePlatforms.emplace_back(aPlatformName); }
   void RemoveViablePlatform(const std::string& aPlatformName);

   std::vector<wkf::AirCombatDataEngagementSummary>& GetEngagementDataList() { return mDataList; }
   void SetEngagementDataList(const std::vector<wkf::AirCombatDataEngagementSummary>& aDataList)
   {
      mDataList = aDataList;
   }
   void RemoveEngagementFromDataList(std::vector<wkf::AirCombatDataEngagementSummary>::iterator aItr)
   {
      mDataList.erase(aItr);
   }
   void AddEngagementToDataList(const wkf::AirCombatDataEngagementSummary& aData) { mDataList.emplace_back(aData); }

   void Clear();

private:
   std::vector<wkf::AirCombatDataEngagementSummary> mDataList;
   std::vector<std::string>                         mViablePlatforms;
};
} // namespace wkf

#endif // !WKFAIRCOMBATDATACONTAINER_HPP
