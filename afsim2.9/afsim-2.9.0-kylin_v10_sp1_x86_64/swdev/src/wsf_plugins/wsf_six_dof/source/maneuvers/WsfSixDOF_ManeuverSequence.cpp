// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfSixDOF_ManeuverSequence.hpp"

#include <limits>

#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::ManeuverSequence::ManeuverSequence()
   : Maneuver{}
   , mManeuvers{}
   , mCurrentManeuver{}
   , mLoop{false}
{
}

wsf::six_dof::ManeuverSequence::ManeuverSequence(const ManeuverSequence& aOther)
   : Maneuver(aOther)
   , mManeuvers(aOther.mManeuvers)
   , mCurrentManeuver{}
   , mLoop{aOther.mLoop}
{
   mCurrentManeuver = mManeuvers.begin();
}

wsf::six_dof::ManeuverSequence* wsf::six_dof::ManeuverSequence::Clone() const
{
   return new ManeuverSequence{*this};
}

//! Execute the maneuver sequence.
//!
//! This will execute the current maneuver in the sequence and advance to the
//! next when appropriate.
double wsf::six_dof::ManeuverSequence::Execute(double aSimTime)
{
   if (mCurrentManeuver == mManeuvers.end())
   {
      SetNextUpdateTime(-1.0);
   }
   else
   {
      Iterator sentinel = mCurrentManeuver;
      while (mCurrentManeuver != mManeuvers.end() && GetNextUpdateTime() <= aSimTime)
      {
         Maneuver* curr = mCurrentManeuver->get();
         if (!curr->IsAssigned())
         {
            curr->Assign(aSimTime, GetMover());
         }

         curr->Update(aSimTime);

         if (curr->IsCompleted() || curr->IsCanceled())
         {
            Iterator prev    = mCurrentManeuver;
            mCurrentManeuver = Advance(prev);

            if (sentinel == mCurrentManeuver)
            {
               {
                  // RAII block
                  auto out = ut::log::warning()
                             << "Probably infinite loop detected in a maneuver sequence on platform.";
                  out.AddNote() << "Platform: " << GetMover()->GetPlatform()->GetName();
                  out.AddNote() << "Maneuver sequence aborted.";
               }
               Cancel();
               SetNextUpdateTime(-1.0);
               break;
            }

            if (mCurrentManeuver != mManeuvers.end())
            {
               SetNextUpdateTime((*mCurrentManeuver)->GetNextUpdateTime());
            }
            else
            {
               SetNextUpdateTime(-1.0);
            }

            if (mLoop)
            {
               (*prev)->Reset();
            }
         }
         else
         {
            SetNextUpdateTime(curr->GetNextUpdateTime());
         }
      }
   }
   return GetNextUpdateTime();
}

//! Clear out this maneuver sequence.
void wsf::six_dof::ManeuverSequence::Clear()
{
   mManeuvers.clear();
   mCurrentManeuver = mManeuvers.begin();
   mLoop            = false;
}

//! Append the given maneuver to the end of this sequence.
//!
//! This will append @p aManeuverPtr to the end of this sequence.
//!
//! @param aManeuverPtr - the maneuver to append
void wsf::six_dof::ManeuverSequence::Append(std::unique_ptr<Maneuver>&& aManeuverPtr)
{
   mManeuvers.emplace_back(std::move(aManeuverPtr));
   if (mManeuvers.size() == 1)
   {
      mCurrentManeuver = mManeuvers.begin();
   }
   else if (mCurrentManeuver == mManeuvers.end())
   {
      // We were done with the sequence, and we appended something.
      mCurrentManeuver = --mManeuvers.end();
      SetNextUpdateTime(-1.0); // Make sure we execute next Update
   }
}

//! Insert the maneuver after the current maneuver in the sequence.
//!
//! This will insert the given @p aManeuverPtr into the sequence after the current
//! maneuver. If there is no current maneuver, then @p aManeuverPtr will be inserted at
//! the front of the sequence.
//!
//! @param aManeuverPtr - the maneuver to insert
void wsf::six_dof::ManeuverSequence::InsertNext(std::unique_ptr<Maneuver>&& aManeuverPtr)
{
   if (mCurrentManeuver != Iterator{})
   {
      auto next = Advance(mCurrentManeuver);
      mManeuvers.emplace(next, std::move(aManeuverPtr));
   }
   else
   {
      // This means that the sequence has finished, and so we should append the maneuver.
      // Or, it could be that the sequence is just empty, in which case we append the maneuver.
      mManeuvers.emplace_back(std::move(aManeuverPtr));
   }
}

//! Enable/disable sequence looping.
//!
//! This will enable or disable the sequence from looping indefinitely.
//! If looping is enabled after the sequence has already completed, it
//! will restart the sequence at the beginning.
void wsf::six_dof::ManeuverSequence::SetLoop(bool aLoop)
{
   mLoop = aLoop;

   if (mLoop)
   {
      // Reset any maneuvers before the current.
      for (auto it = mManeuvers.begin(); it != mCurrentManeuver; ++it)
      {
         (*it)->Reset();
      }

      // If the sequence is already finished, then restart it.
      if (mCurrentManeuver == mManeuvers.end())
      {
         mCurrentManeuver = mManeuvers.begin();
         SetNextUpdateTime(-1.0);
      }
   }
}

//! Get the current maneuver in the sequence.
wsf::six_dof::Maneuver* wsf::six_dof::ManeuverSequence::GetCurrentManeuver() const
{
   Maneuver* retval{nullptr};
   if (mCurrentManeuver != mManeuvers.end())
   {
      retval = (*mCurrentManeuver).get();
   }
   return retval;
}

//! Advance to next maneuver taking looping sequences into account.
wsf::six_dof::ManeuverSequence::Iterator wsf::six_dof::ManeuverSequence::Advance(const Iterator& aIterator)
{
   Iterator retval = aIterator;
   ++retval;
   if (mLoop && retval == mManeuvers.end())
   {
      retval = mManeuvers.begin();
   }
   return retval;
}
