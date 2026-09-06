// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFMANEUVER_HPP
#define WSFMANEUVER_HPP

#include "wsf_p6dof_export.h"

#include <memory>

#include "UtScriptAccessible.hpp"

class WsfManeuverConstraint;
class WsfP6DOF_Mover;
class WsfSimulation;

//! A maneuver that can be executed by a WsfP6DOF_Mover.
//!
//! A maneuver encapsulates a particular change or set of changes to
//! the control or state of the mover to which this maneuver is assigned.
//! Upon assignment, the maneuver will begin evaluating the entry constraint,
//! if any. Once the entry constraint is satisfied, the maneuver will call into
//! Execute a number of times until the execution has finished. Then the
//! maneuver will being evaluating the exit constraint. Once the exit
//! constraint is satisfied, the maneuver will complete.
class WSF_P6DOF_EXPORT WsfManeuver : public UtScriptAccessible
{
public:
   WsfManeuver();
   WsfManeuver(const WsfManeuver& aOther);
   ~WsfManeuver() override;

   WsfManeuver& operator=(const WsfManeuver& aOther) = delete;

   const char* GetScriptClassName() const override { return "WsfManeuver"; }

   void Update(double aSimTime);

   virtual WsfManeuver* Clone() const = 0;

   //! Execute the maneuver.
   //!
   //! Override this method to provide the action of the maneuver. This method
   //! should return the next time at which the maneuver should be executed.
   //! If a time is returned that is before @p aSimTime, then this signals that
   //! the execution is done, and the maneuver can instead turn to the evaluation
   //! of the exit constraint.
   //!
   //! \param aSimTime - the sim time when this method is being called.
   //! \returns        - the next time at which the maneuver needs to call this method.
   //!                   Return a value before @p aSimTime to indicate completion.
   virtual double Execute(double aSimTime) = 0;

   //! Return a type for this maneuver.
   //!
   //! Override this method to provide a name for this type of maneuver. This is
   //! used in Update() when extra debugging output is enabled as a way of identifying
   //! the type of maneuver being executed.
   virtual const char* Type() const = 0;

   //! Return true if extra debugging output is enabled.
   bool GetDebug() const { return mDebug; }

   //! Enable or disable extra debugging output.
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   WsfP6DOF_Mover*      GetMover() const;
   const WsfSimulation* GetSimulation() const;

   //! Return true if the command has been assigned.
   bool IsAssigned() const { return mStatus != Status::cUNASSIGNED; }

   //! Return true if the command is pending.
   bool IsPending() const { return mStatus == Status::cPENDING; }

   //! Return true if the command is executing.
   bool IsExecuting() const { return mStatus == Status::cEXECUTING; }

   //! Return true if the command is completing.
   bool IsCompleting() const { return mStatus == Status::cCOMPLETING; }

   //! Return true if the command is completed.
   bool IsCompleted() const { return mStatus == Status::cCOMPLETED; }

   //! Return true if the command was canceled.
   bool IsCanceled() const { return mStatus == Status::cCANCELED; }

   //! Return true if this maneuver is a sequence.
   virtual bool IsSequence() const { return false; }

   virtual void Cancel();
   void         Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr);
   void         Reset();

   void SetPending(double aSimTime);
   void SetExecuting();
   void SetCompleting(double aSimTime);
   void SetCompleted();

   //! Get the next time this maneuver needs to be updated.
   double GetNextUpdateTime() const { return mNextUpdateTime; }

   //! Set the next time this maneuver needs to be updated.
   void SetNextUpdateTime(double aTime) { mNextUpdateTime = aTime; }

   //! Get this maneuver's urgency.
   double GetUrgency() const { return mUrgency; }

   //! Set this maneuver's urgency.
   //!
   //! The urgency of a maneuver is a measure provided by each maneuver that can
   //! control details of the manner in which that maneuver is executed. The
   //! value of urgency can be used in whatever way the concrete subclasses
   //! of WsfManeuver need, but typically this will be a real number between
   //! 0.0 and 1.0, with 1.0 being the most urgent.
   void SetUrgency(double aUrgency) { mUrgency = aUrgency; }

   WsfManeuverConstraint* GetEntryConstraint() const;
   void                   SetEntryConstraint(std::unique_ptr<WsfManeuverConstraint> aConstraintPtr);
   WsfManeuverConstraint* GetExitConstraint() const;
   void                   SetExitConstraint(std::unique_ptr<WsfManeuverConstraint> aConstraintPtr);

private:
   enum class Status
   {
      cUNASSIGNED, //! Maneuver is not assigned.
      cPENDING,    //! Maneuver is assigned, but the entry constraint is not satisfied.
      cEXECUTING,  //! Maneuver is assigned and is executing.
      cCOMPLETING, //! Maneuver is assigned, has executed, as is pending exit.
      cCOMPLETED,  //! Maneuver has executed and the exit constraint was satisfied.
      cCANCELED    //! Maneuver was canceled before completion.
   };

   WsfP6DOF_Mover*                        mMoverPtr;       //! The mover to which this maneuver is assigned.
   Status                                 mStatus;         //! The current status of the maneuver.
   double                                 mLastUpdateTime; //! The last time Update was called.
   double                                 mNextUpdateTime; //! The next time update needs to execute.
   double                                 mUrgency;        //! The maneuver urgency.
   std::unique_ptr<WsfManeuverConstraint> mEntryPtr;       //! Maneuver entry constraint.
   std::unique_ptr<WsfManeuverConstraint> mExitPtr;        //! Maneuver exit constraint.
   bool                                   mDebug;          //! Indicates if extra debugging output is enabled.
};

#endif // WSFMANEUVER_HPP
