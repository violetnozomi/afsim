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

#ifndef WSFCLUSTERCORRELATION_HPP
#define WSFCLUSTERCORRELATION_HPP

#include "wsf_mil_export.h"

#include <vector>

class UtInput;
class WsfClusterManager;
#include "WsfCorrelationStrategy.hpp"
#include "WsfEvent.hpp"

//! An implementation of WsfCorrelationStrategy for cluster correlation.
//! The WsfClusterCorrelation class uses clustering to determine how -
//! raw tracks should be correlated into local tracks [system tracks].
//! @see WsfCluster
//! @see WsfClusterManager
class WSF_MIL_EXPORT WsfClusterCorrelation : public WsfCorrelationStrategy
{
public:
   WsfClusterCorrelation(WsfScenario& aScenario);
   ~WsfClusterCorrelation() override;

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(WsfTrackManager* aTrackManagerPtr) override;

   WsfCorrelationStrategy* Clone() const override { return new WsfClusterCorrelation(*this); }

   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;

   void ClusterRawTracksIntoLocalTracks(double aSimTime);

   //! @name 'correlation / decorrelation' methods.
   //@{
   WsfLocalTrack* GetCorrelatedTrack(std::vector<WsfTrack*>& aCluster);
   WsfLocalTrack* GetCorrelatedTrack(const WsfTrackId& aNonLocalTrackId);
   void           MakeCorrelated(const WsfTrackId& aNonLocalTrackId, WsfLocalTrack& aLocalTrack);

   void MakeCorrelated(std::vector<WsfTrack*>& aCluster, WsfLocalTrack& aLocalTrack);

   void MakeDecorrelated(const WsfTrackId& aNonLocalTrackId, WsfLocalTrack& aLocalTrack);
   void MakeDecorrelated(WsfLocalTrack& aLocalTrack);

   void MakeDecorrelated(std::vector<WsfTrack*>& aCluster);

   //@}

   double GetMaxMeasurementError(double aSimTime, const WsfTrack& aTrack);
   double GetUpdateInterval() const { return mUpdateInterval; }
   double GetLastClusterUpdateTime() const { return mLastReclusterTime; }

private:
   WsfClusterManager*            mClusterManagerPtr;
   std::map<WsfStringId, double> mSensorIdToRangeMap;
   double                        mUpdateInterval;
   double                        mLastReclusterTime;

   class ClusterCorrelationEvent : public WsfEvent
   {
   public:
      ClusterCorrelationEvent(double aSimTime, WsfClusterCorrelation* aCorrelatorPtr)
         : WsfEvent(aSimTime)
         , mCorrelatorPtr(aCorrelatorPtr)
      {
      }
      EventDisposition Execute() override;

   private:
      WsfClusterCorrelation* mCorrelatorPtr;
   };
};

#endif
