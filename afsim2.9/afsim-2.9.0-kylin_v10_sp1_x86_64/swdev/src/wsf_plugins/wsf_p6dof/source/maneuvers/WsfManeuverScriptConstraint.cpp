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

#include "WsfManeuverScriptConstraint.hpp"

#include <limits>

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptTypes.hpp"
#include "WsfP6DOF_Mover.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

WsfManeuverScriptConstraint::WsfManeuverScriptConstraint()
   : mAssignTime{-1.0}
   , mMoverPtr{nullptr}
   , mSatisfiedPtr{nullptr}
   , mNextEvaluationTimePtr{nullptr}
{
}

void WsfManeuverScriptConstraint::Assign(double aSimTime, WsfP6DOF_Mover* aMoverPtr)
{
   mAssignTime = aSimTime;
   mMoverPtr   = aMoverPtr;
}

bool WsfManeuverScriptConstraint::Satisfied(double aSimTime) const
{
   bool retval{true};
   if (mSatisfiedPtr != nullptr)
   {
      WsfScriptContext& ctxPtr = mMoverPtr->GetSimulation()->GetScriptContext();

      UtScriptData timeArg;
      timeArg.SetDouble(mAssignTime);

      UtScriptData moverArg;
      moverArg.SetPointer(new UtScriptRef(mMoverPtr, ctxPtr.GetClass("WsfP6DOF_Mover")));

      UtScriptDataList scriptArgs;
      scriptArgs.push_back(timeArg);
      scriptArgs.push_back(moverArg);

      UtScriptData scriptRetval;
      ctxPtr.ExecuteScript(aSimTime, mSatisfiedPtr, scriptRetval, scriptArgs);
      retval = scriptRetval.GetBool();
   }
   return retval;
}

double WsfManeuverScriptConstraint::NextEvaluationTime(double aSimTime) const
{
   double retval{-1.0};
   if (mNextEvaluationTimePtr != nullptr)
   {
      WsfScriptContext& ctx = mMoverPtr->GetSimulation()->GetScriptContext();

      UtScriptData timeArg;
      timeArg.SetDouble(mAssignTime);

      UtScriptData moverArg;
      moverArg.SetPointer(new UtScriptRef(mMoverPtr, ctx.GetClass("WsfP6DOF_Mover")));

      UtScriptDataList scriptArgs;
      scriptArgs.push_back(timeArg);
      scriptArgs.push_back(moverArg);

      UtScriptData scriptRetval;
      ctx.ExecuteScript(aSimTime, mNextEvaluationTimePtr, scriptRetval, scriptArgs);
      retval = scriptRetval.GetDouble();
   }
   return retval;
}

//! Set the script implementing the check for constraint satisfaction.
//!
//! Set the script that implements the satisfaction check for the constraint. This
//! not only checks for existence of the script at the global context with the
//! given @p aScriptName, but will also validate that the signature of the
//! given script matches requirements.
//!
//! \param aScriptName - the name of script.
//! \param aSimulation - the simulation in which this constraint in participating.
void WsfManeuverScriptConstraint::SetSatisfiedScript(const std::string& aScriptName, const WsfSimulation& aSimulation)
{
   if (!aScriptName.empty())
   {
      auto& ctx     = aSimulation.GetScriptContext();
      mSatisfiedPtr = ctx.FindScript(aScriptName);
      if (mSatisfiedPtr)
      {
         bool ok = ctx.ValidateScript(mSatisfiedPtr, "bool", "double, WsfP6DOF_Mover");
         if (!ok)
         {
            mSatisfiedPtr = nullptr;
            auto out      = ut::log::warning() << "Script cannot be used as a script constraint.";
            out.AddNote() << "Script: " << aScriptName;
            out.AddNote() << "The required signature is bool (double, WsfP6DOF_Mover).";
            out.AddNote() << "This constraint will instead be always satisfied.";
         }
      }
      else
      {
         auto out = ut::log::warning() << "Script not found in the global context.";
         out.AddNote() << "Script: " << aScriptName;
         out.AddNote() << "This constraint will instead be always satisfied.";
      }
   }
}

//! Set the script implementing the computation of the next evaluation time.
//!
//! Set the script that implements the computation of the next evaluation time
//! for the constraint. This not only checks for existence of the script at the
//! global context with the given @p aScriptName, but will also validate that the
//! signature of the given script matches requirements.
//!
//! \param aScriptName - the name of script.
//! \param aSimulation - the simulation in which this constraint in participating.
void WsfManeuverScriptConstraint::SetNextEvaluationTimeScript(const std::string&   aScriptName,
                                                              const WsfSimulation& aSimulation)
{
   if (!aScriptName.empty())
   {
      auto& ctx              = aSimulation.GetScriptContext();
      mNextEvaluationTimePtr = ctx.FindScript(aScriptName);
      if (mNextEvaluationTimePtr)
      {
         bool ok = ctx.ValidateScript(mNextEvaluationTimePtr, "double", "double, WsfP6DOF_Mover");
         if (!ok)
         {
            mNextEvaluationTimePtr = nullptr;
            auto out               = ut::log::warning() << "Script cannot be used as a script constraint.";
            out.AddNote() << "Script: " << aScriptName;
            out.AddNote() << "The required signature is double (double, WsfP6DOF_Mover).";
            out.AddNote() << "This constraint will evaluate on each update of the mover.";
         }
      }
      else
      {
         auto out = ut::log::warning() << "Script not found in the global context.";
         out.AddNote() << "Script: " << aScriptName;
         out.AddNote() << "This constraint will evaluate on each update of the mover.";
      }
   }
}
