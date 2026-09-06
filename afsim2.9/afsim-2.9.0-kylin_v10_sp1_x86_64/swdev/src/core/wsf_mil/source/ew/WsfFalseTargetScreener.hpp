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

#ifndef WSFFALSETARGETSCREENER_HPP
#define WSFFALSETARGETSCREENER_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

#include "UtCloneablePtr.hpp"
class UtInput;
class UtInputBlock;
#include "UtRandom.hpp"

class WsfEM_Xmtr;
class WsfEM_Interaction;
class WsfEW_FalseTargetEffect;
#include "WsfFalseTarget.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensor.hpp"
class WsfSensorMode;
class WsfTrack;
#include "WsfTrackId.hpp"

//! A class for collecting false target blips and merging them with existing tracks in the sensor tracker.
//!
//! @see WsfSensorTracker
//! @see WsfDefaultSensorTracker
class WSF_MIL_EXPORT WsfFalseTargetScreener : public WsfObject
{
public:
   using FT_JammerPair      = std::pair<WsfFalseTarget*, WsfEM_Xmtr*>;
   using FT_JammerIdPair    = std::pair<WsfStringId, unsigned int>;
   using FT_JammerVec       = std::vector<FT_JammerPair>;
   using FT_Result          = std::map<unsigned int, WsfEM_Interaction*>;
   using BlipAlloc          = std::map<WsfFalseTarget::BlipBehavior, int>;
   using BlipIdFTJammerList = std::vector<std::pair<size_t, FT_JammerPair>>;
   using TargetIds          = std::vector<size_t>;
   using RR_Map             = std::map<WsfFalseTarget::BlipBehavior, double>; // rejection ratio
   using RR_MapPair         = std::pair<WsfFalseTarget::BlipBehavior, double>;
   using TrackIdMap         = std::map<size_t, WsfTrackId>;

   struct ForceData
   {
      ForceData(double aAzimuth, double aRange, double aSNR)
         : mAzimuth(aAzimuth)
         , mRange(aRange)
         , mSNR(aSNR)
      {
      }

      double mAzimuth;
      double mRange;
      double mSNR;
   };
   using BlipIdToForceDataMap = std::map<size_t, ForceData>;

   //! Describe the output state of this object.
   enum OutputType
   {
      cPLOT,    //!< Producing plotted output, a pseudo-radar screen representation.
      cTRACKS,  //!< Producing tracks, for communication to external entities.
      cOVERLOAD //!< Countermeasures have produced an excessive number of false targets, causing an overload.
   };

   //! Describe whether the electronic attack is successful in getting enough
   //! "applied technique" energy into the target receiver for the desired effect.
   enum Picture
   {
      cPERFECT,    //!< Enough energy is injected to produce the desired effect.
      cDEGRADED,   //!< Marginally effective.
      cINEFFECTIVE //!< Not successful with the desired effect.
   };

   //! An enumeration that indicates the blip range constraining type to force target tracks.
   //! Note: Do not change these values. cFORCE_IN_OUT must equal cFORCE_INSIDE | cFORCE_OUTSIDE.
   enum RangeForceType
   {
      cFORCE_NEITHER = 0, //<! The part cannot be slewed
      cFORCE_INSIDE  = 1, //<! The part can be slewed only in azimuth
      cFORCE_OUTSIDE = 2, //<! The part can be slewed only in elevation
      cFORCE_IN_OUT  = 3  //<! The part can be slewed in both azimuth and elevation
   };

   static WsfFalseTargetScreener* Find(const WsfSensorMode& aSensorMode);

   explicit WsfFalseTargetScreener(const WsfScenario& aScenario);
   WsfFalseTargetScreener(const WsfFalseTargetScreener& aSrc);
   WsfFalseTargetScreener& operator=(const WsfFalseTargetScreener&) = delete;
   ~WsfFalseTargetScreener() override;
   WsfFalseTargetScreener* Clone() const override;

   virtual bool Initialize(double aSimTime, WsfSensorMode* aSensorModePtr, bool aReportsSignalToNoise);
   bool         ProcessInput(UtInput& aInput) override;
   void         ProcessInputBlock(UtInputBlock& aInputBlock);

   bool NeedsUpdated(double aSimTime);
   void Update(double aSimTime, WsfSensor::Settings& aSettings);
   void Reset();
   void PrintScreenerState(double aSimTime);

   void           SetSensorModePtr(WsfSensorMode* aSensorModePtr) { mSensorModePtr = aSensorModePtr; }
   WsfSensorMode* GetSensorModePtr() { return mSensorModePtr; }

   bool ReportsSNR() const { return mReportsSignalToNoise; }

   OutputType GetScreenerOutputType() const { return mOutputType; }

   //! @name Status methods.
   //@{
   bool GetDebugEnabled() const { return mDebugEnabled; }
   bool GetScreenerValid() { return mScreenerValid; }
   //@}

   //! @name Data methods.
   //@{
   int    GetPlotCapacity() const { return mPlotCapacity; }
   int    GetTrackCapacity() const { return mTrackCapacity; }
   int    GetTransferCapacity() const { return mTransferCapacity; }
   int    GetRangeForceType() const { return mRangeForceType; }
   double GetForceSectorArc() const { return mForceSectorArc; }
   double GetForceRange() const { return mForceRange; }
   int    GetForceCountPerRangeSector() const { return mForceCountPerRangeSector; }
   double GetForceJtoSDeltaThreshold() const { return mForceJtoSDeltaThreshold; }
   //@}

   //! @name Blip Methods.
   //@{
   bool                  FindTrackableBlip(size_t aBlipId);
   BlipIdFTJammerList&   GetTrackableBlipIds() { return mTrackableBlipIds; }
   WsfFalseTarget::Blip* GetTrackableBlip(size_t aBlipId);

   bool                  FindBlockedBlip(size_t aBlipId);
   BlipIdFTJammerList&   GetBlockedBlipIds() { return mBlockedBlipIds; }
   WsfFalseTarget::Blip* GetBlockedBlip(size_t aBlipId);

   bool                  FindUndetectedBlip(size_t aBlipId);
   BlipIdFTJammerList&   GetUndetectedBlipIds() { return mUndetectedBlipIds; }
   WsfFalseTarget::Blip* GetUndetectedBlip(size_t aBlipId);
   //@}

   WsfFalseTarget* GetFalseTarget(const std::string& aName, WsfEM_Xmtr* aJammerXmtrPtr) const;

   bool AddFalseTarget(FT_JammerPair aFT_JammerPair);
   bool RemoveFalseTarget(FT_JammerPair aFT_JammerPair);
   bool RemoveFalseTargets(WsfFalseTarget* aFalseTargetPtr);
   bool FindFalseTarget(FT_JammerPair aFT_JammerPair);

   WsfEM_Xmtr* GetJammerXmtr(WsfFalseTarget* aFalseTargetPtr) const;
   bool        RemoveJammerXmtr(WsfEM_Xmtr* aJammerXmtrPtr);

   TrackIdMap GetActiveTargetTracks() const;
   TrackIdMap GetBlockedTargets() { return mBlockedTargetTrackIds; }

   //! @name False target track methods.
   //@{
   // TODO - see about refactoring some of these into WsfDefaultSensorTracker
   void       AddFalseTargetTrack(size_t aBlipId, WsfTrackId aTrackId);
   void       DropFalseTargetTrack(size_t aBlipId);
   bool       FindFalseTargetTrack(size_t aBlipId);
   TrackIdMap GetFalseTargetTracks() { return mFalseTargetTrackIds; };

   bool IsRequestBasedTracking() const { return mRequestBasedTracking; }
   bool OnlyScreenFT_TrackRequests() const { return mOnlyScreenFT_Requests; }

   inline void AddTargetTrack(WsfStringInt aObjectId, WsfTrack* aValidTrackPtr)
   {
      return UpdateTargetTrack(aObjectId, aValidTrackPtr);
   }

   void UpdateTargetTrack(size_t aObjectId, WsfTrack* aValidTrackPtr);
   void DropTargetTrack(size_t aObjectId);
   bool AllowTargetTrack(double aSimTime, size_t aObjectId, WsfTrack* aTrackPtr, const WsfSensorResult& aResult);
   bool AllowTrackReporting(size_t aObjectId, const WsfTrack* aTrackPtr);
   void TracksTransfered();
   bool TrackExists(size_t aObjectId);
   //@}

   bool IsRadarModeTurnedOn();

   //! @name Fusion Center (WsfFusionCenter) methods.
   //@{

   int GetPassedFalseTargetNumber() const { return mPassedFalseTargetNumber; }
   int GetPassedRealTargetNumber() const { return mPassedRealTargetNumber; }
   int GetFalseTargetNumber() const { return (int)mTrackableBlipIds.size(); }
   int GetRealTargetNumber() const { return (int)(mActiveTargetTrackIds.size() + mForcedTargetTrackIds.size()); }
   //@}

   WsfPlatform*   GetTempPlatform() { return mTempPlatformPtr.get(); }
   WsfSimulation* GetSimulation() const;

protected:
   //! Reportable Track Id list from the sensor tracker. The sensor tracker manages this list.
   TrackIdMap mReportingTrackIds;

   //! List of false target jammer pairs being considered by screener function
   FT_JammerVec mFalseTargetsJammers;

   //! @name Target track list variables.
   //! A target track Id is only allowed to be in one of the three available lists at any instance.
   //@{
   //! List of active target track Ids that are being tracked in the sensor tracker.
   TrackIdMap mActiveTargetTrackIds;

   //! List of target track Ids which exist in the sensor tracker,
   //! but are not being tracked due to maximum track capacity limitations
   TrackIdMap mBlockedTargetTrackIds;

   //! List of target track Ids whose range from the sensor is less than the mMinimumFalseTargetRange
   //! and are being forced as tracks to the sensor tracker.
   TrackIdMap mForcedTargetTrackIds;

   //! List of target track Ids that are being detected but not yet a track in the sensor tracker.
   TrackIdMap mNullTargetTrackIds;

   bool mTargetsAdjusted;
   bool mTargetsInvalid;
   //@}

   //! The last time a full update of the screener was achieved.
   double mLastUpdate;

   //! @name Blip list variables.
   //! A blip Id is only allowed to be in one of the three available lists at any instance.
   //@{
   //! Blocked but detectable blip list.
   BlipIdFTJammerList mBlockedBlipIds;

   //! Undetectable blip list.
   BlipIdFTJammerList mUndetectedBlipIds;

   //! Track-able blip list (i.e. detected).
   BlipIdFTJammerList mTrackableBlipIds;

   bool mBlipsAdjusted;
   bool mBlipsInvalid;
   //@}

   int GetAllowedRealTargetNumber() const { return mAllowedRealTargetNumber; }

   void CheckFT_Toggle();

   void RemoveBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtrPtr);

   void AdjustTargetsAndBlips(double aSimTime);
   bool ForceTargetTrack(double aRadar2TargetRange, double aRadar2TargetAzimuth, double aTargetSNR);

private:
   void SetScreenerValid(bool aBoolValue) { mScreenerValid = aBoolValue; }

   bool CreateBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtrPtr);
   void RemoveBlockedBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr);
   void RemoveUndetectedBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr);
   void RemoveTrackableBlips(WsfFalseTarget* aFalseTargetPtr, WsfEM_Xmtr* aJammerXmtr);
   void UpdateBlips(double aSimTime, WsfSensor::Settings& aSettings);
   void UpdateFalseTargetJammers(double aSimTime);

   size_t PruneTargetTracks(size_t aPruneQuantity);

   size_t AddTrackableBlips(size_t aAddQuantity);
   size_t PruneTrackableBlips(size_t aPruneQuantity);

   bool RejectBlips(double aSimTime);

   //! Pointer to the owning sensor/articulated part.
   WsfSensorMode* mSensorModePtr;

   //! Flag as to whether or not the sensor mode reports the SNR or not, used by the J/S checks
   bool mReportsSignalToNoise;

   bool mDebugEnabled;
   bool mScreenerValid;

   ut::Random mRandom;

   BlipAlloc mBlipsAllocation;

   //! Initial FT rejection ratio based upon the false-target's Behavior
   RR_Map mInitialRejectionRatios;

   //! Run-time FT rejection ratio based upon the blips Behavior
   RR_Map mRejectionRatios;

   //! Based on false target loading will the screener be producing tracks or plots
   OutputType mOutputType;

   //! Block ALL track updates (false target and real) when screener is flooded.
   //! (i.e. when the transfer capacity is less than the registered tracks
   //!  all track updates should be blocked).
   bool mTrackReportingWhenFlooded;

   //! Produce a randomized plot of false targets for display only
   bool mProduceOutputPlot;

   //! Respond to track requests to trigger the screener output.
   bool mRequestBasedTracking;

   //! Respond to requests to screen/track false targets only. Will cause all other false target tracks to be ignored.
   bool mOnlyScreenFT_Requests;

   Picture mPicture;

   //@name Maximum capacity variables.
   //@{
   //! Number of tracks which can be plotted on the radar scope
   int mPlotCapacity;

   //! Number of tracks (false and real) which can be passed
   int mTrackCapacity;

   //! Number of tracks (false and real) which can be passed
   int mTransferCapacity;
   //@}

   //! Status of the false target jammers that this screener is working with, as defined in mFalsetargetJammers list.
   //! 'true' if there was an update to the status during a screener update.
   bool mFT_JammerStatusChange;

   //! The last blip update time.
   double mLastBlipUpdate;

   //! @name Forced target track variables.
   //@{
   int    mForceCountPerRangeSector;
   double mForceJtoSDeltaThreshold;
   double mForceSectorArc;
   double mForceRange;

   RangeForceType mRangeForceType;

   //! The minimum blip range for the track-able blips that was found in the last blip update.
   // Used to force target detections and tracks if the target range is less than this value.
   double mMinimumBlipRange;

   //! The maximum blip range for the track-able blips that was found in the last blip update.
   // Used to force target detections and tracks if the target range is less than this value.
   double mMaximumBlipRange;

   //! The list of all blocked and track-able blips associated range and azimuth wrt sensor sorted by blip Id.
   //! @note This list is cleared and updated by the UpdateBlips(...) method, also cleared by the Reset() method.
   BlipIdToForceDataMap mBlipIdToForceDataMap;
   //@}

   //! Number of false targets passed on, used by fusion center only.
   int mPassedFalseTargetNumber;

   //! Number of real targets passed on, used by fusion center only.
   int mPassedRealTargetNumber;

   //! Number of false targets allowed following screener "algorithmics"
   int mAllowedFalseTargetNumber;

   //! Number of real targets allowed following screener "algorithmics"
   int mAllowedRealTargetNumber;

   //! False target track Id list from the sensor tracker. The sensor tracker manages this list.
   TrackIdMap mFalseTargetTrackIds;

   //! Temporary platform used in interaction calculations, defined here to save runtime by defining it here
   ut::CloneablePtr<WsfPlatform> mTempPlatformPtr;
};

#endif
