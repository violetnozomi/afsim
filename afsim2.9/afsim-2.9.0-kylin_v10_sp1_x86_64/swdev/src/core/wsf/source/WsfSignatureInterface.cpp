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

#include "WsfSignatureInterface.hpp"

#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfPlatform.hpp"
#include "WsfSignature.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"

// =================================================================================================
WsfSignatureInterface::WsfSignatureInterface()
   : mInputType()
   , mState(UtStringIdLiteral("default"))
   , mScaleFactor(1.0F)
   , mSignaturePtr(nullptr)
{
}

// =================================================================================================
WsfSignatureInterface::WsfSignatureInterface(const WsfSignatureInterface& aSrc)
   : mInputType(aSrc.mInputType)
   , mState(aSrc.mState)
   , mScaleFactor(aSrc.mScaleFactor)
   , mSignaturePtr(nullptr)
{
}

// =================================================================================================
WsfSignatureInterface::~WsfSignatureInterface()
{
   delete mSignaturePtr;
}

// =================================================================================================
// virtual
bool WsfSignatureInterface::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   std::string className(GetClassName());

   // Resolve the name of the signature if it was supplied by a script variable
   auto&             scenario      = aPlatformPtr->GetScenario();
   WsfScriptContext& globalContext = aPlatformPtr->GetSimulation()->GetScriptContext();
   WsfScriptContext& localContext  = aPlatformPtr->GetScriptContext();

   bool ok = mInputType.Initialize(className, aPlatformPtr, &localContext, globalContext);
   if (mInputType.GetId() != 0)
   {
      mSignaturePtr = CloneSignature(scenario);
      if (mSignaturePtr != nullptr)
      {
         ok &= mSignaturePtr->Initialize(aSimTime, aPlatformPtr);
      }
      else
      {
         auto out = ut::log::error() << "Undefined " << className << ".";
         out.AddNote() << "Type: " << mInputType;
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfSignatureInterface::ProcessInput(UtInput& aInput)
{
   bool        myCommand(false);
   std::string command(aInput.GetCommand());
   if (command == GetClassName())
   {
      mInputType.ReadValue(aInput);
      myCommand = true;
   }
   return myCommand;
}

// =================================================================================================
void WsfSignatureInterface::SetSignature(WsfSignature* aSignaturePtr)
{
   if (aSignaturePtr != mSignaturePtr)
   {
      delete mSignaturePtr;
      mSignaturePtr = aSignaturePtr;
   }
}
