//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfDefaultTrackExtrapolationStrategy.hpp"

#include "WsfDefaultKinematicStateExtrapolation.hpp"
#include "WsfMeasurement.hpp"
#include "WsfTrackExtrapolation.hpp"
#include "WsfTrackExtrapolators.hpp"
#include "WsfTypes.hpp"

wsf::DefaultTrackExtrapolationStrategy::DefaultTrackExtrapolationStrategy()
{
   RegisterExtrapolation(wsf::TrackExtrapolationIds::cDEFAULT_KINEMATIC_STATE_EXTRAPOLATION,
                         ut::make_unique<wsf::DefaultKinematicStateExtrapolation>());
}

void wsf::DefaultTrackExtrapolationStrategy::InitializeExtrapolation(const WsfMeasurement& aMeasurement,
                                                                     TrackExtrapolators&   aExtrapolators) const
{
   if (aMeasurement.GetSpatialDomain() == WSF_SPATIAL_DOMAIN_SPACE)
   {
      auto spaceExtrapolationIter =
         mPrototypeExtrapolators.find(wsf::TrackExtrapolationIds::cSPACE_KINEMATIC_STATE_EXTRAPOLATION);
      if (spaceExtrapolationIter == mPrototypeExtrapolators.end())
      {
         // The following condition would only occur in the unlikely event that the space library is missing,
         spaceExtrapolationIter =
            mPrototypeExtrapolators.find(wsf::TrackExtrapolationIds::cDEFAULT_KINEMATIC_STATE_EXTRAPOLATION);
      }
      aExtrapolators.Add(ut::clone(*spaceExtrapolationIter->second));
   }
   else
   {
      auto extrapolationIter =
         mPrototypeExtrapolators.find(wsf::TrackExtrapolationIds::cDEFAULT_KINEMATIC_STATE_EXTRAPOLATION);
      aExtrapolators.Add(ut::clone(*extrapolationIter->second));
   }
}
