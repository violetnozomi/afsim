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

#ifndef WSFTRACKREPORTINGSTRATEGY_HPP
#define WSFTRACKREPORTINGSTRATEGY_HPP

#include "wsf_export.h"

class WsfLocalTrack;
#include "WsfObject.hpp"
#include "WsfStringId.hpp"
class WsfTrack;
class WsfTrackProcessor;

//!   This class defines the interface for a track reporting strategy.
//!   It is to be used and configured by the WsfTrackProcessor.
//!   A factory method called WsfTrackReportingStrategy::Create is provided to return new instances of existing
//!   correlation object prototypes.
class WSF_EXPORT WsfTrackReportingStrategy : public WsfObject
{
public:
   WsfTrackReportingStrategy();
   ~WsfTrackReportingStrategy() override = default;

   //! Create a clone of "this" object.
   WsfTrackReportingStrategy* Clone() const override = 0; // pure virtual

   //! Perform any necessary initialization.
   virtual bool Initialize() { return true; };

   //! Report all local tracks.
   virtual void ReportFusedTracks(double aSimTime) = 0;
   virtual void ReportRawTracks(double aSimTime)   = 0;

   //! Return the track manager processor instance associated with this reporting strategy.
   WsfTrackProcessor& GetTrackProcessor() { return *mTrackProcessorPtr; }


   //! Set the track manager reference.
   void SetTrackProcessor(WsfTrackProcessor& aTrackProcessor) { mTrackProcessorPtr = &aTrackProcessor; }

private:
   WsfTrackProcessor* mTrackProcessorPtr;
};

#endif
