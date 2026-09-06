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

#include "WsfThreatProcessor.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <iostream>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtMath.hpp"
#include "UtScriptRef.hpp"
#include "UtVec3.hpp"
#include "WsfEvent.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMilComponentRoles.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackDropMessage.hpp"
#include "WsfTrackList.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfUtil.hpp"
#include "script/WsfScriptContext.hpp"

//! The default constructor
WsfThreatProcessor::WsfThreatProcessor(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfThreatProcessor", "PROCESSOR")
   , mIdentifiedNewThreatScriptPtr(nullptr)
   , mDroppedThreatScriptPtr(nullptr)
   , mThreatList()
   , mEnabled(true)
   , mThreatVelocity(600.0)
   , mThreatAngleSpread(UtMath::cPI / 6.0)
   , mThreatTimeToIntercept(60.0)
   , mRequireIFF_FOE(false)
   , mIgnoreLowerAlts(false)
   , mIgnoreNoLocation(true)
   , mIgnoreNoVelocity(true)
   , mTrackClassPtr(nullptr)
{
}

WsfThreatProcessor::WsfThreatProcessor(WsfScenario& aScenario, const std::string& aPartClass, const std::string& aPartVarName)
   : WsfScriptProcessor(aScenario, aPartClass, aPartVarName)
   , mIdentifiedNewThreatScriptPtr(nullptr)
   , mDroppedThreatScriptPtr(nullptr)
   , mThreatList()
   , mEnabled(true)
   , mThreatVelocity(600.0)
   , mThreatAngleSpread(UtMath::cPI / 6.0)
   , mThreatTimeToIntercept(60.0)
   , mRequireIFF_FOE(false)
   , mIgnoreLowerAlts(false)
   , mIgnoreNoLocation(true)
   , mIgnoreNoVelocity(true)
   , mTrackClassPtr(nullptr)
{
}

WsfThreatProcessor::WsfThreatProcessor(const WsfThreatProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mIdentifiedNewThreatScriptPtr(nullptr)
   , mDroppedThreatScriptPtr(nullptr)
   , mThreatList()
   , mEnabled(aSrc.mEnabled)
   , mThreatVelocity(aSrc.mThreatVelocity)
   , mThreatAngleSpread(aSrc.mThreatAngleSpread)
   , mThreatTimeToIntercept(aSrc.mThreatTimeToIntercept)
   , mRequireIFF_FOE(aSrc.mRequireIFF_FOE)
   , mIgnoreLowerAlts(aSrc.mIgnoreLowerAlts)
   , mIgnoreNoLocation(aSrc.mIgnoreNoLocation)
   , mIgnoreNoVelocity(aSrc.mIgnoreNoVelocity)
   , mTrackClassPtr(aSrc.mTrackClassPtr)
{
   mIdentifiedNewThreatScriptPtr = mContextPtr->FindScript("identified_new_threat");
   mDroppedThreatScriptPtr       = mContextPtr->FindScript("dropped_threat");
}

// virtual
WsfProcessor* WsfThreatProcessor::Clone() const
{
   return new WsfThreatProcessor(*this);
}

// virtual
bool WsfThreatProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   mIdentifiedNewThreatScriptPtr = mContextPtr->FindScript("identified_new_threat");
   mDroppedThreatScriptPtr       = mContextPtr->FindScript("dropped_threat");

   mTrackClassPtr = mContextPtr->GetClass("WsfTrack");
   ok &= (mTrackClassPtr != nullptr);

   return ok;
}

// virtual
bool WsfThreatProcessor::ProcessInput(UtInput& aInput)
{
   bool        wasProcessed = true;
   std::string command(aInput.GetCommand());
   if (command == "threat_velocity")
   {
      aInput.ReadValueOfType(mThreatVelocity, UtInput::cSPEED);
   }
   else if (command == "threat_angle_spread")
   {
      aInput.ReadValueOfType(mThreatAngleSpread, UtInput::cANGLE);
   }
   else if (command == "threat_time_to_intercept")
   {
      aInput.ReadValueOfType(mThreatTimeToIntercept, UtInput::cTIME);
   }
   else if (command == "require_iff_foe")
   {
      aInput.ReadValue(mRequireIFF_FOE);
   }
   else if (command == "ignore_lower_altitude_threats")
   {
      aInput.ReadValue(mIgnoreLowerAlts);
   }
   else if (command == "ignore_without_location")
   {
      aInput.ReadValue(mIgnoreNoLocation);
   }
   else if (command == "ignore_without_velocity")
   {
      aInput.ReadValue(mIgnoreNoVelocity);
   }
   else if (WsfScriptProcessor::ProcessInput(aInput))
   {
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      wasProcessed = false;
   }
   return wasProcessed;
}

// virtual public
//! Executes the 'identified_new_threat' script.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr A pointer to a track.
void WsfThreatProcessor::IdentifiedNewThreat(double aSimTime, const WsfTrack* aTrackPtr)
{
   if (mIdentifiedNewThreatScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      // There is one argument; the track
      scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfTrack*>(aTrackPtr), mTrackClassPtr)));

      mContextPtr->ExecuteScript(aSimTime, mIdentifiedNewThreatScriptPtr, scriptRetVal, scriptArgs);
      // isReportable = scriptRetVal.GetBool();
   }

   return;
}

// virtual public
//! Executes the 'dropped_threat' script.
//! @param aSimTime  The current simulation time.
//! @param aTrackPtr A pointer to a track.
void WsfThreatProcessor::DroppedThreat(double aSimTime, const WsfTrack* aTrackPtr)
{
   if (mDroppedThreatScriptPtr != nullptr)
   {
      UtScriptData     scriptRetVal;
      UtScriptDataList scriptArgs;

      // There is one argument; the track
      scriptArgs.push_back(UtScriptData(new UtScriptRef(const_cast<WsfTrack*>(aTrackPtr), mTrackClassPtr)));

      mContextPtr->ExecuteScript(aSimTime, mDroppedThreatScriptPtr, scriptRetVal, scriptArgs);
      // isReportable = scriptRetVal.GetBool();
   }

   return;
}

// default: bool aKnown = false
bool WsfThreatProcessor::IsThreatening(WsfTrack* aTrack, bool aKnown)
{
   if (mRequireIFF_FOE && (aTrack->GetIFF_Status() != WsfTrack::cIFF_FOE))
   {
      if (this->DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfThreatProcessor says target is not threatening. Not an IFF FOE.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Target: " << aTrack->GetTargetName();
      }
      return false;
   }

   // cheating, but let's ignore stuff that isn't moving
   WsfPlatform* tempPlatPtr = GetSimulation()->GetPlatformByIndex(aTrack->GetTargetIndex());
   if (tempPlatPtr == nullptr || tempPlatPtr->IsStopped())
   {
      if (this->DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfThreatProcessor says target is not threatening. Not moving.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Target: " << aTrack->GetTargetName();
      }
      return false;
   }

   // check velocity, relative bearing, & time to intercept
   if (aTrack->VelocityValid())
   {
      // track velocity isn't always right
      // good, that is called "operating on perception", no cheating please
      double wcsLoc[3];
      double wcsVel[3];
      double nedVel[3];
      aTrack->GetVelocityWCS(wcsVel);
      double   vel = UtVec3d::Magnitude(wcsVel);
      UtEntity tempEnt;
      aTrack->GetLocationWCS(wcsLoc);
      tempEnt.SetLocationWCS(wcsLoc);
      tempEnt.SetVelocityWCS(wcsVel); // useful later when checking relative bearings

      tempEnt.ConvertWCSVectorToNED(nedVel, wcsVel);

      // velocity check, is track threatening?
      if ((vel < mThreatVelocity && !aKnown) ||
          // if the threat is already known, then make it harder to get marked as non-threatening
          (vel < mThreatVelocity / 2 && aKnown))
      {
         if (this->DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfThreatProcessor says target is not threatening. Not fast enough.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Target: " << aTrack->GetTargetName();
         }
         return false;
      }

      // bearing check, is track threatening?
      double myWCSLoc[3] = {0};
      GetPlatform()->GetLocationWCS(myWCSLoc);

      // Get the true bearing from threat to me
      double targetLocNED[3];
      tempEnt.ConvertWCSToNED(myWCSLoc, targetLocNED);
      double trueBearing = atan2(targetLocNED[1], targetLocNED[0]); // from threat to me
      // Adjust by the heading of the threat
      double threatBearing   = atan2(nedVel[1], nedVel[0]); // threat bearing
      double relativeBearing = fabs(UtMath::NormalizeAngleMinusPi_Pi(trueBearing - threatBearing));

      // if the threat is already known, then make it harder to get marked as non-threatening
      if ((relativeBearing > mThreatAngleSpread && !aKnown) || (relativeBearing > (mThreatAngleSpread * 2) && aKnown))
      {
         if (this->DebugEnabled())
         {
            auto out =
               ut::log::debug() << "WsfThreatProcessor says target is not threatening. Not angled towards us enough.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Target: " << aTrack->GetTargetName();
         }
         return false;
      }

      // time to intercept check, is track threatening?
      if (aTrack->LocationValid())
      {
         WsfPlatform* plat       = GetPlatform();
         double       slantRange = WsfUtil::SlantRange(*plat, *aTrack);

         if (slantRange / vel > mThreatTimeToIntercept)
         {
            if (this->DebugEnabled())
            {
               auto out = ut::log::debug()
                          << "WsfThreatProcessor says target is not threatening. Not going to intercept soon enough.";
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Target: " << aTrack->GetTargetName();
            }
            return false;
         }

         double lat = 0;
         double lon = 0;
         double alt = 0;
         aTrack->GetLocationLLA(lat, lon, alt);
         double myLat = 0;
         double myLon = 0;
         double myAlt = 0;
         GetPlatform()->GetLocationLLA(myLat, myLon, myAlt);
         if (mIgnoreLowerAlts && (nedVel[2] < 0) && (alt < myAlt))
         {
            if (this->DebugEnabled())
            {
               auto out = ut::log::debug()
                          << "WsfThreatProcessor says target is not threatening. Lower altitude, and headed down.";
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Target: " << aTrack->GetTargetName();
            }
            return false;
         }
      }
      else if (mIgnoreNoLocation == true)
      {
         if (this->DebugEnabled())
         {
            auto out = ut::log::debug() << "WsfThreatProcessor says target is not threatening. Location not valid.";
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Target: " << aTrack->GetTargetName();
         }
         return false;
      }
   }
   else if (mIgnoreNoVelocity == true)
   {
      if (this->DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfThreatProcessor says target is not threatening. Velocity not valid.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Target: " << aTrack->GetTargetName();
      }
      return false;
   }

   return true;
}

bool WsfThreatProcessor::IsKnown(WsfTrack* aTrackPtr)
{
   if (mThreatList.find(aTrackPtr->GetTrackId()) != mThreatList.end())
   {
      return true;
   }
   return false;
}

void WsfThreatProcessor::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime);
   UpdateThreatList(aSimTime);
}

void WsfThreatProcessor::RemoveDroppedTracks(double aSimTime)
{
   WsfLocalTrackList*             mstPtr = &(GetPlatform()->GetMasterTrackList());
   std::set<WsfTrackId>::iterator iter   = mThreatList.begin();

   while (iter != mThreatList.end())
   {
      if (mstPtr->FindTrack((*iter)) == nullptr)
      {
         // if we erase, update iter with the next item in the set after the element erased
         std::set<WsfTrackId>::iterator thisIter = iter;
         ++iter;
         mThreatList.erase(thisIter);
         DroppedThreat(aSimTime, nullptr);
      }
      else
      {
         // if we don't erase, increment the iterator
         ++iter;
      }
   }
}

void WsfThreatProcessor::RemoveThreat(WsfTrack* aTrackPtr)
{
   if (mThreatList.find(aTrackPtr->GetTrackId()) != mThreatList.end())
   {
      mThreatList.erase(aTrackPtr->GetTrackId());
      DroppedThreat(GetSimulation()->GetSimTime(), aTrackPtr);
      return;
   }
}

void WsfThreatProcessor::AddThreat(WsfTrack* aTrackPtr)
{
   std::pair<std::set<WsfTrackId>::iterator, bool> pr = mThreatList.insert(aTrackPtr->GetTrackId());
   if (pr.second)
   {
      IdentifiedNewThreat(GetSimulation()->GetSimTime(), aTrackPtr);
   }
}

void WsfThreatProcessor::UpdateThreatList(double aSimTime)
{
   WsfLocalTrackList* mstPtr = &(GetPlatform()->GetMasterTrackList());

   for (unsigned int i = 0; i < mstPtr->GetTrackCount(); ++i)
   {
      WsfLocalTrack* ltPtr       = mstPtr->GetTrackEntry(i);
      bool           known       = IsKnown(ltPtr);
      bool           threatening = IsThreatening(ltPtr, known);

      if (!known && threatening)
      {
         AddThreat(ltPtr);
      }
      else if (known && !threatening)
      {
         RemoveThreat(ltPtr);
      }
   }

   RemoveDroppedTracks(aSimTime);
}

std::set<WsfTrackId>* WsfThreatProcessor::GetThreatList()
{
   UpdateThreatList(GetSimulation()->GetSimTime());
   return &mThreatList;
}

WsfTrack* WsfThreatProcessor::GetNearestThreat()
{
   WsfLocalTrack*        nearestTrackPtr = nullptr;
   std::set<WsfTrackId>* threatsPtr      = GetThreatList();
   if (!threatsPtr->empty())
   {
      double minDistance    = 1.0e+200; // Large initial distance (meters)
      double platformWCS[3] = {0};      // Location of the platform this processor is attached to
      double threatWCS[3]   = {0};
      double deltaLocWCS[3] = {0};
      GetPlatform()->GetLocationWCS(platformWCS);

      std::set<WsfTrackId>::iterator it = threatsPtr->begin();
      for (; it != threatsPtr->end(); ++it)
      {
         WsfLocalTrackList* mtlPtr   = &(GetPlatform()->GetMasterTrackList());
         WsfLocalTrack*     trackPtr = mtlPtr->FindTrack(*it);
         if (trackPtr->LocationValid())
         {
            // Track has 3D location, can compute range
            trackPtr->GetExtrapolatedLocationWCS(GetSimulation()->GetSimTime(), threatWCS);
            UtVec3d::Subtract(deltaLocWCS, platformWCS, threatWCS);
            // Don't need actual range, avoid sqrt()
            double distanceSquared = UtVec3d::MagnitudeSquared(deltaLocWCS);
            if (distanceSquared < minDistance)
            {
               minDistance     = distanceSquared;
               nearestTrackPtr = trackPtr;
            }
         }
      }
   }
   // return zero/null if no threats
   return nearestTrackPtr;
}
