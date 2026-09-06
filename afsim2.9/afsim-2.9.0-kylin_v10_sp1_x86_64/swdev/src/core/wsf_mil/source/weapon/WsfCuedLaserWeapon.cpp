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

#include "WsfCuedLaserWeapon.hpp"

#include <cassert>
#include <iostream>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtInputString.hpp"
#include "UtMath.hpp"
#include "UtVec3.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfException.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfUtil.hpp"

WsfCuedLaserWeapon::WsfCuedLaserWeapon(WsfScenario& aScenario)
   : WsfLaserWeapon(aScenario)
   , mBeamDirectorPtr(nullptr)
   , mBeamDirectorName()
   , mBeamDirectorTrack()
   , mCurrentTargetTrack()
   , mBeamDirectorInputs()
   , mBeamDirector()
{
}

WsfCuedLaserWeapon::WsfCuedLaserWeapon(const WsfCuedLaserWeapon& aSrc)
   : WsfLaserWeapon(aSrc)
   , mBeamDirectorPtr(nullptr)
   , mBeamDirectorName(aSrc.mBeamDirectorName)
   , mBeamDirectorTrack(aSrc.mBeamDirectorTrack)
   , mCurrentTargetTrack(aSrc.mCurrentTargetTrack)
   , mBeamDirectorInputs(aSrc.mBeamDirectorInputs)
   , mBeamDirector(aSrc.mBeamDirector)
{
}

// virtual
WsfWeapon* WsfCuedLaserWeapon::Clone() const
{
   return new WsfCuedLaserWeapon(*this);
}

// protected
bool WsfCuedLaserWeapon::HasCurrentEngagement() const
{
   return (GetCurrentEngagement() != nullptr);
}

// virtual
bool WsfCuedLaserWeapon::ProcessInput(UtInput& aInput)
{
   bool               ok      = true;
   const std::string& command = aInput.GetCommand();

   if (command == "beam_director")
   {
      std::string beamDirector;
      aInput.ReadValue(beamDirector);
      mBeamDirector.emplace(beamDirector);
   }
   else if (command == "use_default_beam_director")
   {
      mBeamDirector.reset();
   }
   else
   {
      ok = WsfLaserWeapon::ProcessInput(aInput);
   }

   return ok;
}

// virtual
bool WsfCuedLaserWeapon::Initialize(double aSimTime)
{
   bool ok = WsfLaserWeapon::Initialize(aSimTime);
   if (ok)
   {
      // The weapon is always turned on so it can receive messages from the beam director.
      GetSimulation()->TurnPartOn(aSimTime, this);
   }
   else
   {
      auto out = ut::log::info() << "WsfCuedLaserWeapon: No beam director found.";
      out.AddNote()
         << "Configure HEL platform with a BEAM_DIRECTOR sensor definition, or use SIMPLE_LASER_WEAPON instead.";
   }
   return ok;
}

// virtual
bool WsfCuedLaserWeapon::Initialize2(double aSimTime)
{
   // Find the beam director
   bool beamDirectorFound = false;
   for (WsfComponentList::RoleIterator<WsfSensor> iter(*GetPlatform()); !iter.AtEnd(); ++iter)
   {
      mBeamDirectorPtr = dynamic_cast<WsfBeamDirector*>(*iter);

      if (mBeamDirectorPtr != nullptr)
      {
         if (!mBeamDirector.has_value() || mBeamDirectorPtr->GetName() == mBeamDirector.value())
         {
            mBeamDirectorPtr->GetInternalLinks().AddRecipient(this);
            beamDirectorFound = true;

            if (DebugEnabled())
            {
               auto out = ut::log::debug() << "WsfCuedLaserWeapon linked with WsfBeamDirector";
               out.AddNote() << "Weapon: " << GetName();
               out.AddNote() << "Beam Director: " << mBeamDirectorPtr->GetName();
            }

            break;
         }
      }
   }

   if (DebugEnabled() && beamDirectorFound)
   {
      auto out = ut::log::debug() << "Internal Links:";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Weapon: " << GetName();
      auto note = out.AddNote() << "Recipient:";
      for (unsigned i = 0; i < mBeamDirectorPtr->GetInternalLinks().GetRecipientCount(); ++i)
      {
         note.AddNote() << mBeamDirectorPtr->GetInternalLinks().GetRecipientEntry(i)->GetName();
      }
   }

   if (mBeamDirector.has_value() && !beamDirectorFound)
   {
      auto out = ut::log::error() << "WsfBeamDirector specified for WsfCuedLaserWeapon not found on platform";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "WsfCuedLaserWeapon: " << GetName();
      out.AddNote() << "Desired Beam Director: " << mBeamDirector.value();
   }

   return beamDirectorFound;
}

// virtual
void WsfCuedLaserWeapon::TurnOff(double aSimTime)
{
   mState = cNOT_READY_TO_FIRE;
   mBeamDirectorPtr->TurnOff(aSimTime);
   WsfWeapon::TurnOff(aSimTime);
}

// virtual protected
bool WsfCuedLaserWeapon::EndShot(double aSimTime)
{
   bool ended = WsfLaserWeapon::EndShot(aSimTime);
   return ended;
}

//! Private
//! Fire at a track, assuming that the beam director already has an actionable track.
void WsfCuedLaserWeapon::UpdateEngagement(double aSimTime)
{
   // Decision constraints to fire:
   if ((GetState() == cREADY_TO_FIRE) || (GetState() == cPAUSED))
   {
      // Update the track.
      mBeamDirectorTrack.Update(aSimTime);

      // Make sure the target still exists.
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByName(mBeamDirectorTrack.GetTargetName());
      if (targetPtr != nullptr)
      {
         std::string targetOffset;
         if (mBeamDirectorTrack.GetAuxDataConst().AttributeExists("TARGET_OFFSET"))
         {
            targetOffset = mBeamDirectorTrack.GetAuxDataConst().GetString("TARGET_OFFSET");
         }

         Update(aSimTime);
         bool isFiring = BeginShot(aSimTime);

         if (isFiring && DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: Firing HEL at target.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
            out.AddNote() << "Target: " << targetPtr->GetName();
            out.AddNote() << "Track Target: " << mBeamDirectorTrack.GetTargetName();
            if (!targetOffset.empty())
            {
               out.AddNote() << "Offset: " << targetOffset;
            }
         }
      }
   }
}

void WsfCuedLaserWeapon::PauseFire(double aSimTime)
{
   if (mState == cFIRING)
   {
      EndShot(aSimTime);
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "HEL: Pause Fire.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      mState = cPAUSED;
   }
}

void WsfCuedLaserWeapon::ResumeFire(double aSimTime)
{
   if (mState == cPAUSED)
   {
      if (CanLase(aSimTime))
      {
         BeginShot(aSimTime);
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: Resume Fire.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
      else
      {
         mState = cREADY_TO_FIRE;
      }
   }
}

// virtual
void WsfCuedLaserWeapon::CeaseFire(double aSimTime)
{
   WsfLaserWeapon::CeaseFire(aSimTime);
   GetPlatform()->GetTrackManager().ClearCurrentTarget();
}

bool WsfCuedLaserWeapon::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   WsfStringId typeId    = aMessage.GetType();
   bool        processed = (GetEngagement() != nullptr);
   if (processed)
   {
      if (typeId == WsfTrackMessage::GetTypeId())
      {
         const WsfTrackMessage& message = static_cast<const WsfTrackMessage&>(aMessage);
         // todo hack ... offset info coming from the tracker, which does not have aux data
         std::string offset;
         if (mBeamDirectorTrack.GetAuxDataConst().AttributeExists("TARGET_OFFSET") &&
             (mBeamDirectorTrack.GetTargetIndex() == message.GetTrack()->GetTargetIndex()))
         {
            offset = mBeamDirectorTrack.GetAuxDataConst().GetString("TARGET_OFFSET");
         }
         mBeamDirectorTrack = *message.GetTrack();
         if (!offset.empty())
         {
            mBeamDirectorTrack.GetAuxData().Assign("TARGET_OFFSET", offset);
         }
      }
      else if (typeId == "CUED_TO_IMAGE")
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: CUED-TO-IMAGE message received.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }

         if (mState == cNOT_READY_TO_FIRE)
         {
            mState = cREADY_TO_FIRE; // In constraints
            UpdateEngagement(aSimTime);
         }
      }
      else if (typeId == "BREAKLOCK")
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: BREAKLOCK message received.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         PauseFire(aSimTime);         // Can't hit target, so pause firing.
         mState = cNOT_READY_TO_FIRE; // Not in constraints
      }
      else if (typeId == "LOST_IMAGE_CUE")
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: LOST_IMAGE_CUE message received.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         PauseFire(aSimTime);         // Can't hit target, so pause firing.
         mState = cNOT_READY_TO_FIRE; // Not in constraints
      }
      else if (typeId == "BEAM_DIRECTOR_TURN_OFF")
      {
         if (DebugEnabled())
         {
            auto out = ut::log::debug() << "HEL: BEAM_DIRECTOR_TURN_OFF message received.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
         CeaseFire(aSimTime);
         mState = cNOT_READY_TO_FIRE; // beam director turned off
      }
      else if (typeId == "END_FIRING")
      {
         CeaseFire(aSimTime);
      }
      else
      {
         processed = false;
      }
   }
   return processed;
}

// virtual
WsfWeapon::FireResult WsfCuedLaserWeapon::Fire(double aSimTime, const FireTarget& aTarget, const FireOptions& aSettings)
{
   FireResult result;
   // We must make sure the weapon is 'on'.
   // This does not mean it is lasing; only that it
   // processes periodic updates and can be turned 'off'.
   if (!IsTurnedOn())
   {
      GetSimulation()->TurnPartOn(aSimTime, this);
   }

   if (aTarget.mTrackPtr != nullptr)
   {
      if (HasCurrentEngagement() && (mCurrentTargetTrack.GetTrackId() == aTarget.mTrackPtr->GetTrackId()))
      {
         // If we've previously paused this engagement, resume fire.
         mBeamDirectorPtr->PointAtTarget(aSimTime, aTarget);
         if (GetState() == cPAUSED)
         {
            ResumeFire(aSimTime);
         }
         else if (GetState() == cREADY_TO_FIRE) // Could happen
         {
            UpdateEngagement(aSimTime);
         }
      }
      else
      {
         // This is a new track (and presumably, a new target as well).
         if (HasCurrentEngagement())
         {
            CeaseFire(aSimTime);
         }
         mState              = cNOT_READY_TO_FIRE; // Can't fire until we get a cued status from the beam director
         mCurrentTargetTrack = *aTarget.mTrackPtr;
         mBeamDirectorPtr->PointAtTarget(aSimTime, aTarget);

         // Set this as the "Current Target" to give other processors an easy reference
         // (e.g., the Engagement Interface)
         GetPlatform()->GetTrackManager().SetCurrentTarget(aSimTime, mCurrentTargetTrack);

         // Set up the new engagement in the base class.
         result = WsfImplicitWeapon::Fire(aSimTime, aTarget, aSettings);
      }
   }
   else if (!aTarget.mTargetName.empty())
   {
      // Use the truth location of the target in the current target track.
      WsfPlatform* targetPtr = GetSimulation()->GetPlatformByName(aTarget.mTargetName);
      bool         hasTarget = (targetPtr != nullptr);
      if (hasTarget)
      {
         if (mCurrentTargetTrack.GetTargetName() != aTarget.mTargetName) // todo check offset
         {
            WsfTrack targetTrack;
            WsfUtil::MakeTrack(aSimTime, targetTrack, *targetPtr, *GetPlatform());
            targetTrack.GetAuxData().Assign("TARGET_OFFSET", aTarget.mTargetOffset);
            FireTarget target;
            target.mTrackPtr = &targetTrack;
            result           = Fire(aSimTime, target, aSettings);
         }
         else
         {
            FireTarget target;
            target.mTrackPtr = &mCurrentTargetTrack;
            result           = Fire(aSimTime, target, aSettings);
         }
      }
      return result;
   }
   return result;
}
