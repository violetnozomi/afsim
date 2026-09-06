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

#include "WsfCommRouterTypes.hpp"

#include "UtStringIdLiteral.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

namespace
{
//! Component factory to process platform input.
class CommRouterComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   // All platforms have at least a "default" router
   bool PreInitialize(double aSimTime, WsfPlatform& aPlatform) override
   {
      //! Note: This occurs AFTER user defined types are added to the platform.
      //! Thus, the user may define their own default router with appropriate
      //! settings before this one is added, assuming the name "default" is
      //! used for that user defined router.
      if (aPlatform.GetComponent<Router>(UtStringIdLiteral("default")) == nullptr)
      {
         std::unique_ptr<Router> routerPtr = ut::make_unique<Router>(GetScenario());
         routerPtr->SetName(UtStringIdLiteral("default"));
         routerPtr->SetType("WSF_COMM_ROUTER");
         aPlatform.AddComponent(routerPtr.release());
      }

      return true;
   }

   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      Types& types(Types::Get(GetScenario()));
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cCOMPONENT_ROLE<Router>());
   }

   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      Types& types(Types::Get(GetScenario()));
      return types.DeleteNamedComponent(aInput, aPlatform, cCOMPONENT_ROLE<Router>());
   }
};
} // namespace

// =================================================================================================
//! Return a modifiable reference to the type list associated with the specified scenario.
Types& Types::Get(WsfScenario& aScenario)
{
   return aScenario.GetRouterTypes();
}

// =================================================================================================
//! Return a const reference to the type list associated with the specified scenario.
const Types& Types::Get(const WsfScenario& aScenario)
{
   return aScenario.GetRouterTypes();
}

// =================================================================================================
Types::Types(WsfScenario& aScenario)
   : WsfObjectTypeList<Router>(aScenario, cREDEFINITION_ALLOWED, "router")
{
   aScenario.RegisterComponentFactory(ut::make_unique<CommRouterComponentFactory>());

   AddCoreType("WSF_COMM_ROUTER", ut::make_unique<router::Router>(aScenario));
}

} // namespace router
} // namespace comm
} // namespace wsf
