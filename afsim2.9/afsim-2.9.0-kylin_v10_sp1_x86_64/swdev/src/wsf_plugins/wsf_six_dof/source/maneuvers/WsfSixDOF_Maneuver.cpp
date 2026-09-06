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

#include "WsfSixDOF_Maneuver.hpp"

#include <limits>

#include "UtLog.hpp"
#include "WsfSixDOF_ManeuverConstraint.hpp"
#include "WsfSixDOF_Mover.hpp"

wsf::six_dof::Maneuver::Maneuver()
   : mMoverPtr{nullptr}
   , mStatus{Status::cUNASSIGNED}
   , mLastUpdateTime{-1.0}
   , mNextUpdateTime{-1.0}
   , mUrgency{1.0}
   , mEntryPtr{nullptr}
   , mExitPtr{nullptr}
   , mDebug{false}
{
}

wsf::six_dof::Maneuver::Maneuver(const Maneuver& aOther)
   : mMoverPtr{nullptr}
   , mStatus{aOther.mStatus}
   , mLastUpdateTime{aOther.mLastUpdateTime}
   , mNextUpdateTime{aOther.mNextUpdateTime}
   , mUrgency{1.0}
   , mEntryPtr{nullptr}
   , mExitPtr{nullptr}
   , mDebug{aOther.mDebug}
{
   if (aOther.mEntryPtr)
   {
      mEntryPtr.reset(aOther.mEntryPtr->Clone());
   }
   if (aOther.mExitPtr)
   {
      mExitPtr.reset(aOther.mExitPtr->Clone());
   }
}

wsf::six_dof::Maneuver::~Maneuver() {}

//! Update the maneuver.
//!
//! This will update this maneuver and manage the life cycle of the maneuver.
//! This method is a template method that calls into the Execute() method
//! defined on each concrete subclass. After the call to this method, the next
//! required update time will be set and Update() will do no work before reaching
//! that time.
void wsf::six_dof::Maneuver::Update(double aSimTime)
{
   // Avoid infinite recursion in a few corner cases.
   if (aSimTime <= mLastUpdateTime)
   {
      return;
   }

   mLastUpdateTime = aSimTime;

   if (aSimTime < mNextUpdateTime)
   {
      return;
   }

   auto out = ut::log::debug();

   if (GetDebug())
   {
      out << "Updating maneuver.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Type: " << Type();
   }

   if (IsCanceled())
   {
      if (GetDebug())
      {
         out.AddNote() << "Maneuver is canceled.";
      }
      mNextUpdateTime = std::numeric_limits<double>::max();
   }
   else
   {
      if (IsPending())
      {
         if (GetDebug())
         {
            out.AddNote() << "Maneuver is pending.";
         }

         if (!mEntryPtr || mEntryPtr->Satisfied(aSimTime))
         {
            if (GetDebug())
            {
               out.AddNote() << "Entry constraint satisfied.";
            }
            SetExecuting();
         }
         else
         {
            mNextUpdateTime = mEntryPtr->NextEvaluationTime(aSimTime);
            if (GetDebug())
            {
               out.AddNote() << "Entry constraint will reevaluate at time " << mNextUpdateTime << " sec.";
            }
         }
      }

      if (IsExecuting())
      {
         if (GetDebug())
         {
            out.AddNote() << "Maneuver is executing.";
         }
         mNextUpdateTime = Execute(aSimTime);

         if (mNextUpdateTime < aSimTime)
         {
            if (GetDebug())
            {
               out.AddNote() << "Execution finished.";
            }
            SetCompleting(aSimTime);
         }
      }

      if (IsCompleting())
      {
         if (GetDebug())
         {
            out.AddNote() << "Maneuver is completing.";
         }

         if (!mExitPtr || mExitPtr->Satisfied(aSimTime))
         {
            if (GetDebug())
            {
               out.AddNote() << "Maneuver is complete.";
            }
            mNextUpdateTime = std::numeric_limits<double>::max();
            SetCompleted();
         }
         else
         {
            mNextUpdateTime = mExitPtr->NextEvaluationTime(aSimTime);
            if (GetDebug())
            {
               out.AddNote() << "Exit constraint will reevaluate at time " << mNextUpdateTime << " sec.";
            }
         }
      }
   }
}

//! Return the mover to which this maneuver is assigned.
wsf::six_dof::Mover* wsf::six_dof::Maneuver::GetMover() const
{
   return mMoverPtr;
}

//! Return the simulation in which this maneuver is assigned to a mover.
const WsfSimulation* wsf::six_dof::Maneuver::GetSimulation() const
{
   if (mMoverPtr)
   {
      return mMoverPtr->GetSimulation();
   }
   return nullptr;
}

//! Cancel this maneuver.
void wsf::six_dof::Maneuver::Cancel()
{
   if (IsAssigned() && !IsCompleted())
   {
      mStatus = Status::cCANCELED;
   }
}

//! Assign this maneuver to a mover.
void wsf::six_dof::Maneuver::Assign(double aSimTime, Mover* aMoverPtr)
{
   if (!IsAssigned() && aMoverPtr != nullptr)
   {
      mNextUpdateTime = aSimTime;
      mMoverPtr       = aMoverPtr;
      SetPending(aSimTime);
   }
}

//! Reset the maneuver.
//!
//! This is used by maneuver sequences when they are looping. The maneuver needs to be
//! reset so that it can be assigned again.
void wsf::six_dof::Maneuver::Reset()
{
   mMoverPtr       = nullptr;
   mStatus         = Status::cUNASSIGNED;
   mNextUpdateTime = -1.0;
}

//! Set the maneuver to pending.
//!
//! A pending maneuver is assigned but is not currently executing. In addition to updating
//! the status of this maneuver, this will call Assign() on the entry constraint so that
//! it can be evaluated with the correct context.
//!
//! \param aSimTime - the time at which this maneuver was set to pending.
void wsf::six_dof::Maneuver::SetPending(double aSimTime)
{
   mStatus = Status::cPENDING;
   if (mEntryPtr != nullptr)
   {
      mEntryPtr->Assign(aSimTime, mMoverPtr);
   }
}

//! Set the maneuver to executing.
void wsf::six_dof::Maneuver::SetExecuting()
{
   mStatus = Status::cEXECUTING;
}

//! Set the maneuver to completing.
//!
//! A maneuver is completing if the execution has finished, but it has not yet met its
//! exit constraint. In addition to changing the status of this maneuver, this will
//! call Assign() on the exit constraint so that it can be evaluated with the correct
//! context.
//!
//! \param aSimTime - the time at which this maneuver was set to completing.
void wsf::six_dof::Maneuver::SetCompleting(double aSimTime)
{
   mStatus = Status::cCOMPLETING;
   if (mExitPtr != nullptr)
   {
      mExitPtr->Assign(aSimTime, mMoverPtr);
   }
}

//! Set the maneuver to complete.
void wsf::six_dof::Maneuver::SetCompleted()
{
   mStatus = Status::cCOMPLETED;
}

//! Return this maneuver's entry constraint.
wsf::six_dof::ManeuverConstraint* wsf::six_dof::Maneuver::GetEntryConstraint() const
{
   return mEntryPtr.get();
}

//! Set this maneuver's entry constraint.
//!
//! Set the entry constraint for this maneuver. The constraint will only be set if this maneuver
//! is not already assigned.
//!
//! @param aConstraintPtr - the entry constraint.
void wsf::six_dof::Maneuver::SetEntryConstraint(std::unique_ptr<ManeuverConstraint> aConstraintPtr)
{
   if (!IsAssigned())
   {
      mEntryPtr = std::move(aConstraintPtr);
   }
}

//! Return this maneuver's exit constraint.
wsf::six_dof::ManeuverConstraint* wsf::six_dof::Maneuver::GetExitConstraint() const
{
   return mExitPtr.get();
}

//! Set this maneuver's exit constraint.
//!
//! Set the exit constraint for this maneuver. The constraint will only be set if this maneuver
//! is not already assigned.
//!
//! @param aConstraintPtr - the exit constraint.
void wsf::six_dof::Maneuver::SetExitConstraint(std::unique_ptr<ManeuverConstraint> aConstraintPtr)
{
   if (!IsAssigned())
   {
      mExitPtr = std::move(aConstraintPtr);
   }
}
