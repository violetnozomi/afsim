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

#include "WsfScriptedOrbitalDynamicsTerm.hpp"

#include <sstream>
#include <stdexcept>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtScript.hpp"
#include "UtScriptData.hpp"
#include "UtScriptDataPack.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVec3.hpp"
#include "WsfIntegratingPropagator.hpp"
#include "WsfOrbitalDynamics.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "script/WsfScriptContext.hpp"

bool WsfScriptedOrbitalDynamicsTerm::ProcessInput(UtInput& aInput)
{
   bool        retval{false};
   std::string command = aInput.GetCommand();
   if (command == "script")
   {
      aInput.ReadCommand(mScriptName);
      retval = true;
   }
   return retval;
}

UtVec3d WsfScriptedOrbitalDynamicsTerm::ComputeAcceleration(double            aMass,
                                                            const UtCalendar& aTime,
                                                            const UtVec3d&    aPosition,
                                                            const UtVec3d&    aVelocity) const
{
   UtVec3d retval{0.0, 0.0, 0.0};
   if (mScriptPtr)
   {
      // NOTE: GetDynamics will return non-null because mScriptPtr is only set in a method when dynamics
      // have been associated with this object. Note also that the copy constructor clears mScriptPtr to
      // null.
      const WsfSpaceMoverBase* moverPtr      = GetDynamics()->GetPropagator()->GetSpaceMover();
      WsfSimulation*           simulationPtr = moverPtr->GetSimulation();
      auto&                    ctx           = simulationPtr->GetScriptContext();

      UtScriptDataPacker scriptArgs{ctx.GetTypes()};
      scriptArgs << moverPtr;
      scriptArgs << aMass;
      scriptArgs << new UtScriptRef{new UtCalendar{aTime}, ctx.GetClass("Calendar"), UtScriptRef::cMANAGE};
      scriptArgs << UtScriptVec3::Create(aPosition);
      scriptArgs << UtScriptVec3::Create(aVelocity);

      UtScriptData scriptRetval;
      ctx.ExecuteScript(mScriptPtr, scriptRetval, scriptArgs);
      auto refPtr = scriptRetval.GetPointer();
      if (refPtr && refPtr->IsValid())
      {
         retval = *scriptRetval.GetPointer()->GetAppObject<UtVec3d>();
      }
      else
      {
         ut::log::error() << "Script '" << mScriptName << "' must return a value. Disabling this orbital dynamics term.\n";
         mScriptPtr = nullptr;
      }
   }
   return retval;
}

bool WsfScriptedOrbitalDynamicsTerm::Initialize(const WsfOrbitalDynamics& aDynamics)
{
   bool ok = WsfOrbitalDynamicsTerm::Initialize(aDynamics);
   ok      = FindScript() && ok;
   return ok;
}

bool WsfScriptedOrbitalDynamicsTerm::FindScript()
{
   bool retval{true};
   if (!mScriptName.empty())
   {
      // NOTE: GetDynamics() will be non-null because this method is only called after the base class is initialized.
      WsfSpaceMoverBase* moverPtr = GetDynamics()->GetPropagator()->GetSpaceMover();
      if (moverPtr)
      {
         WsfSimulation* simulationPtr = moverPtr->GetSimulation();
         auto&          ctx           = simulationPtr->GetScriptContext();
         mScriptPtr                   = ctx.FindScript(mScriptName);
         if (mScriptPtr)
         {
            bool ok = ctx.ValidateScript(mScriptPtr, "Vec3", "WsfIntegratingSpaceMover, double, Calendar, Vec3, Vec3");
            if (!ok)
            {
               mScriptPtr = nullptr;
               retval     = false;
               ut::log::error()
                  << "'" << mScriptName << "' cannot be used as a script orbital dynamics term."
                  << " The required signature is Vec3 (WsfIntegratingSpaceMover, double, Calendar, Vec3, Vec3).\n";
            }
         }
         else
         {
            ut::log::error() << "There is so script in the global context with the name '" << mScriptName << "'.\n";
            retval = false;
         }
      }
   }
   else
   {
      ut::log::error() << "No script specified for scripted orbital dynamics term.\n";
      retval = false;
   }
   return retval;
}
