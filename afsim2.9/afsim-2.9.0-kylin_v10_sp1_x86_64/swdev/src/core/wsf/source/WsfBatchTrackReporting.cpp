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

#include "WsfBatchTrackReporting.hpp"

#include "WsfExternalLinks.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackProcessor.hpp"

// virtual
WsfTrackReportingStrategy* WsfBatchTrackReporting::Clone() const
{
   return new WsfBatchTrackReporting(*this);
}

// protected
//! Send all fused tracks over external links.
void WsfBatchTrackReporting::ReportFusedTracks(double aSimTime)
{
   WsfTrackManager&   trackManager  = GetTrackProcessor().GetTrackManager();
   wsf::ExternalLinks externalLinks = GetTrackProcessor().GetExternalLinks();

   unsigned int trackCount = trackManager.GetTrackCount();
   if (externalLinks.HasLinks() && (trackCount != 0))
   {
      for (unsigned int trackIndex = 0; trackIndex < trackCount; ++trackIndex)
      {
         // Force the type to be WsfTrack; this is considered a non-local track.
         WsfLocalTrack* trackPtr = trackManager.GetTrackEntry(trackIndex);
         GetTrackProcessor().SendTrack(aSimTime, *trackPtr);
      }
   }
}

// protected
//! Send all raw tracks over external links.
void WsfBatchTrackReporting::ReportRawTracks(double aSimTime)
{
   WsfTrackList&      rawTrackList  = GetTrackProcessor().GetTrackManager().GetRawTrackList();
   wsf::ExternalLinks externalLinks = GetTrackProcessor().GetExternalLinks();

   if (externalLinks.HasLinks() && (rawTrackList.GetTrackCount() > 0))
   {
      for (unsigned int trackIndex = 0; trackIndex < rawTrackList.GetTrackCount(); ++trackIndex)
      {
         WsfTrack* trackPtr = rawTrackList.GetTrackEntry(trackIndex);
         GetTrackProcessor().SendTrack(aSimTime, *trackPtr);
      }
   }
}
