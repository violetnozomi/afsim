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

#include "WsfScriptLaunchComputer.hpp"

#include "UtInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
//! Constructor
// (JAJ) I don't think this is used. Maybe leftover from the old checkpoint/restart code.
WsfScriptLaunchComputer::WsfScriptLaunchComputer()
   : WsfLaunchComputer()
   , mContextPtr(nullptr)
   , mScriptPtr(nullptr)
{
}

// =================================================================================================
//! Constructor
WsfScriptLaunchComputer::WsfScriptLaunchComputer(WsfScenario& aScenario)
   : WsfLaunchComputer()
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfLaunchComputer"))
   , mScriptPtr(nullptr)
{
   mContextPtr->DeclareVariable("WsfPlatform", "PLATFORM");
   mContextPtr->DeclareVariable("WsfWeapon", "WEAPON");
}

// =================================================================================================
//! Copy constructor (for Clone).
// protected
WsfScriptLaunchComputer::WsfScriptLaunchComputer(const WsfScriptLaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mScriptPtr(nullptr)
{
}

// =================================================================================================
//! Destructor
// virtual
WsfScriptLaunchComputer::~WsfScriptLaunchComputer()
{
   delete mContextPtr;
}

// =================================================================================================
// virtual
WsfLaunchComputer* WsfScriptLaunchComputer::Clone() const
{
   return new WsfScriptLaunchComputer(*this);
}

// =================================================================================================
// virtual
bool WsfScriptLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool ok = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   ok &= mContextPtr->Initialize(*GetSimulation(), this);
   UtScriptContext& inst = mContextPtr->GetContext();
   inst.Var("WEAPON").GetPointer()->SetExternalManaged(GetWeapon());
   inst.Var("PLATFORM").GetPointer()->SetExternalManaged(GetWeapon()->GetPlatform());

   // Ensure the script 'compute_intercept' exists and has the correct arguments.
   mScriptPtr = mContextPtr->FindScript("compute_intercept");
   if (mScriptPtr != nullptr)
   {
      ok &= mContextPtr->ValidateScript(mScriptPtr, "void", "WsfTrack, double");
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfScriptLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
double WsfScriptLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   // If computer generation mode simply call the base class, which will apply the commanded values.
   // We don't care about estimating the time because that is what computer generation is for!
   if (ComputerGenerationMode())
   {
      return WsfLaunchComputer::EstimatedTimeToIntercept(aSimTime, aTrack, aLaunchDelayTime);
   }

   InitializeResults(aSimTime, aTrack);

   if (mScriptPtr != nullptr)
   {
      static UtScriptClass* trackClassPtr = mContextPtr->GetClass("WsfTrack");
      assert(trackClassPtr != nullptr);

      UtScriptData     retVal;
      UtScriptDataList argList;
      argList.push_back(UtScriptData(new UtScriptRef(const_cast<WsfTrack*>(&aTrack), trackClassPtr)));
      argList.push_back(UtScriptData(aLaunchDelayTime));
      mContextPtr->ExecuteScript(aSimTime, mScriptPtr, retVal, argList);
   }

   double timeToIntercept = cFOREVER;
   if (InterceptTimeIsValid())
   {
      timeToIntercept = mInterceptTime - aSimTime;
   }
   UpdateResults(aSimTime, timeToIntercept, aTrack);
   return timeToIntercept;
}

// =================================================================================================
// protected virtual
bool WsfScriptLaunchComputer::InitializeTTIData()
{
   // No TTI data here... must return true to prevent base class failure.
   return true;
}
