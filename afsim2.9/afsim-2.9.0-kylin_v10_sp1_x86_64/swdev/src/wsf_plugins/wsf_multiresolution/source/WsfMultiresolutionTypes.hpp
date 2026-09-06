// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSF_MULTIRESOLUTION_TYPES_HPP
#define WSF_MULTIRESOLUTION_TYPES_HPP

#include <unordered_map>

#include "ComponentNameHelper.hpp"
#include "MultiresolutionRoles.hpp"
#include "WsfComponentFactory.hpp"
#include "WsfMultiresolutionPlatformComponent.hpp"
#include "WsfMultiresolutionWrapperMetaModel.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfPlatform.hpp"

class WsfScenario;

template<typename DerivedComponent>
class WsfMultiresolutionTypes : public WsfObjectTypeList<WsfMultiresolutionPlatformComponent<DerivedComponent>>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WsfMultiresolutionTypes<DerivedComponent>&       Get(WsfScenario& aScenario);
   static const WsfMultiresolutionTypes<DerivedComponent>& Get(const WsfScenario& aScenario);
   //@}

   explicit WsfMultiresolutionTypes(WsfScenario& aScenario);
};

class WsfMultiresolutionTypesRegistration : public WsfScenarioExtension
{
public:
   void AddedToScenario() override;

   //! @throw std::bad_cast
   //! @throw std::out_of_range
   template<typename DerivedComponent>
   WsfMultiresolutionTypes<DerivedComponent>& GetMultiresolutionTypes() const
   {
      const size_t index = mRoleIndexMap.at(cCOMPONENT_ROLE<WsfMultiresolutionPlatformComponent<DerivedComponent>>());
      auto& types = dynamic_cast<WsfMultiresolutionTypes<DerivedComponent>&>(*GetScenario().GetTypeLists().at(index));
      return types;
   }

private:
   template<typename DerivedComponent>
   void AddMultiresolutionType()
   {
      mRoleIndexMap[cCOMPONENT_ROLE<WsfMultiresolutionPlatformComponent<DerivedComponent>>()] =
         GetScenario().GetTypeLists().size();
      GetScenario().AddTypeList(ut::make_unique<WsfMultiresolutionTypes<DerivedComponent>>(GetScenario()));
   }

   std::unordered_map<int, size_t> mRoleIndexMap;
};

namespace impl
{
template<typename DerivedComponent>
class MultiresolutionComponentFactory : public WsfComponentFactory<WsfPlatform>
{
public:
   static constexpr int cMULTIRESOLUTION_COMPONENT_ROLE =
      cCOMPONENT_ROLE<WsfMultiresolutionPlatformComponent<DerivedComponent>>();

   bool ProcessAddOrEditCommand(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding) override
   {
      return ProcessAddOrEditCommandImpl(aInput, aPlatform, aIsAdding);
   }
   bool ProcessDeleteCommand(UtInput& aInput, WsfPlatform& aPlatform) override
   {
      return ProcessDeleteCommandImpl(aInput, aPlatform);
   }

private:
   // The following functions can be replaced with `if constexpr` in C++17
   template<typename U = DerivedComponent>
   typename std::enable_if<wsf::multiresolution::ComponentName<U>::cREQUIRES_USER_INPUT_NAME, bool>::type
   ProcessAddOrEditCommandImpl(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding)
   {
      auto& types = WsfMultiresolutionTypes<DerivedComponent>::Get(GetScenario());
      return types.LoadNamedComponent(aInput, aPlatform, aIsAdding, cMULTIRESOLUTION_COMPONENT_ROLE);
   }
   template<typename U = DerivedComponent>
   typename std::enable_if<!wsf::multiresolution::ComponentName<U>::cREQUIRES_USER_INPUT_NAME, bool>::type
   ProcessAddOrEditCommandImpl(UtInput& aInput, WsfPlatform& aPlatform, bool aIsAdding)
   {
      auto& types = WsfMultiresolutionTypes<DerivedComponent>::Get(GetScenario());
      return types.LoadUnnamedComponent(aInput, aPlatform, aIsAdding, cMULTIRESOLUTION_COMPONENT_ROLE);
   }
   template<typename U = DerivedComponent>
   typename std::enable_if<wsf::multiresolution::ComponentName<U>::cREQUIRES_USER_INPUT_NAME, bool>::type
   ProcessDeleteCommandImpl(UtInput& aInput, WsfPlatform& aPlatform)
   {
      auto& types = WsfMultiresolutionTypes<DerivedComponent>::Get(GetScenario());
      return types.DeleteNamedComponent(aInput, aPlatform, cMULTIRESOLUTION_COMPONENT_ROLE);
   }
   template<typename U = DerivedComponent>
   typename std::enable_if<!wsf::multiresolution::ComponentName<U>::cREQUIRES_USER_INPUT_NAME, bool>::type
   ProcessDeleteCommandImpl(UtInput& aInput, WsfPlatform& aPlatform)
   {
      auto& types = WsfMultiresolutionTypes<DerivedComponent>::Get(GetScenario());
      return types.DeleteUnnamedComponent(aInput, aPlatform, cMULTIRESOLUTION_COMPONENT_ROLE);
   }
};
} // namespace impl

template<typename DerivedComponent>
WsfMultiresolutionTypes<DerivedComponent>& WsfMultiresolutionTypes<DerivedComponent>::Get(WsfScenario& aScenario)
{
   auto& registration =
      dynamic_cast<WsfMultiresolutionTypesRegistration&>(aScenario.GetExtension(wsf::multiresolution::cEXTENSION_NAME));
   return registration.GetMultiresolutionTypes<DerivedComponent>();
}

template<typename DerivedComponent>
const WsfMultiresolutionTypes<DerivedComponent>& WsfMultiresolutionTypes<DerivedComponent>::Get(const WsfScenario& aScenario)
{
   const auto& registration = dynamic_cast<const WsfMultiresolutionTypesRegistration&>(
      aScenario.GetExtension(wsf::multiresolution::cEXTENSION_NAME));
   return registration.GetMultiresolutionTypes<DerivedComponent>();
}

template<typename DerivedComponent>
WsfMultiresolutionTypes<DerivedComponent>::WsfMultiresolutionTypes(WsfScenario& aScenario)
   : WsfObjectTypeList<WsfMultiresolutionPlatformComponent<DerivedComponent>>(
        aScenario,
        WsfObjectTypeListBase::cREDEFINITION_ALLOWED,
        wsf::multiresolution::CommandName<DerivedComponent>())
{
   aScenario.RegisterComponentFactory(ut::make_unique<impl::MultiresolutionComponentFactory<DerivedComponent>>());
   this->Add(wsf::multiresolution::BaseTypeName<DerivedComponent>(),
             ut::make_unique<WsfMultiresolutionWrapperMetaModel<DerivedComponent>>());
}

#endif
