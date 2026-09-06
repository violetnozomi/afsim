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

#include "WsfEW_TechniqueTypes.hpp"

#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "WsfDeferredInput.hpp"
#include "WsfEW_EA.hpp"
#include "WsfEW_EP.hpp"
#include "WsfMil.hpp"
#include "WsfScenario.hpp"

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
WsfEW_TechniqueTypes& WsfEW_TechniqueTypes::Get(WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEW_TechniqueTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const WsfEW_TechniqueTypes& WsfEW_TechniqueTypes::Get(const WsfScenario& aScenario)
{
   return WsfMilExtension::Get(aScenario).GetEW_TechniqueTypes();
}

// =================================================================================================
WsfEW_TechniqueTypes::WsfEW_TechniqueTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfEW_Technique>(aScenario, "electronic_warfare_technique")
{
   Add("WSF_EA_TECHNIQUE", ut::make_unique<WsfEW_EA::EA_Technique>());
   Add("WSF_EP_TECHNIQUE", ut::make_unique<WsfEW_EP::EP_Technique>());
}

// =================================================================================================
//! Load an instance of a 'type object' of this class
//! Process the current command if it would create an instance of an 'type object' of this class.
//! @param aInput The input stream.
//! @param aEA_EP_Ptr The object into which the instance is to be attached.
//! @returns true if the command was recognized as one for instantiating an object of this type.
//! @throws UtInput::ExceptionBase (or derived class) if an error was encountered.
bool WsfEW_TechniqueTypes::LoadInstance(UtInput& aInput, WsfEW_EA_EP* aEA_EP_Ptr) const
{
   bool myCommand = false;
   if (aInput.GetCommand() == "technique")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      std::string  instanceName;
      aInput.ReadValue(instanceName);
      if (!aEA_EP_Ptr->TechniqueIsAvailable(WsfStringId(instanceName)))
      {
         // Adding a new instance
         std::string baseType;
         aInput.ReadValue(baseType);
         GetScenario()->GetDeferredInput().MaybeRequires("electronic_warfare_technique", baseType);
         if (Find(baseType) != nullptr)
         {
            std::unique_ptr<WsfEW_Technique> instancePtr(Clone(baseType));
            if (((dynamic_cast<WsfEW_EA*>(aEA_EP_Ptr) != nullptr) && (!instancePtr->IsA_TypeOf("WSF_EA_TECHNIQUE"))) ||
                ((dynamic_cast<WsfEW_EP*>(aEA_EP_Ptr) != nullptr) && (!instancePtr->IsA_TypeOf("WSF_EP_TECHNIQUE"))))
            {
               throw UtInput::BadValue(aInput, "EA/EP Technique: " + instanceName + " is of wrong base type.");
            }

            instancePtr->SetName(instanceName);
            inputBlock.ProcessInput(instancePtr.get());
            if (aEA_EP_Ptr->AddTechnique(instancePtr.get()))
            {
               instancePtr.release();
            }
            else
            {
               throw UtInput::BadValue(aInput, "Duplicate EW technique: " + instanceName);
            }
         }
         else // Allow for legacy inputs
         {
            // This section is bit of a hack to allow for legacy type input with the
            // receiver and transmitter EA and EP blocks. Does cause a little issue
            // if the user has mis-spelled or entered an incorrect type, but should
            // then error out when processing the input block.
            aInput.PushBack(baseType);
            if (dynamic_cast<WsfEW_EA*>(aEA_EP_Ptr) != nullptr)
            {
               baseType = "WSF_EA_TECHNIQUE";
            }
            else if (dynamic_cast<WsfEW_EP*>(aEA_EP_Ptr) != nullptr)
            {
               baseType = "WSF_EP_TECHNIQUE";
            }

            std::unique_ptr<WsfEW_Technique> instancePtr2(Clone(baseType));
            if (instancePtr2 != nullptr)
            {
               // cout << "****** WARNING EW technique '"<< instanceName << ":" << baseType
               //      << "' loading legacy technique type (inline non-type defined) definition from input." << endl;
               instancePtr2->SetName(instanceName);
               inputBlock.ProcessInput(instancePtr2.get());
               if (aEA_EP_Ptr->AddTechnique(instancePtr2.get()))
               {
                  instancePtr2.release();
               }
               else
               {
                  throw UtInput::BadValue(aInput, "Duplicate EW technique: " + instanceName);
               }
            }
         }
         // else
         //{
         //    throw UtInput::BadValue(aInput, "Unknown EW technique type: " + baseType);
         // }
      }
      else
      {
         // Edit an existing instance
         WsfEW_Technique* instancePtr = aEA_EP_Ptr->GetTechnique(WsfStringId(instanceName));
         if (instancePtr != nullptr)
         {
            std::string baseType;
            aInput.ReadValue(baseType);
            if (Find(baseType) == nullptr)
            {
               aInput.PushBack(baseType);
               inputBlock.ProcessInput(instancePtr);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Redefining an existing technique from type: " + instanceName);
            }
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown EW technique: " + instanceName);
         }
      }
   }
   return myCommand;
}
