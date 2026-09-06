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

#include "WsfSixDOF_ScriptedManeuver.hpp"

#include "UtLog.hpp"
#include "UtScript.hpp"
#include "WsfSimulation.hpp"
#include "WsfSixDOF_Mover.hpp"
#include "script/WsfScriptContext.hpp"

wsf::six_dof::ScriptedManeuver::ScriptedManeuver()
   : Maneuver{}
   , mExecutePtr{nullptr}
{
}

wsf::six_dof::ScriptedManeuver::ScriptedManeuver(const ScriptedManeuver& aOther)
   : Maneuver(aOther)
   , mExecutePtr{aOther.mExecutePtr}
{
}

double wsf::six_dof::ScriptedManeuver::Execute(double aSimTime)
{
   double retval{-1.0};
   if (mExecutePtr != nullptr)
   {
      WsfScriptContext& ctx = GetMover()->GetSimulation()->GetScriptContext();

      UtScriptData timeArg;
      timeArg.SetDouble(aSimTime);

      UtScriptData moverArg;
      moverArg.SetPointer(new UtScriptRef(GetMover(), ctx.GetClass("WsfSixDOF_Mover")));

      UtScriptDataList scriptArgs;
      scriptArgs.push_back(timeArg);
      scriptArgs.push_back(moverArg);

      UtScriptData scriptRetval;
      ctx.ExecuteScript(aSimTime, mExecutePtr, scriptRetval, scriptArgs);
      retval = scriptRetval.GetDouble();
   }
   return retval;
}

//! Set the script for the execute operation for this maneuver.
//!
//! This will not only find the script in the global context with the given
//! @p aScriptName, but it will also validate that the given script has the
//! required signature.
//!
//! @param aScriptName - the name of script.
//! @param aSimulation - the simulation in which this maneuver is a part.
void wsf::six_dof::ScriptedManeuver::SetExecuteScript(const std::string& aScriptName, const WsfSimulation& aSimulation)
{
   if (!aScriptName.empty())
   {
      auto& ctx   = aSimulation.GetScriptContext();
      mExecutePtr = ctx.FindScript(aScriptName);
      if (mExecutePtr != nullptr)
      {
         // Validation isn't able to handle derived classes - disabling
         // bool ok = ctx.ValidateScript(mExecutePtr, "double", "double, Mover");
         // if (!ok)
         //{
         //   mExecutePtr = nullptr;
         //   auto out = ut::log::warning() << "Script cannot be used as the Execute script for a maneuver.";
         //   out.AddNote() << "Script: " << aScriptName;
         //   out.AddNote() << "The required signature is double (double, Mover).";
         //   out.AddNote() << "This maneuver will instead do nothing.";
         //}
      }
      else
      {
         auto out = ut::log::warning() << "Script not found in the global context.";
         out.AddNote() << "Script: " << aScriptName;
         out.AddNote() << "This maneuver will instead do nothing.";
      }
   }
}
