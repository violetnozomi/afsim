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

#ifndef WSFTHREATPROCESSOR_HPP
#define WSFTHREATPROCESSOR_HPP

#include "wsf_mil_export.h"

#include <set>

#include "script/WsfScriptProcessor.hpp"
class WsfLocalTrack;
class WsfScript;
class WsfScriptContext;
class WsfTrack;
#include "WsfTrackId.hpp"
#include "WsfTrackList.hpp"

//! This is a processor that attempts to identify threatening tracks from the
//! master track list and then adds them to a threat list.  Threatening tracks
//! are typically going to be those on a collision course for the platform
//! containing the WsfThreatProcessor.

class WSF_MIL_EXPORT WsfThreatProcessor : public WsfScriptProcessor
{
public:
   WsfThreatProcessor(WsfScenario& aScenario);
   WsfThreatProcessor(WsfScenario& aScenario, const std::string& aPartClass, const std::string& aPartVarName);
   WsfThreatProcessor& operator=(const WsfThreatProcessor&) = delete;
   ~WsfThreatProcessor() override                           = default;
   WsfProcessor* Clone() const override;

   bool Initialize(double aSimTime) override;
   void Update(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;

   void Enable() { mEnabled = true; }
   void Disable() { mEnabled = false; }

   std::set<WsfTrackId>* GetThreatList();
   WsfTrack*             GetNearestThreat();

   void AddThreat(WsfTrack* aTrackPtr);
   void RemoveThreat(WsfTrack* aTrackPtr);

   const char* GetScriptClassName() const override { return "WsfThreatProcessor"; }

   bool IsThreatening(WsfTrack* aTrackPtr, bool aKnown = false);

protected:
   WsfThreatProcessor(const WsfThreatProcessor& aSrc);

   //! Executes the script
   virtual void IdentifiedNewThreat(double aSimTime, const WsfTrack* aTrackPtr);

   //! Executes the script
   virtual void DroppedThreat(double aSimTime, const WsfTrack* aTrackPtr);

private:
   void UpdateThreatList(double aSimTime);
   bool IsKnown(WsfTrack* aTrackPtr);
   void RemoveDroppedTracks(double aSimTime);

   UtScript* mIdentifiedNewThreatScriptPtr;
   UtScript* mDroppedThreatScriptPtr;

   std::set<WsfTrackId> mThreatList;
   bool                 mEnabled;
   double               mThreatVelocity;    // the velocity above which a track is considered threatening
   double               mThreatAngleSpread; // the relative heading of the threat to you
   // required (or smaller) for something to be threatening
   double mThreatTimeToIntercept; // the distance/vel threshold below which
   // a track may be considered to be threatening
   bool mRequireIFF_FOE;   // require an IFF_FOE for threat
   bool mIgnoreLowerAlts;  // prunes out all threats at lower altitude
   bool mIgnoreNoLocation; // prune out threats that lack location data
   bool mIgnoreNoVelocity; // prune out threats that lack velocity data

   UtScriptClass* mTrackClassPtr;
};

#endif
