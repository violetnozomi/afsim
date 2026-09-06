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

#include "WsfZoneTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfZoneDefinition.hpp"
#include "WsfZoneReference.hpp"
#include "WsfZoneSet.hpp"

namespace
{
//! Component factory to process platform input.
class ZoneComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   bool ProcessInput(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfZoneTypes& types(WsfZoneTypes::Get(GetScenario()));
      return types.LoadInstance(aInput, &aPlatform);
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      WsfZoneTypes& types(WsfZoneTypes::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<WsfZone>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfZoneTypes& WsfZoneTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetZoneTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfZoneTypes& WsfZoneTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetZoneTypes();
}

// =================================================================================================
WsfZoneTypes::WsfZoneTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfZone>(aScenario, WsfObjectTypeListBase::cREDEFINITION_ALLOWED, "zone")
{
   aScenario.RegisterComponentFactory(ut::make_unique<ZoneComponentFactory>());

   AddCoreType("WSF_ZONE", ut::make_unique<WsfZoneDefinition>());
}

// =================================================================================================
bool WsfZoneTypes::LoadInstance(UtInput& aInput, WsfPlatform* aPlatformPtr)
{
   bool        myCommand = false;
   std::string command(aInput.GetCommand());

   if (command == "zone")
   {
      myCommand = true;
      std::unique_ptr<WsfZone> zonePtr(ProcessZoneInput(aInput));
      aPlatformPtr->DeleteComponent<WsfZone>(zonePtr->GetNameId()); // Delete any existing zone with same name
      if (!aPlatformPtr->AddComponent(zonePtr.get()))               // Shouldn't fail as same name was just deleted
      {
         throw UtInput::BadValue(aInput,
                                 "Duplicate zone " + zonePtr->GetName() + " on platform " + aPlatformPtr->GetName());
      }
      zonePtr.release();
   }
   else if (command == "zone_set")
   {
      myCommand = true;
      std::unique_ptr<WsfZone> zonePtr(ProcessZoneSetInput(aInput));
      aPlatformPtr->DeleteComponent<WsfZone>(zonePtr->GetNameId()); // Delete any existing zone with same name
      if (!aPlatformPtr->AddComponent(zonePtr.get()))               // Shouldn't fail as same name was just deleted
      {
         throw UtInput::BadValue(aInput,
                                 "Duplicate zone " + zonePtr->GetName() + " on platform " + aPlatformPtr->GetName());
      }
      zonePtr.release();
   }
   else if (command == "use_zone")
   {
      myCommand = true;

      std::string globalZoneName;
      aInput.ReadValue(globalZoneName);

      // Check the global zone name
      if (Find(globalZoneName) == nullptr)
      {
         throw UtInput::BadValue(aInput, "Global zone does not exist: " + globalZoneName);
      }

      std::string as;       // This is a throwaway string which should contain "as",
      aInput.ReadValue(as); // just for a more readable syntax.

      std::string localZoneName;
      aInput.ReadValue(localZoneName);

      std::unique_ptr<WsfZone> zonePtr(new WsfZoneReference(globalZoneName));
      zonePtr->SetName(localZoneName);
      zonePtr->SetGlobalName(globalZoneName);
      aPlatformPtr->DeleteComponent<WsfZone>(zonePtr->GetNameId()); // Delete any existing zone with same name
      if (!aPlatformPtr->AddComponent(zonePtr.get()))               // Shouldn't fail as same name was just deleted
      {
         throw UtInput::BadValue(aInput, "Duplicate zone name: " + localZoneName);
      }
      zonePtr.release();
   }

   return myCommand;
}

// =================================================================================================
//! Load a new 'type object' from an input stream.
//! Process the current command if it is one that defines a new 'type object' of this class.
//! @param aInput The input stream.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
WsfZoneTypes::LoadResult WsfZoneTypes::LoadType(UtInput& aInput)
{
   LoadResult result;

   std::string command(aInput.GetCommand());
   if (command == "zone")
   {
      std::unique_ptr<WsfZone> zonePtr(ProcessZoneInput(aInput));
      result.mObjectTypePtr = zonePtr.get();
      if (!Add(result.mObjectTypePtr->GetName(), std::move(zonePtr)))
      {
         // Shouldn't occur because redefinition is allowed.
         throw UtInput::BadValue(aInput, "Duplicate global zone name: " + result.mObjectTypePtr->GetName());
      }
      result.mIsCommandProcessed = true;
   }
   else if (command == "zone_set")
   {
      std::unique_ptr<WsfZone> zonePtr(ProcessZoneSetInput(aInput));
      result.mObjectTypePtr = zonePtr.get();
      if (!Add(result.mObjectTypePtr->GetName(), std::move(zonePtr)))
      {
         // Shouldn't occur because redefinition is allowed.
         throw UtInput::BadValue(aInput, "Duplicate global zone name: " + result.mObjectTypePtr->GetName());
      }
      result.mIsCommandProcessed = true;
   }

   return result;
}

// =================================================================================================
// private
WsfZone* WsfZoneTypes::ProcessZoneInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);
   std::string  zoneName;
   aInput.ReadValue(zoneName);
   std::unique_ptr<WsfZoneDefinition> zonePtr(new WsfZoneDefinition());
   zonePtr->SetName(zoneName);
   inputBlock.ProcessInput(zonePtr.get());
   return zonePtr.release();
}

// =================================================================================================
// private
WsfZone* WsfZoneTypes::ProcessZoneSetInput(UtInput& aInput)
{
   UtInputBlock inputBlock(aInput);
   std::string  zoneName;
   aInput.ReadValue(zoneName);
   std::unique_ptr<WsfZone> zonePtr(new WsfZoneSet());
   zonePtr->SetName(zoneName);
   inputBlock.ProcessInput(zonePtr.get());
   return zonePtr.release();
}

// =================================================================================================
void WsfZoneTypes::InitializeZones(WsfScenario& aScenario)
{
   ObjectPtrList types;
   GetCurrentTypes(types);
   for (auto& type : types)
   {
      type->Initialize(aScenario);
   }
}
