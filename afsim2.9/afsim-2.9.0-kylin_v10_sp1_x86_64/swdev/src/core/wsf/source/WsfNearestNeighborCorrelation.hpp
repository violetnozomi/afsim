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

#ifndef WSFNEARESTNEIGHBORCORRELATION_HPP
#define WSFNEARESTNEIGHBORCORRELATION_HPP

#include "wsf_export.h"

#include <map>
#include <set>

class UtInput;
#include "WsfCorrelationStrategy.hpp"
#include "WsfCovariance.hpp"
#include "WsfTrackId.hpp"

//! An implementation of WsfCorrelationStrategy for which the spatially closest track is
//! selected as being associated with the measurement under consideration.
class WSF_EXPORT WsfNearestNeighborCorrelation : public WsfCorrelationStrategy
{
public:
   WsfNearestNeighborCorrelation();
   ~WsfNearestNeighborCorrelation() override = default;
   WsfCorrelationStrategy* Clone() const override;
   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;

   bool ProcessInput(UtInput& aInput) override;

   void Decorrelate(const WsfTrackId& aRawTrackId) override;

private:
   WsfLocalTrack* CorrelateWithConfidenceInterval(double             aSimTime,
                                                  const WsfTrack&    aNonLocalTrack,
                                                  WsfLocalTrackList& aTrackList,
                                                  double&            aSigmaConfidence,
                                                  WsfTrackId&        aExistingCorrelation);

   using CovarMap = std::map<WsfTrackId, WsfCovariance*>;

   std::map<WsfTrackId, double> mChangeMap;
   std::set<WsfTrackId>         mTurnSet;
   double                       mTrackingSigma;
   double                       mTurningSigma;
   double                       mChangeTime;
   bool                         mPreciseMode;
};

#endif
