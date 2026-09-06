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

#include "WsfPerfectTracker.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfUtil.hpp"

WsfPerfectTracker::WsfPerfectTracker(WsfScenario& aScenario)
   : WsfProcessor(aScenario)
   , mTgtNameId(nullptr)
   , mTargetIndex(0)
   , mChanged(false)
   , mTrackId()
   , mTrack()
   , mLastUpdate(0.0)
{
}


WsfPerfectTracker::~WsfPerfectTracker() {}


WsfPerfectTracker::WsfPerfectTracker(const WsfPerfectTracker& aSrc)
   : WsfProcessor(aSrc)
   , mTgtNameId(aSrc.mTgtNameId)
   , mTargetIndex(aSrc.mTargetIndex)
   , mChanged(aSrc.mChanged)
   , mTrackId(aSrc.mTrackId)
   , mTrack(aSrc.mTrack)
   , mLastUpdate(aSrc.mLastUpdate)
{
}


WsfProcessor* WsfPerfectTracker::Clone() const
{
   return new WsfPerfectTracker(*this);
}


/**
    Set the Entity desired to be tracked, using a Name ID.
    @note A NameID specification can lie dormant, and a track
          will be established after some time lapse, when the
          specified target enters the simulation.
*/
void WsfPerfectTracker::SetTargetByNameId(WsfStringId aNameId)
{
   if (aNameId != mTgtNameId)
   {
      mChanged     = true;
      mTgtNameId   = aNameId;
      mTargetIndex = 0;
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfPerfectTracker was requested to start tracking target.";
         out.AddNote() << "Platform: " << (GetPlatform() ? GetPlatform()->GetName() : "<unknown>");
         out.AddNote() << "Tracker: " << GetName();
         out.AddNote() << "Target: " << aNameId;
      }
   }
}


/**
    Set the Entity desired to be tracked, using a Platform Index.
    @note A PlatformID specification _must_ be valid at the next
          Update() call, or it will be ignored, and track will
          not be established.  User will then have to re-call
          this method again to pursue track on another target.
*/
void WsfPerfectTracker::SetTargetByIndex(size_t aPlatformIndex)
{
   if (aPlatformIndex != mTargetIndex)
   {
      mChanged     = true;
      mTargetIndex = aPlatformIndex;
      mTgtNameId   = WsfStringId();
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "WsfPerfectTracker was requested to start tracking a target.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         out.AddNote() << "Tracker: " << GetName();
         WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mTargetIndex);
         if (platformPtr == nullptr)
         {
            out.AddNote() << "Target: <unknown> (Index " << mTargetIndex << ")";
         }
         else
         {
            out.AddNote() << "Target: " << platformPtr->GetName();
         }
      }
   }
}


/**
    Clear the Entity ID so nothing is tracked.
*/
void WsfPerfectTracker::ClearTrackedTarget()
{
   SetTargetByIndex(0);
}


bool WsfPerfectTracker::Initialize2(double aSimTime)
{
   bool ok = WsfProcessor::Initialize2(aSimTime);

   // If this instance has nothing to track at initialization time,
   // it will attempt to get a target ID from the platform's current target.

   auto trackManagerPtr = &(GetPlatform()->GetTrackManager());

   if ((!mTgtNameId) && (mTargetIndex == 0) && (trackManagerPtr->GetCurrentTarget() != nullptr))
   {
      size_t tid = trackManagerPtr->GetCurrentTarget()->GetTargetIndex();
      SetTargetByIndex(tid);
   }
   return ok;
}


bool WsfPerfectTracker::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "tracked_platform")
   {
      std::string tgtName;
      aInput.ReadValue(tgtName);
      SetTargetByNameId(tgtName);
   }
   else
   {
      myCommand = WsfProcessor::ProcessInput(aInput);
   }
   return myCommand;
}


/** This virtual method does nothing in the base class,
    as it maintains a perfect track, but derived classes
    may wish to implement this method to alter the track
    in some way to degrade it.
*/
// virtual
void WsfPerfectTracker::DegradeTrack(double /*aSimTime*/)
{
   // Base method does nothing to alter a "perfect" track.
}


/** Remove the track of interest, and delete it
    from the platform's CurrentTrack consideration.
*/
void WsfPerfectTracker::NullTrack()
{
   if (!mTrackId.IsNull())
   {
      // Delete the old current target track out of the platform.
      GetPlatform()->GetMasterTrackList().DeleteTrack(mTrackId);
      mTrackId.Null();
   }
}


/** Begin a new track on a just-acquired entity. */
inline void WsfPerfectTracker::SetValidityFlags()
{
   mTrack.Set3D(true);
   mTrack.SetWCS_LocationValid(true);
   mTrack.SetVelocityValid(true);
   mTrack.SetRangeValid(true);
   mTrack.SetBearingValid(true);
   mTrack.SetElevationValid(true);
}


/** Begin a new track on a just-acquired entity. */
void WsfPerfectTracker::EstablishNewTrack(double aSimTime, WsfPlatform* aTgtPtr)
{
   // Assure the position is current:
   aTgtPtr->Update(aSimTime);

   // Now get the next sequential ID for this new track:
   mTrackId = GetPlatform()->GetNextTrackId();
   mTrack.Initialize(aSimTime, mTrackId, *GetSimulation());
   mTrack.SetTargetIndex(mTargetIndex);

   double myLocWCS[3];
   GetPlatform()->GetLocationWCS(myLocWCS);
   mTrack.SetOriginatorLocationWCS(myLocWCS);

   double tgtLocWCS[3];
   aTgtPtr->GetLocationWCS(tgtLocWCS);
   mTrack.SetLocationWCS(tgtLocWCS);

   double tgtVelWCS[3];
   aTgtPtr->GetVelocityWCS(tgtVelWCS);
   mTrack.SetVelocityWCS(tgtVelWCS);

   // In theory one only needs location, but the previous form also had range. Unfortunately it did not include bearing
   // and elevation, and range without bearing and elevation doesn't give location. So we are just making it consistent.
   // (Technically we should just be able to report L, but I didn't want to mess up anyone who is also counting on R.)

   double range;
   double bearing;
   double elevation;
   UtMeasurementUtil::RangeBearingElevation(myLocWCS, tgtLocWCS, range, bearing, elevation);
   mTrack.SetRange(range);
   mTrack.SetBearing(bearing);
   mTrack.SetElevation(elevation);

   SetValidityFlags();

   // A virtual method with null implementation in the base class.
   DegradeTrack(aSimTime);

   // Assign this track to CurrentTarget:
   GetPlatform()->GetTrackManager().SetCurrentTarget(aSimTime, mTrack);

   mChanged = false;

   if (DebugEnabled())
   {
      auto logDebug = ut::log::debug() << "WsfPerfectTracker initiated a true track.";
      logDebug.AddNote() << "T = " << aSimTime;
      logDebug.AddNote() << "Platform: " << GetPlatform()->GetName();
      logDebug.AddNote() << "Tracker: " << GetName();
      logDebug.AddNote() << "Target: " << aTgtPtr->GetName();
   }
}


/** Refresh an existing track with new information. */
void WsfPerfectTracker::UpdateExistingTrack(double aSimTime, WsfPlatform* aTgtPtr)
{
   // Assure the position(s), velocity, and range are current:
   aTgtPtr->Update(aSimTime);

   double myLocWCS[3];
   GetPlatform()->GetLocationWCS(myLocWCS);
   mTrack.SetOriginatorLocationWCS(myLocWCS);

   double tgtLocWCS[3];
   aTgtPtr->GetLocationWCS(tgtLocWCS);
   mTrack.SetLocationWCS(tgtLocWCS);

   double tgtVelWCS[3];
   aTgtPtr->GetVelocityWCS(tgtVelWCS);
   mTrack.SetVelocityWCS(tgtVelWCS);

   // SEE NOTE IN EstablishNewTrack (above).

   double range;
   double bearing;
   double elevation;
   UtMeasurementUtil::RangeBearingElevation(myLocWCS, tgtLocWCS, range, bearing, elevation);
   mTrack.SetRange(range);
   mTrack.SetBearing(bearing);
   mTrack.SetElevation(elevation);

   // A virtual method with null implementation in the base class.
   DegradeTrack(aSimTime);

   // Update the track:
   mTrack.Update(aSimTime);

   // Assign this updated track to the CurrentTarget:
   GetPlatform()->GetTrackManager().SetCurrentTarget(aSimTime, mTrack);
}


void WsfPerfectTracker::Update(double aSimTime)
{
   double dt = aSimTime - mLastUpdate;

   if (dt > 0.0)
   {
      if (mChanged)
      {
         NullTrack();

         // May have set the target by either NameId or PlatformId...
         if (mTgtNameId.IsNull())
         {
            if (mTargetIndex == 0)
            {
               // Not pursuing any track at the moment, do nothing.
            }
            else
            {
               // User set a PlatformID, so get a corresponding new NameId
               WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(mTargetIndex);
               if (tgtPtr == nullptr)
               {
                  // Supplied PlatformID is not valid, so ignore it.
                  mTargetIndex = 0;
               }
               else
               {
                  mTgtNameId = tgtPtr->GetNameId();
                  // We have a valid target specification, establish a track on it.
                  EstablishNewTrack(aSimTime, tgtPtr);
               }
            }
         }
         else
         {
            // User set a Target Name ID, so get a corresponding PlatformID
            WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByName(mTgtNameId);
            if (tgtPtr == nullptr)
            {
               // Supplied target name is not yet in the simulation,
               // so do nothing, just wait for target to appear or
               // for the designated target to get changed.
            }
            else
            {
               mTargetIndex = tgtPtr->GetIndex();
               // We have a valid target specification, establish a track on it.
               EstablishNewTrack(aSimTime, tgtPtr);
            }
         }
      }
      else if (mTargetIndex == 0)
      {
         // No track, do nothing
      }
      else
      {
         // Not changed, continue with existing track.
         WsfPlatform* tgtPtr = GetSimulation()->GetPlatformByIndex(mTargetIndex);
         if (tgtPtr == nullptr)
         {
            // Tracked target went away.  Null the track, and set ID to "nothing"
            NullTrack();
            mTargetIndex = 0;
            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "WsfPerfectTracker lost track on target.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << GetPlatform()->GetName();
               out.AddNote() << "Tracker: " << GetName();
               if (mTgtNameId != 0)
               {
                  out.AddNote() << "Target: " << mTgtNameId;
               }
               out.AddNote() << "Track lost because target went away.";
            }
            mTgtNameId = WsfStringId();
         }
         else
         {
            UpdateExistingTrack(aSimTime, tgtPtr);
         }

      } // end if (mChanged) block

      mLastUpdate = aSimTime;

   } // end if (dt > 0)
}
