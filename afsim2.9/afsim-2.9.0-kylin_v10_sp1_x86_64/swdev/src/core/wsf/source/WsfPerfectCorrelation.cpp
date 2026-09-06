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

#include "WsfPerfectCorrelation.hpp"

#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"

// =================================================================================================
WsfPerfectCorrelation::WsfPerfectCorrelation()
   : WsfCorrelationStrategy()
{
}

// =================================================================================================
WsfPerfectCorrelation::~WsfPerfectCorrelation() {}

// =================================================================================================
// virtual
//! Given a non-local track update (or measurement), find the track in the track list
//! that correlates with the given track or measurement.
WsfLocalTrack* WsfPerfectCorrelation::CorrelateImpl(double             aSimTime,
                                                    const WsfTrack&    aNonLocalTrack,
                                                    WsfLocalTrackList& aTrackList)
{
   // Check if a correlation has already been made
   const WsfTrackId& correlatedLocalTrack = GetCorrelatedTrackId(aNonLocalTrack.GetTrackId());
   if (!correlatedLocalTrack.IsNull())
   {
      WsfLocalTrack* localTrackPtr = aTrackList.FindTrack(correlatedLocalTrack);
      if (localTrackPtr != nullptr)
      {
         return localTrackPtr;
      }
   }


   WsfLocalTrack* localTrackPtr    = WsfTrackManager::FindCorrelatedTrack(aNonLocalTrack.GetTrackId(), aTrackList);
   bool           localTrackExists = (localTrackPtr != nullptr);

   if ((!localTrackExists) && (aNonLocalTrack.GetTargetName() != 0))
   {
      for (unsigned int trackNum = 0; trackNum < aTrackList.GetTrackCount(); ++trackNum)
      {
         localTrackPtr = aTrackList.GetTrackEntry(trackNum);

         // The following line IS the correlation algorithm.  Simple, no?
         localTrackExists = (aNonLocalTrack.GetTargetName() != 0) &&
                            (aNonLocalTrack.GetTargetName() == localTrackPtr->GetTargetName()) &&
                            (localTrackPtr->IsFusible());

         if (localTrackExists)
         {
            break;
         }
      }
      if (localTrackExists && localTrackPtr->Correlate(aNonLocalTrack.GetTrackId()))
      {
         // Notify observers
         NotifyOfLocalTrackCorrelation(aSimTime, *localTrackPtr, aNonLocalTrack.GetTrackId());
         assert(localTrackPtr->IsCorrelatedWith(aNonLocalTrack.GetTrackId()));
      }
      else
      {
         localTrackPtr = nullptr;
      }
   }

   return localTrackPtr;
}
