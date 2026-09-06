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

#include "WsfPlatformTypes.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfPlatformTypes& WsfPlatformTypes::Get(WsfScenario& aScenario)
{
   return aScenario.GetPlatformTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfPlatformTypes& WsfPlatformTypes::Get(const WsfScenario& aScenario)
{
   return aScenario.GetPlatformTypes();
}

// =================================================================================================
WsfPlatformTypes::WsfPlatformTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfPlatform>(aScenario, "platform_type")
{
   Add("WSF_PLATFORM", ut::make_unique<WsfPlatform>(aScenario));
}

// =================================================================================================
//! Load a new 'type object' from an input stream.
//! Process the current command if it is one that defines a new 'type object' of this class.
//! @param aInput The input stream.
//! @return 'true' if the command was processed or 'false' if not.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
WsfPlatformTypes::LoadResult WsfPlatformTypes::LoadType(UtInput& aInput)
{
   LoadResult result;
   // return GetObjectTypeList().LoadType(aInput, "platform_type") != 0;

   if (aInput.GetCommand() == "platform_type")
   {
      UtInput::StoredLocation loc = aInput.StoreLocation();

      result.mIsCommandProcessed = true;
      UtInputBlock inputBlock(aInput);
      std::string  derivedType;
      aInput.ReadValue(derivedType);

      if (!GetScenario()->GetDeferredInput().IsDelayLoading())
      {
         std::string cmd;
         while (inputBlock.ReadCommand(cmd))
            ;

         GetDeferredInput().DelayLoad("platform_type",
                                      derivedType,
                                      loc,
                                      std::bind(&WsfPlatformTypes::ProcessInput, this, std::placeholders::_1));
         return result;
      }

      std::string baseType;
      aInput.ReadValue(baseType);
      if (Find(baseType) == nullptr)
      {
         GetScenario()->GetDeferredInput().Requires("platform_type", baseType);
      }

      std::unique_ptr<WsfPlatform> derivedPtr(Clone(baseType));
      if (derivedPtr.get() != nullptr)
      {
         derivedPtr->ProcessInputBlock(inputBlock, true);
         result.mObjectTypePtr = derivedPtr.get();
         if (!Add(derivedType, std::move(derivedPtr)))
         {
            throw UtInput::BadValue(aInput, "Duplicate platform_type: " + derivedType);
         }
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown platform_type: " + baseType);
      }
   }
   return result;
}

// =================================================================================================
bool WsfPlatformTypes::ProcessInput(UtInput& aInput)
{
   return LoadType(aInput);
}

// =================================================================================================
void WsfPlatformTypes::CompleteLoad()
{
   TypeIdList ids;
   GetTypeIds(ids);
   for (size_t i = 0; i < ids.size(); ++i)
   {
      WsfPlatform* platformPtr = Find(ids[i]);
      if (platformPtr != nullptr)
      {
         platformPtr->CompleteLoad(*GetScenario());
      }
   }
}
