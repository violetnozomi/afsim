// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFFALSETARGET_HPP
#define WSFFALSETARGET_HPP

#include "wsf_mil_export.h"

#include <cfloat> // for DBL_MAX
#include <map>
#include <mutex>
#include <vector>

#include "UtEntity.hpp"
class UtInput;
#include "UtRandom.hpp"
#include "UtVec2.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Interaction.hpp"
class WsfEM_Xmtr;
class WsfEM_Rcvr;
class WsfEW_EffectManager;
class WsfEM_Interaction;
class WsfFalseTargetBlip;
class WsfFalseTargetScreener;
#include "WsfObject.hpp"
class WsfPlatform;
class WsfScenario;
class WsfSimulation;
#include "WsfStringId.hpp"

//! An instance of this class quantitatively models the multiple false targets that will be perceived by
//  radar receivers that interact with the transmitter that includes this technique as part of its arsenal
//! Electronic Attack (EA) techniques.
//! Different instances of this type will be created if the randomness among multiple radar sites.
//! * A per platform instance is used if this type is to be consistent among different radar sites but random to
//! multiple platforms.
//! * A single instance is used if this type is to be consistent among different radar sites and consistent to multiple
//! platforms.

class WSF_MIL_EXPORT WsfFalseTarget : public WsfObject
{
private:
   // static std::vector<double> sRCS_Values;
   static std::map<WsfStringInt, size_t> sFT_NameCounter;
   static size_t                         sFT_UniqueId;
   static std::recursive_mutex           sFT_Mutex;

public:
   using Blip = WsfFalseTargetBlip;
   friend class WsfFalseTargetBlip;

   static void   ResetStatic();
   static size_t AssignFT_BlipUniqueId();

   //! "PlatformBehavior" describes the observed time-variant behavior for the false-target blips
   //! among different jamming platforms, this behavior coupled with the "RadarBehavior" defines
   //! how cCONSISTENT_TO_MULTIPLE_RADARS are fused among different jamming platforms:
   enum PlatformBehavior
   {
      cRANDOM_TO_MULTIPLE_PLATFORMS,    //!< Perception from multiple platforms is uncorrelated.
      cCONSISTENT_TO_MULTIPLE_PLATFORMS //!< Perception from multiple platforms is correlated.
   };

   //! "RadarBehavior" describes the observed time-variant behavior for the false-target blips among different radar sites:
   enum RadarBehavior
   {
      cRANDOM_TO_MULTIPLE_RADARS,    //!< Perception from multiple radars is uncorrelated.
      cCONSISTENT_TO_MULTIPLE_RADARS //!< Perception from multiple radars is correlated.
   };

   //! "BlipBehavior" describes the observed time-variant behavior for the false-target blips
   //! for each radar site:
   enum BlipBehavior
   {
      cRANDOM_SCAN_TO_SCAN,    //!< Perception changes randomly from scan to scan.
      cCONSISTENT_SCAN_TO_SCAN //!< Relative to the true track geometry, perception is consistent from scan to scan.
   };

   //! "BlipDistribution" appearance describes how the false targets are spread over an area surrounding a radar site.
   enum BlipDistribution
   {
      cRANDOM,
      cCIRCULAR,
      cSECTORED
   };

   //! "RangeDistribution"  describes how the false targets are spread over the range from the radar site.
   enum RangeDistribution
   {
      cUNIFORM,
      cLOG,
      cEXPONENTIAL
   };

   //! OnOffState indicates what the state of this false target technique.
   enum OnOffState
   {
      cOn,
      cOff
   };

   //! ReferenceBearing indicates what the reference angle for zero azimuth is.
   enum ReferenceBearing
   {
      cJAMMER,
      cNORTH,
      cPROTECTED_ENTITY
   };

   // Blip reference for speed and heading
   enum BlipReference
   {
      cBR_NONE,  //!< Uses the input speed and heading for the false target blip movement.
      cBR_JAMMER //!< Uses the jammer (i.e. transmitter) platform for the heading and speed of the blips.
   };

   struct JammerState
   {
      bool         mBlipsCreated{false};
      double       mLastScan{std::numeric_limits<double>::lowest()};
      double       mLastUpdate{std::numeric_limits<double>::lowest()};
      double       mLastBlipUpdate{std::numeric_limits<double>::lowest()};
      double       mLastBlipLocWCS[3]{0.0};
      unsigned int mScreenedNumberOfFTs{0};
      bool         mFTsRejected{false};
   };

   struct BaseData
   {
      double      mLocationWCS[3];
      double      mSpeed;
      double      mHeading;
      WsfStringId mNameId;
      size_t      mUniqueId;

      void Clear()
      {
         UtVec3d::Set(mLocationWCS, 0.0);
         mSpeed    = 0.0;
         mHeading  = 0.0;
         mNameId   = nullptr;
         mUniqueId = 0;
      }
   };

   struct ExtentPair
   {
      double mMin;
      double mMax;
   };

   using RadarSnsrPair = std::pair<WsfStringInt, WsfStringInt>;
   using ExtentData    = std::vector<ExtentPair>;

   using BaseLocsVec        = std::vector<BaseData>;
   using Blips              = std::vector<WsfFalseTargetBlip*>;
   using BlipsIterators     = std::vector<Blips::iterator>;
   using JammerIdToBlipsMap = std::map<unsigned int, Blips>;
   using JammerIdToStateMap = std::map<unsigned int, JammerState>;

   using ScreenerPtrJammerXmtrPtr = std::pair<WsfFalseTargetScreener*, WsfEM_Xmtr*>;
   using BlipStateVec             = std::vector<ScreenerPtrJammerXmtrPtr>;

   explicit WsfFalseTarget(WsfScenario& aScenario);
   WsfFalseTarget(const WsfFalseTarget& aSrc);
   WsfFalseTarget& operator=(const WsfFalseTarget&) = delete;
   ~WsfFalseTarget() override;

   WsfFalseTarget* Clone() const override;
   virtual bool    Initialize(WsfSimulation* aSimulationPtr);
   bool            ProcessInput(UtInput& aInput) override;
   virtual void    Update(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr);

   bool Initialize(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr);
   void Reset();

   BlipBehavior     GetBlipBehavior() const { return mBlipBehavior; }
   RadarBehavior    GetRadarBehavior() const { return mRadarBehavior; }
   PlatformBehavior GetPlatformBehavior() const { return mPlatformBehavior; }

   double GetScanInterval() const { return mScanInterval; }

   bool GetNewBlipsState(WsfFalseTargetScreener* aScreenerPtr, WsfEM_Xmtr* aJammerXmtrPtr);
   void SetNewBlipsState(WsfFalseTargetScreener* aScreenerPtr, WsfEM_Xmtr* aJammerXmtrPtr, bool aState);

   virtual void ClearAllBlips();
   virtual void ClearBlips(unsigned int aJammerId);
   Blips&       GetBlips(unsigned int aJammerId);
   Blip*        GetBlipPtr(WsfStringId aBlipNameId, unsigned int aJammerId);
   Blip*        GetBlipPtr(size_t aBlipUniqueId, unsigned int aJammerId);
   bool         GetBlipsCreated(unsigned int aJammerId);
   void         CreateBlipNameId(Blip* aBlipPtr);

   void SetBlipBehavior(BlipBehavior aBlipBehavior) { mBlipBehavior = aBlipBehavior; };
   void SetRadarBehavior(RadarBehavior aRadarBehavior) { mRadarBehavior = aRadarBehavior; }
   void SetPlatformBehavior(PlatformBehavior aPlatformBehavior) { mPlatformBehavior = aPlatformBehavior; }
   void SetScanInterval(double aScanInterval);
   void SetPersistence(double aPersistence);
   void SetInitialNumberOfFTs(unsigned int aInitialNumberOfFTs, bool aIsExplicit = false);

   void SetAzimuthExtent(double aAzLower, double aAzUpper);
   void SetRangeExtent(double aRangeLower, double aRangeUpper);
   void SetBlipSpeeds(double aMinSpeed, double aMaxSpeed);

   void GetBlipSpeeds(double& aMinBlipSpeed, double& aMaxBlipSpeed) const;
   void GetBlipHeadings(double& aMinBlipHeading, double& aMaxBlipHeading) const;

   void             SetBearingReference(ReferenceBearing aBearingReference) { mBearingReference = aBearingReference; }
   ReferenceBearing GetBearingReference() const { return mBearingReference; }

   void         SetReferenceCentroid(double aLatitude, double aLongitude, double aAltitude);
   void         GetReferenceCentroid(double& aLatitude, double& aLongitude, double& aAltitude);
   WsfPlatform* GetCentroidPlatform() const { return mCentroidPlatformPtr; }

   virtual void RemoveJammer(WsfEM_Xmtr* aJammerXmtrPtr);

   bool GetChanged() const { return mChanged; }
   void SetDebug(bool aDebug) { mDebug = aDebug; }
   bool GetDebug() const { return mDebug; }

   unsigned int GetInitialNumberOfFTs() const { return mInitialNumberOfFTs; }
   double       GetPersistence() const { return mPersistence; }
   bool         GetRangeConstrained() const { return mRangeConstrained; }
   void         SetRangeConstrained(bool aRangeConstrained) { mRangeConstrained = aRangeConstrained; }

   void       SetState(OnOffState aState) { mState = aState; }
   OnOffState GetState() const { return mState; }

   void SetToggled(bool aToggled) { mToggled = aToggled; }
   bool GetToggled() const { return mToggled; }

   void DropTracks(RadarSnsrPair aIdKey, WsfEM_Xmtr* aJammerXmtrPtr);

   // bool UpdateGeometry(double aSimTime, WsfEM_Rcvr* aRcvrPtr, WsfEM_Xmtr* aJammerXmtrPtr);

   bool PowerSufficiencyCheck(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr, WsfEM_Xmtr* aRadarXmtrPtr, WsfEM_Rcvr* aRadarRcvrPtr);

   void SetBlips(int aJammerID, WsfFalseTarget::Blips& aBlips) { mBlipsMap[aJammerID] = aBlips; }

   bool ExplicitAzimuthExtent() const { return mExplicitAzimuthExtent; }
   bool ExplicitNumberOfFTs() const { return mExplicitNumberOfFTs; }
   bool ExplicitRangeConstraint() const { return mExplicitRangeConstraint; }
   bool ExplicitRangeExtent() const { return mExplicitRangeExtent; }
   bool ExplicitReferenceCentroid() const { return mExplicitReferenceCentroid; }
   bool ExplicitScanInterval() const { return mExplicitScanInterval; }
   bool ExplicitBlipSpeeds() const { return mExplicitBlipSpeeds; }


protected:
   WsfSimulation* GetSimulation() const { return mSimulationPtr; }
   virtual void   Rescan(double aSimTime, WsfEM_Xmtr* aJammerXmtrPtr);
   virtual void   UpdateBlipPositions(double aSimTime, unsigned int aJammerId);
   void           UpdateBaseBlipPositions(double aSimTime);
   void           UpdateCentroidPosition(double aSimTime);

private:
   bool             mDebug;
   WsfSimulation*   mSimulationPtr;
   BlipBehavior     mBlipBehavior;
   RadarBehavior    mRadarBehavior;
   PlatformBehavior mPlatformBehavior;
   unsigned int     mInitialNumberOfFTs;
   double           mScanInterval;
   double           mPersistence;

   ExtentData        mAzimuthExtent;
   ExtentData        mRangeExtent;
   bool              mRangeConstrained;
   ReferenceBearing  mBearingReference;
   RangeDistribution mRangeDistribution;

   WsfPlatform* mCentroidPlatformPtr;
   double       mCentroidHeading;
   double       mCentroidSpeed;
   double       mLastCentroidUpdate;
   bool         mCentroidNeedsAGL_Adjustment;

   bool mExplicitReferenceCentroid;
   bool mExplicitNumberOfFTs;
   bool mExplicitScanInterval;
   bool mExplicitAzimuthExtent;
   bool mExplicitRangeConstraint;
   bool mExplicitRangeExtent;
   bool mExplicitBlipSpeeds;

   double        mBlipHeadings[2];
   double        mBlipSpeeds[2];
   BlipReference mBlipReference;

   bool   mInitialized;
   double mInitializationTime;

   bool mChanged;

   JammerIdToBlipsMap mBlipsMap;
   BaseLocsVec        mBaseLocations;

   WsfStringId mFusedPlatformId;

   JammerIdToStateMap mJammerIdToStateMap;

   double mLastBaseLocWCS[3];
   double mLastBaseLocsUpdate;
   double mLastScan;
   double mLastUpdate;

   BlipStateVec mNewBlipsStateVec;

   OnOffState mState;
   // WsfPlatform*       mTempPlatformPtr;
   bool mToggled;

   // Unique Blip Name Id variables
   size_t mFTNameExtensionId;
   size_t mBlipIdCounter;

   ut::Random mRandom;

   mutable std::recursive_mutex mMutex;
};

class WSF_MIL_EXPORT WsfFalseTargetBlip : public UtEntity
{
public:
   using RadarSnsrPair = WsfFalseTarget::RadarSnsrPair;
   using BlipBehavior  = WsfFalseTarget::BlipBehavior;
   using RadarSnsrVec  = std::vector<RadarSnsrPair>;

   //! "BlipStrength" describes the false-target signal strength being received
   //! by the targeted radar's receiver.  At this point this value reflects the
   //! the computed jammer SNR versus the desired BlipType SNR
   enum BlipStrength
   {
      cStrong,
      cWeak,
      cUndetected
   };

   struct BlipData
   {
      double             mActualSNR;
      double             mRadarToBlipRange;
      double             mEquivalentRCS;
      double             mJammerToRadarRange;
      BlipStrength       mStrength;
      double             mLocationDelta[3];
      WsfEM_Interaction* mInteractionPtr;

      void Clear()
      {
         mActualSNR          = -DBL_MAX;
         mRadarToBlipRange   = -DBL_MAX;
         mEquivalentRCS      = -DBL_MAX;
         mJammerToRadarRange = -DBL_MAX;
         mStrength           = cUndetected;
         UtVec3d::Set(mLocationDelta, 0.0);
         if (mInteractionPtr != nullptr)
         {
            mInteractionPtr->Reset();
         }
      }
   };

   using BlipDataMap = std::map<RadarSnsrPair, BlipData>;

   WsfFalseTargetBlip();
   WsfFalseTargetBlip(const WsfFalseTargetBlip& aSrc) = delete;

   ~WsfFalseTargetBlip() override;

   void         SetPlatform(WsfPlatform* aPlatformPtr) { mPlatformPtr = aPlatformPtr; }
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   void        SetJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr) { mJammerXmtrPtr = aJammerXmtrPtr; }
   WsfEM_Xmtr* GetJammerXmtr() const { return mJammerXmtrPtr; }

   WsfFalseTarget* GetFalseTarget() const { return mFtPtr; }
   void            SetFalseTarget(WsfFalseTarget* aFtPtr) { mFtPtr = aFtPtr; }

   BlipBehavior GetBehavior() const { return mFtBehavior; }
   void         SetBehavior(BlipBehavior aFalseTargetBehavior) { mFtBehavior = aFalseTargetBehavior; }

   void AddStrength(RadarSnsrPair aIdKey, BlipStrength aStrength);

   void AddTargetedRadarId(RadarSnsrPair aIdKey);
   bool IsValidTargetedRadarId(RadarSnsrPair aIdKey);
   void ResetBlipData(RadarSnsrPair aIdKey);
   void UpdateBlipData(RadarSnsrPair            aIdKey,
                       const WsfEM_Interaction& aBlipInteraction,
                       const WsfEM_Interaction& aJammerInteraction);

   void   AddRadarToBlipRange(RadarSnsrPair aIdKey, double aRadar2BlipRange);
   double GetRadarToBlipRange(RadarSnsrPair aIdKey);

   void   AddJammerToRadarRange(RadarSnsrPair aIdKey, double aJammer2RadarRange);
   double GetJammerToRadarRange(RadarSnsrPair aIdKey);
   double GetReferenceJammerToRadarRange();

   void         UnDetect(RadarSnsrPair aIdKey);
   BlipStrength GetStrength(RadarSnsrPair aIdKey);

   void AddLocationDelta(RadarSnsrPair aIdKey, const double aLocationDelta[3]);
   void GetLocationDelta(RadarSnsrPair aIdKey, double aLocationDelta[3]);

   void   AddActualSNR(RadarSnsrPair aIdKey, double aActualSNR);
   double GetActualSNR(RadarSnsrPair aIdKey);
   double ComputeSNR(double aSimTime, WsfEM_Interaction& aBlipInteraction, WsfEM_Interaction& aJammerInteraction);
   double GetMinimumActualSNR(const RadarSnsrVec& aRadarSensorVec);

   double ComputeRcvdPower(double aSimTime, WsfEM_Interaction& aBlipInteraction, WsfEM_Interaction& aJammerInteraction);

   void   AddEquivalentRCS(RadarSnsrPair aIdKey, double aEquivalentRCS);
   double GetEquivalentRCS(RadarSnsrPair aIdKey);
   double ComputeEquivalentRCS(const WsfEM_Interaction& aBlipInteraction) const;

   // double GetToRadarRange(RadarSnsrPair aIdKey) const;

   void        SetNameId(WsfStringId aNameId) { mNameId = aNameId; }
   WsfStringId GetNameId() const { return mNameId; }

   std::string GetTypeId() const { return mFT_BlipTypeId; }

   void   SetUniqueId(size_t aUniqueId) { mUniqueId = aUniqueId; }
   size_t GetUniqueId() const { return mUniqueId; }

   void               SetInteraction(RadarSnsrPair aIdKey, WsfEM_Interaction* aInteractionPtr);
   WsfEM_Interaction* GetInteraction(RadarSnsrPair aIdKey);

private:
   WsfStringId mNameId;
   size_t      mUniqueId;

   WsfStringId mFT_BlipTypeId;

   // Map of the radar to data
   BlipDataMap mDataMap;

   BlipBehavior    mFtBehavior;
   WsfFalseTarget* mFtPtr;
   WsfEM_Xmtr*     mJammerXmtrPtr;

   // Platform instance that may be set if needed by WsfFusionCenter.
   // Note: Care must be taken as this uses a WsfUniqueId in the simulation when used.
   WsfPlatform* mPlatformPtr;

   RadarSnsrVec mRadarSensorIds;

   mutable std::recursive_mutex mMutex;
};

#endif
