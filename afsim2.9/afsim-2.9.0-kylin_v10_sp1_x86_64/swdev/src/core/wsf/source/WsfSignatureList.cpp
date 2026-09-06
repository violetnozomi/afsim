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

#include "WsfSignatureList.hpp"

#include <sstream>

#include "UtException.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformObserver.hpp"
#include "WsfSignature.hpp"
#include "WsfSimulation.hpp"

// =================================================================================================
WsfSignatureList::WsfSignatureList()
   : mPlatformPtr(nullptr)
   , mInterfacePtrs()
{
}

// =================================================================================================
WsfSignatureList::WsfSignatureList(const WsfSignatureList& aSrc, WsfPlatform* aPlatformPtr)
   : mPlatformPtr(aPlatformPtr)
   , mInterfacePtrs()
{
   mInterfacePtrs.resize(aSrc.mInterfacePtrs.size(), nullptr);
   for (size_t i = 0; i < aSrc.mInterfacePtrs.size(); ++i)
   {
      if (aSrc.mInterfacePtrs[i] != nullptr)
      {
         mInterfacePtrs[i] = aSrc.mInterfacePtrs[i]->Clone();
      }
   }
}

// =================================================================================================
WsfSignatureList::~WsfSignatureList()
{
   for (size_t i = 0; i < mInterfacePtrs.size(); ++i)
   {
      delete mInterfacePtrs[i];
   }
}

// =================================================================================================
//! Add a signature item to the prototype signature list.
//! @param aInterfacePtr The pointer to the interface. The class assumes ownership.
//! @param aIndex        The index of the signature. This must be unique. An exception will be thrown
//!                      if it is not unique.
//! @note This should be called ONLY by the signature type objects or their surrogate.
void WsfSignatureList::AddInterface(Interface* aInterfacePtr, size_t aIndex)
{
   size_t newSize(aIndex + 1);
   if (newSize > mInterfacePtrs.size())
   {
      mInterfacePtrs.resize(newSize, nullptr);
   }
   if (mInterfacePtrs[aIndex] != nullptr)
   {
      auto out = ut::log::error() << "Duplicate signature index in WsfSignatureList.";
      out.AddNote() << "Index: " << aIndex;
      throw UtException("Duplicate signature index in WsfSignatureList: " + std::to_string(aIndex));
   }
   mInterfacePtrs[aIndex] = aInterfacePtr;
}

// =================================================================================================
// virtual
bool WsfSignatureList::Initialize(double aSimTime)
{
   bool ok(true);
   for (size_t i = 0; i < mInterfacePtrs.size(); ++i)
   {
      if (mInterfacePtrs[i] != nullptr)
      {
         ok &= mInterfacePtrs[i]->Initialize(aSimTime, mPlatformPtr);
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfSignatureList::ProcessInput(UtInput& aInput)
{
   bool myCommand(false);
   for (size_t i = 0; i < mInterfacePtrs.size(); ++i)
   {
      if ((mInterfacePtrs[i] != nullptr) && (mInterfacePtrs[i]->ProcessInput(aInput)))
      {
         myCommand = true;
         break;
      }
   }
   return myCommand;
}

// =================================================================================================
//! Set the signature state for the given type of signature.
//! @param aState The string Id of the new signature state.
//! @param aType  The string of the type of signature to change the state of.
void WsfSignatureList::SetState(WsfStringId aState, const std::string& aType)
{
   for (size_t i = 0; i < mInterfacePtrs.size(); ++i)
   {
      if ((mInterfacePtrs[i] != nullptr) && ((aType == "all") || (aType == mInterfacePtrs[i]->GetShortName())))
      {
         SetState(i, aState);
      }
   }
}

// =================================================================================================
std::string WsfSignatureList::GetClassName(size_t aIndex) const
{
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      return mInterfacePtrs[aIndex]->GetClassName();
   }
   return "<unknown>";
}

// =================================================================================================
WsfStringId WsfSignatureList::GetInputType(size_t aIndex) const
{
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      return mInterfacePtrs[aIndex]->GetInputType();
   }
   return WsfStringId();
}

// =================================================================================================
WsfStringId WsfSignatureList::GetState(size_t aIndex) const
{
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      return mInterfacePtrs[aIndex]->GetState();
   }
   return WsfStringId();
}

// =================================================================================================
bool WsfSignatureList::SetState(size_t aIndex, WsfStringId aState)
{
   bool isValid(false);
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      Interface* interfacePtr(mInterfacePtrs[aIndex]);
      if (interfacePtr->GetSignature() != nullptr)
      {
         WsfStringId oldState(interfacePtr->GetState());
         if (interfacePtr->GetSignature()->IsA_ValidState(aState))
         {
            interfacePtr->SetState(aState);
            isValid = true;
         }
         else
         {
            { // RAII block
               auto out = ut::log::warning() << "Undefined " << interfacePtr->GetClassName() << " state.";
               out.AddNote() << "T = " << mPlatformPtr->GetSimTime();
               out.AddNote() << "Platform: " << mPlatformPtr->GetName();
               out.AddNote() << "Platform Type: " << mPlatformPtr->GetType();
               out.AddNote() << "State: " << aState;
               out.AddNote() << "Using default.";
            }

            interfacePtr->SetState("default");
            isValid = false;
         }
         if (interfacePtr->GetState() != oldState)
         {
            WsfSimulation* simulationPtr = mPlatformPtr->GetSimulation();
            if (simulationPtr != nullptr)
            {
               WsfObserver::AppearanceType appearanceType =
                  static_cast<WsfObserver::AppearanceType>(WsfObserver::cSIGNATURE_STATE_START + aIndex);
               WsfObserver::PlatformAppearanceChanged(
                  simulationPtr)(mPlatformPtr->GetSimTime(), mPlatformPtr, appearanceType);
            }
         }
      }
   }
   return isValid;
}

// =================================================================================================
float WsfSignatureList::GetScaleFactor(size_t aIndex) const
{
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      return mInterfacePtrs[aIndex]->GetScaleFactor();
   }
   return 1.0F;
}

// =================================================================================================
bool WsfSignatureList::SetScaleFactor(size_t aIndex, float aScaleFactor)
{
   bool ok(false);
   if ((aIndex < mInterfacePtrs.size()) && (mInterfacePtrs[aIndex] != nullptr))
   {
      if (aScaleFactor > 0.0)
      {
         Interface* interfacePtr(mInterfacePtrs[aIndex]);
         if (aScaleFactor != interfacePtr->GetScaleFactor())
         {
            interfacePtr->SetScaleFactor(aScaleFactor);
            WsfSimulation* simulationPtr = mPlatformPtr->GetSimulation();
            if (simulationPtr != nullptr)
            {
               WsfObserver::AppearanceType appearanceType =
                  static_cast<WsfObserver::AppearanceType>(WsfObserver::cSIGNATURE_SCALE_FACTOR_START + aIndex);
               WsfObserver::PlatformAppearanceChanged(
                  simulationPtr)(mPlatformPtr->GetSimTime(), mPlatformPtr, appearanceType);
            }
         }
         ok = true;
      }
   }
   return ok;
}
