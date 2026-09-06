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

#include "WsfEW_RcvrComponent.hpp"

#include "WsfEW_EP.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfFalseTargetScreener.hpp"
#include "WsfSensorMode.hpp"

// =================================================================================================
//! Find the electronic warfare receiver component for the specified receiver.
// static
WsfEW_RcvrComponent* WsfEW_RcvrComponent::Find(const WsfEM_Rcvr& aRcvr)
{
   WsfEW_RcvrComponent* componentPtr(nullptr);
   aRcvr.GetComponents().FindByRole(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the electronic warfare receiver component for the specified receiver.
//! If the component does not exist then it will be created.
// static
WsfEW_RcvrComponent* WsfEW_RcvrComponent::FindOrCreate(WsfEM_Rcvr& aRcvr)
{
   WsfEW_RcvrComponent* componentPtr = Find(aRcvr);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfEW_RcvrComponent();
      aRcvr.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
WsfEW_RcvrComponent::WsfEW_RcvrComponent()
   : mElectronicProtectPtr(nullptr)
{
}

// =================================================================================================
WsfEW_RcvrComponent::WsfEW_RcvrComponent(const WsfEW_RcvrComponent& aSrc)
   : mElectronicProtectPtr(nullptr)
{
   if (aSrc.mElectronicProtectPtr != nullptr)
   {
      mElectronicProtectPtr = aSrc.mElectronicProtectPtr->Clone();
   }
}

// =================================================================================================
WsfEW_RcvrComponent::~WsfEW_RcvrComponent()
{
   delete mElectronicProtectPtr;
}

// =================================================================================================
WsfComponent* WsfEW_RcvrComponent::CloneComponent() const
{
   return new WsfEW_RcvrComponent(*this);
}

// =================================================================================================
WsfStringId WsfEW_RcvrComponent::GetComponentName() const
{
   static WsfStringId nullString; // The string ID for the NULL string is ALWAYS the same
   return nullString;
}

// =================================================================================================
const int* WsfEW_RcvrComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_EW_RCVR, 0};
   return roles;
}

// =================================================================================================
void* WsfEW_RcvrComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_EW_RCVR)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
bool WsfEW_RcvrComponent::Initialize(double aSimTime)
{
   bool ok(true);
   if (mElectronicProtectPtr != nullptr)
   {
      assert(GetComponentParent() != nullptr);
      WsfEM_Rcvr* rcvrPtr = GetComponentParent();
      return mElectronicProtectPtr->Initialize(*(rcvrPtr->GetSimulation()), rcvrPtr);
   }
   return ok;
}

// =================================================================================================
//! Called by WsfEM_Rcvr::RemoveInteractor.
void WsfEW_RcvrComponent::RemoveInteractor(WsfEM_Xmtr* aXmtrPtr)
{
   if (aXmtrPtr->GetFunction() == WsfEM_Xmtr::cXF_INTERFERER)
   {
      WsfEM_Rcvr*    rcvrPtr       = GetComponentParent();
      WsfSensorMode* sensorModePtr = dynamic_cast<WsfSensorMode*>(rcvrPtr->GetMode());
      if (sensorModePtr != nullptr)
      {
         WsfFalseTargetScreener* screenerPtr(WsfFalseTargetScreener::Find(*sensorModePtr));
         if (screenerPtr != nullptr)
         {
            screenerPtr->RemoveJammerXmtr(aXmtrPtr);
         }
      }
      if (mElectronicProtectPtr != nullptr)
      {
         mElectronicProtectPtr->RemoveJammerXmtr(aXmtrPtr);
      }
   }
}

// =================================================================================================
//! Set the Electronic Protect object for this receiver.
//! @param aElectronicProtectPtr Pointer to the new electronic protect object.
//! @note Any existing electronic protect object is deleted.
void WsfEW_RcvrComponent::SetElectronicProtect(WsfEW_EA_EP* aElectronicProtectPtr)
{
   if (aElectronicProtectPtr != mElectronicProtectPtr)
   {
      delete mElectronicProtectPtr;
      mElectronicProtectPtr = aElectronicProtectPtr;
   }
}
