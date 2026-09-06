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

#include "WsfCyclicTrackReporting.hpp"

#include "WsfEvent.hpp"
#include "WsfExternalLinks.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackProcessor.hpp"

namespace
{
template<class TrackListType>
class ReportingEvent : public WsfEvent
{
public:
   ReportingEvent() = delete;
   ReportingEvent(double aSimTime, WsfTrackReportingStrategy& aTrackReporting, TrackListType& aTrackList);
   ReportingEvent& operator=(const ReportingEvent& aSrc) = delete;
   ~ReportingEvent() override                            = default;
   WsfEvent::EventDisposition Execute() override;

private:
   void CalculateReportingInterval();
   void Update();

   WsfTrackReportingStrategy& mTrackReporting;
   TrackListType&             mTrackList;

   double       mTotalReportingInterval;
   unsigned int mTotalTracksToReport;

   double mTimeBetweenReports;
   int    mTracksLeftToReport;
   double mTimeLeftToReport;
   size_t mPlatformIndex;
};
} // namespace

typedef ReportingEvent<WsfTrackList>      RawReportingEvent;
typedef ReportingEvent<WsfLocalTrackList> FusedReportingEvent;

// virtual
WsfTrackReportingStrategy* WsfCyclicTrackReporting::Clone() const
{
   return new WsfCyclicTrackReporting(*this);
}

// protected
//! Send all fused tracks over external links.
void WsfCyclicTrackReporting::ReportFusedTracks(double aSimTime)
{
   GetTrackProcessor().GetSimulation()->AddEvent(
      ut::make_unique<FusedReportingEvent>(aSimTime, *this, GetTrackProcessor().GetTrackManager().GetTrackList()));
}

// protected
//! Send all raw tracks over external links.
void WsfCyclicTrackReporting::ReportRawTracks(double aSimTime)
{
   GetTrackProcessor().GetSimulation()->AddEvent(
      ut::make_unique<RawReportingEvent>(aSimTime, *this, GetTrackProcessor().GetTrackManager().GetRawTrackList()));
}

// Implementation of class ReportingEvent methods.
namespace
{
template<class TrackListType>
ReportingEvent<TrackListType>::ReportingEvent(double                     aSimTime,
                                              WsfTrackReportingStrategy& aTrackReporting,
                                              TrackListType&             aTrackList)
   : WsfEvent(aSimTime)
   , mTrackReporting(aTrackReporting)
   , mTrackList(aTrackList)
   , mTotalReportingInterval(aTrackReporting.GetTrackProcessor().GetReportingInterval())
   , mTotalTracksToReport(aTrackList.GetTrackCount())
   , mTimeBetweenReports(0.0)
   , mTracksLeftToReport(mTotalTracksToReport)
   , mTimeLeftToReport(mTotalReportingInterval)
   , mPlatformIndex(mTrackReporting.GetTrackProcessor().GetPlatform()->GetIndex())
{
   CalculateReportingInterval(); // i.e., the time between reports.
}

template<class TrackListType>
void ReportingEvent<TrackListType>::CalculateReportingInterval()
{
   mTimeBetweenReports = mTimeLeftToReport / (mTracksLeftToReport + 1);
}

template<class TrackListType>
void ReportingEvent<TrackListType>::Update()
{
   bool recalculate = false;
   if (mTrackList.GetTrackCount() != mTotalTracksToReport) // track list changed size.
   {
      recalculate = true;
      mTracksLeftToReport += (mTrackList.GetTrackCount() - mTotalTracksToReport);
      mTotalTracksToReport = mTrackList.GetTrackCount();
      if (mTracksLeftToReport < 0)
      {
         mTracksLeftToReport = 0;
      }
   }
   if (mTrackReporting.GetTrackProcessor().GetReportingInterval() != mTotalReportingInterval)
   {
      recalculate = true;
      mTimeLeftToReport += (mTrackReporting.GetTrackProcessor().GetReportingInterval() - mTotalReportingInterval);
      mTotalReportingInterval = mTrackReporting.GetTrackProcessor().GetReportingInterval();
      if (mTimeLeftToReport < 0.0)
      {
         mTimeLeftToReport = 0.0;
      }
   }
   if (recalculate)
   {
      CalculateReportingInterval();
   }
}

template<class TrackListType>
WsfEvent::EventDisposition ReportingEvent<TrackListType>::Execute()
{
   WsfEvent::EventDisposition disposition = WsfEvent::cDELETE;

   if (GetSimulation()->PlatformExists(mPlatformIndex))
   {
      Update();

      if (mTracksLeftToReport > 0)
      {
         // Send the track.
         mTrackReporting.GetTrackProcessor().SendTrack(GetTime(),
                                                       *mTrackList.GetTrackEntry(mTotalTracksToReport -
                                                                                 mTracksLeftToReport));

         if (mTracksLeftToReport > 1)
         {
            mTracksLeftToReport -= 1;
            mTimeLeftToReport -= mTimeBetweenReports;
            SetTime(GetTime() + mTimeBetweenReports);
            disposition = WsfEvent::cRESCHEDULE;
         }
      }
   }
   return disposition;
}
} // namespace
