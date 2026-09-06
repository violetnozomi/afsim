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

#ifndef WSFTRUTHCORRELATION_HPP
#define WSFTRUTHCORRELATION_HPP

#include "wsf_export.h"

#include <map>

#include "UtCallbackHolder.hpp"
class UtInput;
#include "WsfCorrelationStrategy.hpp"
class WsfPlatform;
#include "WsfTrackId.hpp"

//! An implementation of WsfCorrelationStrategy for correlation with an existing truth object.
//! This correlation strategy is useful for correlation with tracks that don't have a truth id,
//! such as from an external common operational picture (C2 feed).
class WSF_EXPORT WsfTruthCorrelation : public WsfCorrelationStrategy
{
public:
   WsfTruthCorrelation();
   ~WsfTruthCorrelation() override;

   WsfCorrelationStrategy* Clone() const override { return new WsfTruthCorrelation(*this); }

   bool ProcessInput(UtInput& aInput) override;

   bool Initialize(WsfTrackManager* aTrackManagerPtr) override;

   WsfLocalTrack* CorrelateImpl(double aSimTime, const WsfTrack& aNonLocalTrack, WsfLocalTrackList& aTrackList) override;

private:
   void LocalTrackCorrelation(double aSimTime, const WsfLocalTrack* aLocalTrackPtr, const WsfTrackId& aNonLocalTrackId);

   WsfPlatform* FindClosestPlatform(double aSimTime, const WsfTrack& aNonLocalTrack);

   std::map<WsfTrackId, double> mChangeMap;
   std::map<WsfTrackId, size_t> mTruthMap;

   double           mChangeTime;
   double           mMaxDistanceForCorrelation;
   bool             mIgnoreTrackTarget;
   bool             mIgnoreSameSide;
   UtCallbackHolder mCallbacks;
};

#endif
