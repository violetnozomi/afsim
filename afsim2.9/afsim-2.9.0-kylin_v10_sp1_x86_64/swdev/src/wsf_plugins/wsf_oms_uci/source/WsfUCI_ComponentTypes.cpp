// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WsfUCI_ComponentTypes.hpp"

#include "UtMemory.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfUCI.hpp"
#include "WsfUCI_AMTI_Component.hpp"
#include "WsfUCI_Component.hpp"
#include "WsfUCI_Computer.hpp"
#include "WsfUCI_ESM_Component.hpp"
#include "WsfUCI_IRST_Component.hpp"
//#include "WsfUCI_SMTI_Component.hpp"
#include "WsfUCI_WeaponComponent.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
   namespace
   {
      //! Component factory to process platform input.
      class UCI_TypeFactory : public WsfComponentFactory<WsfPlatform>
      {
         public:

            bool ProcessAddOrEditCommand(UtInput&     aInput,
                                         WsfPlatform& aPlatform,
                                         bool         aIsAdding) override
            {
               UCI_ComponentTypes& types(UCI_ComponentTypes::Get(GetScenario()));
               return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<UCI_Component>());
            }

            bool ProcessDeleteCommand(UtInput&     aInput,
                                      WsfPlatform& aPlatform) override
            {
               UCI_ComponentTypes& types(UCI_ComponentTypes::Get(GetScenario()));
               return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<UCI_Component>());
            }
      };
   }

   // =================================================================================================
   //! Use the type list constructor to register itself with the scenario.
   UCI_ComponentTypes::UCI_ComponentTypes(WsfScenario& aScenario)
      : WsfObjectTypeList<UCI_Component>(aScenario, cREDEFINITION_ALLOWED, "uci_component")
   {
      Add("AMTI", ut::make_unique<UCI_AMTI_Component>(aScenario));
      Add("COMPUTER", ut::make_unique<UCI_Computer>(aScenario));
      Add("ESM", ut::make_unique<UCI_ESM_Component>(aScenario));
      Add("IRST", ut::make_unique<UCI_IRST_Component>(aScenario));
      Add("WEAPON", ut::make_unique<UCI_WeaponComponent>(aScenario));
      //Add("SMTI", ut::make_unique<UCI_SMTI_Component>(aScenario));
   }

   // =================================================================================================
   UCI_ComponentTypes& UCI_ComponentTypes::Get(WsfScenario& aScenario)
   {
      return OMS_UCI_Extension::Get(aScenario).GetUCI_ComponentTypes();
   }

   // =================================================================================================
   const UCI_ComponentTypes& UCI_ComponentTypes::Get(const WsfScenario& aScenario)
   {
      return OMS_UCI_Extension::Get(aScenario).GetUCI_ComponentTypes();
   }

   // =================================================================================================
   void UCI_ComponentTypes::RegisterComponentFactory(WsfScenario& aScenario)
   {
      aScenario.RegisterComponentFactory(ut::make_unique<UCI_TypeFactory>());
   }
}
