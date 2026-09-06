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

#include "WsfEW_XmtrComponent.hpp"

#include "WsfEW_EA.hpp"

// =================================================================================================
//! Find the electronic warfare receiver component for the specified receiver.
// static
WsfEW_XmtrComponent* WsfEW_XmtrComponent::Find(const WsfEM_Xmtr& aXmtr)
{
   WsfEW_XmtrComponent* componentPtr = nullptr;
   aXmtr.GetComponents().FindByRole(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the electronic warfare receiver component for the specified receiver.
//! If the component does not exist then it will be created.
// static
WsfEW_XmtrComponent* WsfEW_XmtrComponent::FindOrCreate(WsfEM_Xmtr& aXmtr)
{
   WsfEW_XmtrComponent* componentPtr = Find(aXmtr);
   if (componentPtr == nullptr)
   {
      componentPtr = new WsfEW_XmtrComponent();
      aXmtr.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
WsfEW_XmtrComponent::WsfEW_XmtrComponent()
   : mElectronicAttackPtr(nullptr)
{
}

// =================================================================================================
WsfEW_XmtrComponent::WsfEW_XmtrComponent(const WsfEW_XmtrComponent& aSrc)
   : mElectronicAttackPtr(nullptr)
{
   if (aSrc.mElectronicAttackPtr != nullptr)
   {
      mElectronicAttackPtr = aSrc.mElectronicAttackPtr->Clone();
   }
}

// =================================================================================================
WsfEW_XmtrComponent::~WsfEW_XmtrComponent()
{
   delete mElectronicAttackPtr;
}

// =================================================================================================
WsfComponent* WsfEW_XmtrComponent::CloneComponent() const
{
   return new WsfEW_XmtrComponent(*this);
}

// =================================================================================================
WsfStringId WsfEW_XmtrComponent::GetComponentName() const
{
   static WsfStringId nullString; // The string ID for the NULL string is ALWAYS the same
   return nullString;
}

// =================================================================================================
const int* WsfEW_XmtrComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_EW_XMTR, 0};
   return roles;
}

// =================================================================================================
void* WsfEW_XmtrComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_EW_XMTR)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
bool WsfEW_XmtrComponent::Initialize(double aSimTime)
{
   bool ok(true);
   if (mElectronicAttackPtr != nullptr)
   {
      assert(GetComponentParent() != nullptr);
      WsfEM_Xmtr* xmtrPtr = GetComponentParent();
      return mElectronicAttackPtr->Initialize(*(xmtrPtr->GetSimulation()), xmtrPtr);
   }
   return ok;
}

// =================================================================================================
//! Set the Electronic Attack object for this receiver.
//! @param aElectronicAttackPtr Pointer to the new electronic attack object.
//! @note Any existing electronic attack object is deleted.
void WsfEW_XmtrComponent::SetElectronicAttack(WsfEW_EA_EP* aElectronicAttackPtr)
{
   if (aElectronicAttackPtr != mElectronicAttackPtr)
   {
      delete mElectronicAttackPtr;
      mElectronicAttackPtr = aElectronicAttackPtr;
   }
}

// =================================================================================================
#ifdef NOT_USED
// The following block was in the former WsfEW_Xmtr::NotifyListeners but was commented out. It is
// being left here for reference. If it is found to be needed then the following will have to be
// done:
//
// -) Add a NotifyListeners method to WsfEM_XmtrComponent.
// -) Modify WsfEM_Xmtr::NotifyListers to invoke the NotifyListeners method in attached components
// -) Add a NotifyListeners method to WsfEW_XmtrComponent that contains the following code:
//
void WsfEW_XmtrComponent::NotifyListeners(double aSimTime, WsfEM_Interaction& aResult)
{
   // TODO - AWK Not sure this is required commented out for now
   // Was in the NotifySensorListeners block in WsfDisEmissions and refactored to here.
   // May be need for external emitting sensors e.g. over DIS
   // Note that the GetFunction() check was added when refactoring
   if (GetFunction() != cXF_INTERFERER)
   {
      // Also compute the jammer effects. This will cause any jammers that are jamming this sensor/comm
      // to 'emit' and will trigger the listener interface for them....
      WsfRF_Jammer::ComputeTotalJammerEffects(aSimTime, aResult, false);
      GetArticulatedPart()->ClearTransientCue();
   }
}
#endif
