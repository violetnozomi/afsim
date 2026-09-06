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

#ifndef WSF_SIXDOF_FORMATIONCOMMAND_HPP
#define WSF_SIXDOF_FORMATIONCOMMAND_HPP

#include "wsf_six_dof_export.h"

#include <memory>
#include <string>

#include "UtScriptAccessible.hpp"
#include "WsfEvent.hpp"

class WsfSimulation;

namespace wsf
{
namespace six_dof
{
class Formation;

//! This class is the base from which all formation commands will be derived.
//! This specifies the interface and provides a few utility functions.
class WSF_SIX_DOF_EXPORT FormationCommand : public UtScriptAccessible
{
public:
   FormationCommand();
   FormationCommand(const FormationCommand& aOther);
   virtual ~FormationCommand() = default;

   FormationCommand& operator=(const FormationCommand& aOther) = delete;

   //! Subclasses must provide the ability to clone a command.
   virtual FormationCommand* Clone() const = 0;

   //! Subclasses should report a string type for the command.
   //! Typically these will be one word, in all caps. This is used primarily in
   //! debugging output.
   virtual std::string Type() const = 0;

   const char* GetScriptClassName() const override { return "WsfSixDOF_FormationCommand"; }

   bool   Process(Formation* aFormationPtr, double aSimTime);
   double Update(double aSimTime);

   //! Should this object produce extra debugging output.
   bool GetDebug() const { return mDebug; }

   //! Activate or deactivate debugging output.
   void SetDebug(bool aDebug) { mDebug = aDebug; }

   //! Return if the command is instantaneous.
   //!
   //! Instantaneous commands typically alter the logical structure of the formation,
   //! such as setting the detached state, or renaming a sub-formation. Instantaneous
   //! commands typically only Execute() once.
   virtual bool IsInstantaneous() const = 0;

   //! Return if this command is a sequence of commands.
   virtual bool IsSequence() const = 0;

   //! Return if this command has been assigned to a formation.
   bool IsAssigned() const { return mFormationPtr != nullptr; }

   //! Return if this command is pending execution.
   bool IsPending() const { return mStatus == Status::cPENDING; }

   //! Return if this command is currently executing.
   bool IsExecuting() const { return mStatus == Status::cEXECUTING; }

   //! Return if this command completed successfully.
   bool IsCompleted() const { return mStatus == Status::cCOMPLETED; }

   //! Return if this command is being canceled.
   bool IsCanceling() const { return mStatus == Status::cCANCELING; }

   //! Return if this command was canceled.
   bool IsCanceled() const { return mStatus == Status::cCANCELED; }

   //! Return if this command was dropped.
   bool IsDropped() const { return mStatus == Status::cDROPPED; }

   //! Return if this command is running.
   //!
   //! If a command is running, it has an associated event in the simulation event
   //! queue.
   bool IsRunning() const { return IsPending() || IsExecuting() || IsCanceling(); }

   //! Get the formation to which this is assigned.
   Formation* GetFormation() const { return mFormationPtr; }

   void Assign(Formation* aFormationPtr, WsfSimulation& aSim, double aSimTime, bool aLaunch = true);
   void Cancel();
   void Drop(WsfSimulation& aSim);

   //! An execution constraint on a formation command.
   //!
   //! This currently supports two kinds of constraints: an absolute time constraint
   //! and a relative time constraint.
   class Constraint : public UtScriptAccessible
   {
   public:
      Constraint()
         : mType{Type::cRELATIVE}
         , mConstraintTime{0.0}
         , mAssignmentTime{0.0}
      {
      }
      Constraint(const Constraint& aOther) = default;
      ~Constraint()                        = default;

      Constraint& operator=(const Constraint& aOther) = delete;

      Constraint* Clone() const { return new Constraint{*this}; }

      const char* GetScriptClassName() const override { return "WsfSixDOF_FormationCommandConstraint"; }

      //! Set the time at which the command was assigned to a formation.
      void SetAssignmentTime(double aSimTime) { mAssignmentTime = aSimTime; }

      void SetAbsoluteTimeConstraint(double aAbsoluteTime);
      void SetRelativeTimeConstraint(double aRelativeTime);

      //! Get the absolute time constraint.
      double GetAbsoluteTimeConstraint() const { return mConstraintTime; }

      //! Get the relative time constraint.
      double GetRelativeTimeConstraint() const { return mAssignmentTime + mConstraintTime; }

      bool IsSatisfied(double aSimTime) const;

      double GetNextConstraintEvalTime(double aSimTime) const;

   private:
      enum class Type
      {
         cRELATIVE,
         cABSOLUTE
      };

      Type   mType;
      double mConstraintTime;
      double mAssignmentTime;
   };

   void SetConstraint(std::unique_ptr<Constraint>&& aConstraintPtr);

   //! Get the execution constraint.
   Constraint* GetConstraint() const { return mConstraintPtr.get(); }

   //! Actually execute the command.
   //!
   //! This should return the next time to execute the command. If this returns a negative
   //! time, the command will stop executing and be marked complete.
   virtual double Execute(double aSimTime) = 0;

   //! Decide if the given formation should accept the command.
   //!
   //! This will return true if the given formation should execute this command.
   //! If this returns true, the command will be executed on the formation on which the
   //! assignment was attempted.
   virtual bool AcceptCommand(Formation* aFormationPtr) = 0;

   //! Returns if this command will consider transformations to sub-formations.
   //!
   //! This will return true if the command will consider and potentially cause
   //! commands to be executed on sub-formations of the formation to which this
   //! is assigned.
   virtual bool CanPropagate() const = 0;

   //! Perform common computations for the transformation of this command.
   //!
   //! This allows for a one-time computation of any results needed for the transformation
   //! of this command to the sub-formations of the given formation. This will only
   //! be called if CanPropagate() returns true.
   virtual void ComputeCommonTransformation(Formation* aFormationPtr) = 0;

   //! Compute the specific transformation for a child of a formation of this command.
   //!
   //! This will compute the transformation of this command for the given child of
   //! the given parent. This will only be called if CanPropagate() returns true.
   //! This will be called in sequence for each sub-formation of @p aParentPtr, and after
   //! ComputeCommonTransformation().
   virtual std::unique_ptr<FormationCommand> ComputeTransformation(Formation* aParentPtr, Formation* aChildPtr) = 0;

protected:
   class Event : public WsfEvent
   {
   public:
      Event(FormationCommand* aCommandPtr, WsfSimulation& aSim, double aSimTime);
      ~Event() override = default;
      WsfEvent::EventDisposition Execute() override;

   private:
      FormationCommand* mCommandPtr;
      WsfSimulation&    mSim;
   };

private:
   //! The various statuses available to formation commands.
   enum class Status
   {
      cUNASSIGNED, //! Command is not assigned to a formation
      cPENDING,    //! Command is assigned but it waiting to execute due to constraint
      cEXECUTING,  //! Command is executing
      cCOMPLETED,  //! Command execution completed successfully
      cCANCELING,  //! Command is being canceled
      cCANCELED,   //! Command execution was canceled
      cDROPPED     //! Command was dropped from the formation
   };

   Formation*                  mFormationPtr;  //! The formation to which this command is assigned.
   std::unique_ptr<Constraint> mConstraintPtr; //! The constraint on the command; may be nullptr
   Status                      mStatus;        //! The status of this command
   bool                        mDebug;         //! Display debugging info
};
} // namespace six_dof
} // namespace wsf

#endif // WSF_SIXDOF_FORMATIONCOMMAND_HPP
