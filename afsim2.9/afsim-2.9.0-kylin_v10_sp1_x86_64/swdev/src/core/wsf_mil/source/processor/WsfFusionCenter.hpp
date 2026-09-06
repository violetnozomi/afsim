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

#ifndef WSFFUSIONCENTER_HPP
#define WSFFUSIONCENTER_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFalseTarget.hpp"
#include "WsfFalseTargetScreener.hpp"
class WsfFusionStrategy;
#include "WsfLocalTrack.hpp"
#include "script/WsfScriptProcessor.hpp"
class WsfTrack;
class WsfSensor;

//! The WsfFusionCenter object represents a collection center for real and false tracks from diverse
//! sources.  If required, it will filter them down according to some maximum capacity, and pass them
//! forward to some other entity for consideration.

class WSF_MIL_EXPORT WsfFusionCenter : public WsfScriptProcessor
{
public:
   enum Output
   {
      cPLOT,
      cCORRELATED_PLOT,
      cTRACKS,
      cOVERLOAD
   };

   using PlatformScreenerPair     = std::pair<WsfStringId, WsfFalseTargetScreener*>;
   using RejectionRatios          = std::map<WsfFalseTarget::RadarBehavior, double>;
   using RejectionRatiosIter      = std::map<WsfFalseTarget::RadarBehavior, double>::iterator;
   using IdVec                    = std::vector<size_t>;
   using Screeners                = std::vector<PlatformScreenerPair>;
   using ScreenersIter            = std::vector<PlatformScreenerPair>::iterator;
   using SiteIds                  = std::vector<WsfStringId>;
   using SiteIdsIter              = SiteIds::iterator;
   using RadarSensorIds           = std::vector<WsfFalseTarget::RadarSnsrPair>;
   using RadarSensorIdsIter       = std::vector<WsfFalseTarget::RadarSnsrPair>::iterator;
   using InterScreenerConsistency = std::map<size_t, unsigned int>;
   using IdToTrackIdMultiMap      = std::map<size_t, WsfTrackId>;
   using IdTrackIdPair            = std::pair<size_t, WsfTrackId>;
   using IdTrackIdList            = std::vector<IdTrackIdPair>;

   WsfFusionCenter(WsfScenario& aScenario);
   WsfFusionCenter(const WsfFusionCenter& aSrc);
   WsfFusionCenter& operator=(const WsfFusionCenter&) = delete;
   ~WsfFusionCenter() override;

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;

   // void         GeneratePlot();
   Output       GetOutputType() const { return mOutput; }
   void         PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   void         PrintFuserState(ut::log::MessageStream& aMsgStream, double aSimTime);
   bool         ReceiveMessage(double aSimTime, const WsfMessage& aMessage) override;
   void         Update(double aSimTime) override;
   virtual void UpdateState(double aSimTime);

   void SetConsistencyConstraint(bool aConsistencyConstrained) { mConsistencyConstrained = aConsistencyConstrained; }
   bool GetConsistenctConstraint() { return mConsistencyConstrained; }

protected:
   // void AddFalseTargetTracks(double aSimTime,
   //                           size_t aBlipsToAdd);
   void AdjustTrackQuantities(double aSimTime);
   void CheckForScreenerAdd(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);
   void CheckForScreenerReset(double aSimTime, WsfSensor* aSensorPtr, WsfSensorMode* aModePtr);
   // void CreateFalseTargetPlatforms(double aSimTime);
   bool   FT_Fusible(size_t aBlipId);
   bool   RT_Fusible(size_t aTargetId);
   size_t GetValidBlipCount();
   // void UpdateTrackableBlips(double aSimTime);
   // void UpdateActiveTracks(double aSimTime);
   void UpdateFalseTargetTracks(double aSimTime);
   void UpdateRealTargetTracks(double aSimTime);
   // void UpdateScreenerTracks();

private:
   bool AddScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr);
   bool DeleteScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr);
   bool DeleteAllScreenersOn(WsfStringId aPlatformId);
   void DeleteAllTracks(double aSimTime);
   void DetermineOutputType();
   bool FindScreener(WsfStringId aPlatformId, WsfFalseTargetScreener* aScreenerPtr) const;
   /*
         bool UpdateLocalTrackFromNonLocalTrack(double          aSimTime,
                                                WsfLocalTrack&  aLocalTrack,
                                                const WsfTrack& aNonLocalTrack);
         bool UpdateNonKinematicData(double aSimTime,
                                     WsfTrackManager& aTrackManager,
                                     WsfLocalTrack&   aLocalTrack,
                                     const WsfTrack&  aNonLocalTrack);

         bool            UpdateLocalTrackFromNonLocalTrack(double aSimTime,
                                                           WsfLocalTrack&  aLocalTrack,
                                                           const WsfTrack& aNonLocalTrack);
         bool            UpdateNonKinematicData(double aSimTime,
                                                WsfTrackManager& aTrackManager,
                                                WsfLocalTrack& aLocalTrack,
                                                const WsfTrack&  aNonLocalTrack);
   */
   RejectionRatios GetRejectionRatioMap() { return mRejectionRatios; }
   // void            ResetBlipPlatformPtrs();

   IdTrackIdList       mActiveRT_Tracks; // active target tracks that have been presented to the track manager
   IdTrackIdList       mBlockedRT_Tracks;
   IdToTrackIdMultiMap mScreenersRT_Tracks; // multi-map of aggregate target name Ids to tracks from the screeners
   IdVec               mScreenersRT_IdList; // list of target name Ids from the screeners.

   IdTrackIdList mActiveFT_Tracks;          // active false-target tracks that have been presented to the track manager
   IdToTrackIdMultiMap mScreenersFT_Tracks; // multi-map of aggregate of blip name Ids to false-target tracks from the screeners
   IdVec               mScreenersFT_IdList; // list of false-target blip name Ids from the screeners.

   UtCallbackHolder mCallbacks; // Construct to find out when a sensor turns on or off...

   bool                     mConsistencyChanged;
   bool                     mConsistencyConstrained; // tracks ar0e only reported if consistent across all screeners
   bool                     mDebugEnabled;
   InterScreenerConsistency mFalseTargetConsistency;     // container for denoting screener consistency for FTs
   InterScreenerConsistency mLastFalseTargetConsistency; // container for denoting screener consistency for FTs

   double mFrameTime; // fusion center frame time in seconds

   size_t mFusedFT_Number;   // dynamic number of false targets remaining following screener "algorithmics"
   size_t mFusedRT_Number;   // dynamic number of real targets remaining following screener "algorithmics"
   size_t mLastAggFT_Number; // aggregate false target number, used to determine if a state change has occurred
   size_t mLastAggRT_Number; // aggregate real target number, used to determine if a state change has occurred

   double       mLastUpdateTime; // last time a fusion center update occurred
   Output       mOutput;         // output type for the fusion center display
   unsigned int mPlotCapacity;   // number of targets which can be plotted on the radar scope
   bool mProduceFusedObjects;    // determines whether actual simulation platforms will be created for the false targets
   InterScreenerConsistency mRealTargetConsistency;     // container for denoting screener consistency for RTs
   InterScreenerConsistency mLastRealTargetConsistency; // container for denoting screener consistency for RTs
   RadarSensorIds     mRegisteredSites; // site/sensor key for radars that have successfully registered (i.e. are valid)
   RejectionRatios    mRejectionRatios; // FT rejection ratio based upon Appearance
   Screeners          mScreeners;       // vector of screeners reporting to the fusion center
   bool               mStateChange;     // boolean for denoting whether a state change has occurred
   unsigned int       mTrackCapacity;   // number of tracks which can be maintained by the radar
   SiteIds            mValidSites;      // vector of valid sites (i.e. radars)
   WsfFusionStrategy* mFusionStrategyPtr;
};

#endif
