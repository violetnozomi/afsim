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

#include "WsfCommProtocolTypes.hpp"

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComm.hpp"
#include "WsfCommProtocolIGMP.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{

namespace
{
bool DeleteProtocolComponent(UtInput& aInput, Comm& aComm, int aRole)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "protocol")
   {
      myCommand                = true;
      std::string instanceName = "protocol";
      aInput.ReadValue(instanceName);
      if (!aComm.GetComponents().DeleteComponent(instanceName, aRole))
      {
         throw UtInput::BadValue(aInput, "protocol '" + instanceName + "' does not exist");
      }
   }

   return myCommand;
}

class CommProtocolComponentFactory : public WsfComponentFactory<wsf::comm::Comm>
{
public:
   bool ProcessAddOrEditCommand(UtInput& aInput, wsf::comm::Comm& aComm, bool aIsAdding) override
   {
      const ProtocolTypes& types(ProtocolTypes::Get(GetScenario()));
      return types.LoadProtocolComponent(aInput, aComm, aIsAdding, cCOMPONENT_ROLE<ProtocolInterface>(), true);
   }

   bool ProcessDeleteCommand(UtInput& aInput, wsf::comm::Comm& aComm) override
   {
      return DeleteProtocolComponent(aInput, aComm, cCOMPONENT_ROLE<ProtocolInterface>());
   }
};
} // namespace

// =================================================================================================
ProtocolTypes& ProtocolTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetProtocolTypes();
}

// =================================================================================================
const ProtocolTypes& ProtocolTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetProtocolTypes();
}

// =================================================================================================
ProtocolTypes::ProtocolTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<ProtocolInterface>(aScenario, "protocol")
{
   aScenario.RegisterComponentFactory(ut::make_unique<CommProtocolComponentFactory>());

   //! Register all built-in protocol types.
   AddCoreType("WSF_COMM_PROTOCOL_IGMP", ut::make_unique<ProtocolIGMP>());
}

// =================================================================================================
bool ProtocolTypes::LoadProtocolComponent(UtInput& aInput, Comm& aComm, bool aIsAdding, int aRole, bool aIsEditable) const
{
   bool myCommand = false;
   if (aInput.GetCommand() == "protocol")
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
            GetDeferredInput().Requires("protocol", baseType);
            objectPtr = CloneP(baseType);
         }

         std::unique_ptr<ProtocolInterface> instancePtr(dynamic_cast<ProtocolInterface*>(objectPtr));
         if (instancePtr)
         {
            objectPtr->SetName(instanceName);
            instancePtr->SetComponentParent(&aComm);

            if (aIsEditable)
            {
               instancePtr->PreInput();
               UtInputBlock inputBlock(aInput);
               inputBlock.ProcessInput(instancePtr.get());
            }
            if (aComm.GetComponents().AddComponent(instancePtr.get()))
            {
               instancePtr.release();
               myCommand = true;
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate protocol: " + instanceName);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown protocol type: " + baseType);
         }
      }
      else if (aIsEditable)
      {
         WsfComponent* instancePtr = aComm.GetComponents().FindComponent(instanceName, aRole);

         if (instancePtr)
         {
            UtInputBlock inputBlock(aInput);
            inputBlock.ProcessInput(instancePtr);
            myCommand = true;
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown protocol: " + instanceName);
         }
      }
      else
      {
         throw UtInput::OutOfContext(aInput, "Cannot edit protocol");
      }
   }
   return myCommand;
}

} // namespace comm
} // namespace wsf
