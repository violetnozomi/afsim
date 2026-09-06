// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommRouterProtocolTypes.hpp"

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfCommProtocolAdHoc.hpp"
#include "WsfCommProtocolLegacy.hpp"
#include "WsfCommProtocolMulticast.hpp"
#include "WsfCommProtocolOSPF.hpp"
#include "WsfCommProtocolRIPv2.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

namespace
{
bool DeleteProtocolComponent(UtInput& aInput, Router& aRouter, int aRole)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "router_protocol")
   {
      myCommand                = true;
      std::string instanceName = "router_protocol";
      aInput.ReadValue(instanceName);
      if (!aRouter.GetComponents().DeleteComponent(instanceName, aRole))
      {
         throw UtInput::BadValue(aInput, "router protocol '" + instanceName + "' does not exist");
      }
   }

   return myCommand;
}

class CommRouterProtocolComponentFactory : public WsfComponentFactory<Router>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, Router& aRouter, bool aIsAdding) override
   {
      const ProtocolTypes& types(ProtocolTypes::Get(GetScenario()));
      return types.LoadProtocolComponent(aInput, aRouter, aIsAdding, cCOMPONENT_ROLE<ProtocolInterface>(), true);
   }

   bool ProcessDeleteCommand(UtInput& aInput, Router& aRouter) override
   {
      return DeleteProtocolComponent(aInput, aRouter, cCOMPONENT_ROLE<ProtocolInterface>());
   }
};
} // namespace

// =================================================================================================
ProtocolTypes& ProtocolTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetRouterProtocolTypes();
}

// =================================================================================================
const ProtocolTypes& ProtocolTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetRouterProtocolTypes();
}

// =================================================================================================
ProtocolTypes::ProtocolTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<ProtocolInterface>(aScenario, "router_protocol")
{
   aScenario.RegisterComponentFactory(ut::make_unique<CommRouterProtocolComponentFactory>());

   //! Register all built-in protocol types.
   AddCoreType("WSF_COMM_ROUTER_PROTOCOL_AD_HOC", ut::make_unique<ProtocolAdHoc>(*GetScenario()));
   AddCoreType("WSF_COMM_ROUTER_PROTOCOL_LEGACY", ut::make_unique<ProtocolLegacy>());
   AddCoreType("WSF_COMM_ROUTER_PROTOCOL_MULTICAST", ut::make_unique<ProtocolMulticast>());
   AddCoreType("WSF_COMM_ROUTER_PROTOCOL_OSPF", ut::make_unique<ProtocolOSPF>());
   AddCoreType("WSF_COMM_ROUTER_PROTOCOL_RIPv2", ut::make_unique<router::rip::RIPv2>());
}

// =================================================================================================
bool ProtocolTypes::LoadProtocolComponent(UtInput& aInput, Router& aRouter, bool aIsAdding, int aRole, bool aIsEditable) const
{
   bool myCommand = false;
   if (aInput.GetCommand() == "router_protocol")
   {
      std::string instanceName;
      aInput.ReadValue(instanceName);

      if (aIsAdding)
      {
         std::string baseType;
         aInput.ReadValue(baseType);
         WsfObject* objectPtr(CloneP(baseType));
         if (!objectPtr)
         {
            GetDeferredInput().Requires("router_protocol", baseType);
            objectPtr = CloneP(baseType);
         }

         std::unique_ptr<ProtocolInterface> instancePtr(dynamic_cast<ProtocolInterface*>(objectPtr));
         if (instancePtr)
         {
            objectPtr->SetName(instanceName);
            instancePtr->SetComponentParent(&aRouter);

            if (aIsEditable)
            {
               instancePtr->PreInput();
               UtInputBlock inputBlock(aInput);
               inputBlock.ProcessInput(instancePtr.get());
            }
            if (aRouter.GetComponents().AddComponent(instancePtr.get()))
            {
               instancePtr.release();
               myCommand = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate router protocol: " + instanceName);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown router protocol type: " + baseType);
         }
      }
      else if (aIsEditable)
      {
         WsfComponent* instancePtr = aRouter.GetComponents().FindComponent(instanceName, aRole);

         if (instancePtr)
         {
            UtInputBlock inputBlock(aInput);
            inputBlock.ProcessInput(instancePtr);
            myCommand = true;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown router protocol: " + instanceName);
         }
      }
      else
      {
         throw UtInput::OutOfContext(aInput, "Cannot edit router protocol");
      }
   }
   return myCommand;
}

} // namespace router
} // namespace comm
} // namespace wsf
