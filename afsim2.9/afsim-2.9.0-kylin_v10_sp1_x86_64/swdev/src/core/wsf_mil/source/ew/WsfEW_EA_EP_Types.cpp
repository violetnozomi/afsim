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

#include "WsfEW_EA_EP_Types.hpp"

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfEW_CommComponent.hpp"
#include "WsfEW_RcvrComponent.hpp"
#include "WsfEW_SensorComponent.hpp"
#include "WsfEW_XmtrComponent.hpp"
#include "WsfMil.hpp"
#include "WsfScenario.hpp"

// Pre-defined types
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"

//! Since WsfEM_Xmtr and WsfEM_Rcvr do not derive from WsfComponent, these template declarations allow them to "act" like
//! components only within this translation unit by giving them roles, which they must have in order to use component factories.
template<>
struct WsfComponentRole<WsfEM_Rcvr> : std::integral_constant<int, cWSF_COMPONENT_EM_RCVR>
{
};
template<>
struct WsfComponentRole<WsfEM_Xmtr> : std::integral_constant<int, cWSF_COMPONENT_EM_XMTR>
{
};

namespace
{
//! Component factory to process transmitter input.
class EA_ComponentFactory : public WsfComponentFactory<WsfEM_Xmtr>
{
public:
   bool ProcessInput(UtInput& aInput, WsfEM_Xmtr& aXmtr) override
   {
      bool myCommand(false);
      if (aInput.GetCommand() == "electronic_attack")
      {
         myCommand = true;
         UtInputBlock inputBlock(aInput);
         std::string  baseType;
         aInput.ReadValue(baseType);
         WsfEW_EA_EP*       instancePtr = WsfEW_EA::GetElectronicAttack(aXmtr);
         WsfEW_EA_EP_Types& types(WsfEW_EA_EP_Types::Get(GetScenario()));
         WsfEW_EA_EP*       baseTypePtr = types.Find(baseType);
         if (baseTypePtr != nullptr)
         {
            // If the word after the block introduction is a valid type then a clone of the type
            // becomes the new definition (potentially replacing the existing definition.)

            // Delete the existing instance from the component list if it exists.
            if (instancePtr != nullptr)
            {
               WsfEW_EA::SetElectronicAttack(aXmtr, nullptr);
            }
            // Ensure the base type is an EA type (the type list stores both EA and EP types).
            if (dynamic_cast<WsfEW_EA*>(baseTypePtr) == nullptr)
            {
               throw UtInput::BadValue(aInput, "EW: " + baseTypePtr->GetType() + " is not an electronic_attack type.");
            }
            instancePtr = types.Clone(baseType);
            inputBlock.ProcessInput(instancePtr);
            WsfEW_EA::SetElectronicAttack(aXmtr, instancePtr);
         }
         else if (instancePtr == nullptr)
         {
            // This is that allows for legacy type input within the transmitter block. It does
            // cause a little issue if the user has mis-spelled or entered an incorrect type,
            // but it should error out when processing the input block.
            aInput.PushBack(baseType);
            instancePtr = types.Clone("WSF_ELECTRONIC_ATTACK");
            inputBlock.ProcessInput(instancePtr);
            WsfEW_EA::SetElectronicAttack(aXmtr, instancePtr);
         }
         else
         {
            // Edit the existing instance.
            aInput.PushBack(baseType);
            instancePtr->ProcessInput(aInput);
         }
      }
      return myCommand;
   }
};

//! Component factory to process receiver input.
class EP_ComponentFactory : public WsfComponentFactory<WsfEM_Rcvr>
{
public:
   bool ProcessInput(UtInput& aInput, WsfEM_Rcvr& aRcvr) override
   {
      bool myCommand(false);
      if (aInput.GetCommand() == "electronic_protect")
      {
         myCommand = true;
         UtInputBlock inputBlock(aInput);
         std::string  baseType;
         aInput.ReadValue(baseType);
         WsfEW_EA_EP*       instancePtr = WsfEW_EP::GetElectronicProtect(aRcvr);
         WsfEW_EA_EP_Types& types(WsfEW_EA_EP_Types::Get(GetScenario()));
         WsfEW_EA_EP*       baseTypePtr = types.Find(baseType);
         if (baseTypePtr != nullptr)
         {
            // If the word after the block introduction is a valid type then a clone of the type
            // becomes the new definition (potentially replacing the existing definition.)

            // Delete the existing instance from the component list if it exists.
            if (instancePtr != nullptr)
            {
               WsfEW_EP::SetElectronicProtect(aRcvr, nullptr);
            }
            // Ensure the base type is an EA type (the type list stores both EA and EP types).
            if (dynamic_cast<WsfEW_EP*>(baseTypePtr) == nullptr)
            {
               throw UtInput::BadValue(aInput, "EW: " + baseTypePtr->GetType() + " is not an electronic_protect type.");
            }
            instancePtr = types.Clone(baseType);
            inputBlock.ProcessInput(instancePtr);
            WsfEW_EP::SetElectronicProtect(aRcvr, instancePtr);
         }
         else if (instancePtr == nullptr)
         {
            // This is that allows for legacy type input within the receiver block. It does
            // cause a little issue if the user has mis-spelled or entered an incorrect type,
            // but it should error out when processing the input block.
            aInput.PushBack(baseType);
            instancePtr = types.Clone("WSF_ELECTRONIC_PROTECT");
            inputBlock.ProcessInput(instancePtr);
            WsfEW_EP::SetElectronicProtect(aRcvr, instancePtr);
         }
         else
         {
            // Edit the existing instance.
            aInput.PushBack(baseType);
            instancePtr->ProcessInput(aInput);
         }
      }
      return myCommand;
   }

   bool PreInitialize(double aSimTime, WsfEM_Rcvr& aRcvr) override
   {
      // Ensure the EW component is attached to EVERY receiver.
      WsfEW_RcvrComponent::FindOrCreate(aRcvr);
      return true;
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEW_EA_EP_Types& WsfEW_EA_EP_Types::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEA_EP_Types();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEW_EA_EP_Types& WsfEW_EA_EP_Types::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEA_EP_Types();
}

// =================================================================================================
WsfEW_EA_EP_Types::WsfEW_EA_EP_Types(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEW_EA_EP>(aScenario, "electronic_warfare")
{
   // Register the component factories for comm, sensor, xmtr and receiver.
   WsfEW_CommComponent::RegisterComponentFactory(aScenario);
   WsfEW_SensorComponent::RegisterComponentFactory(aScenario);
   aScenario.RegisterComponentFactory(ut::make_unique<EA_ComponentFactory>());
   aScenario.RegisterComponentFactory(ut::make_unique<EP_ComponentFactory>());

   AddCoreType("WSF_ELECTRONIC_ATTACK", ut::make_unique<WsfEW_EA>());
   AddCoreType("WSF_ELECTRONIC_PROTECT", ut::make_unique<WsfEW_EP>());
}
