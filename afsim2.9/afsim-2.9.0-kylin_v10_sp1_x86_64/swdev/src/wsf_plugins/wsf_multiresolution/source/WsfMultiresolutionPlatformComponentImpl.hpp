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

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "ComponentNameHelper.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "WsfComponentRoles.hpp"
#include "WsfMultiresolutionMultirunTable.hpp"
#include "WsfMultiresolutionPlatformComponent.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace
{
//! @brief Some useful type traits of types deriving from WsfObject. This provides members for determining if
//!    a type is named or unnamed, a platform component or signature, or a mover.
//! @tparam T Must derive from WsfObject
template<typename T>
struct ObjectTypeTraits
{
   static_assert(std::is_base_of<WsfObject, T>::value,
                 "ObjectTypeTraits may only be instantiated with types deriving from WsfObject.");

   static constexpr bool cIS_NAMED   = wsf::multiresolution::ComponentName<T>::cREQUIRES_USER_INPUT_NAME;
   static constexpr bool cIS_UNNAMED = !wsf::multiresolution::ComponentName<T>::cREQUIRES_USER_INPUT_NAME;

   static constexpr bool cIS_PLATFORM_COMPONENT          = std::is_base_of<WsfPlatformComponent, T>::value;
   static constexpr bool cIS_MOVER                       = std::is_base_of<WsfMover, T>::value;
   static constexpr bool cIS_NON_MOVER_COMPONENT         = cIS_PLATFORM_COMPONENT && !cIS_MOVER;
   static constexpr bool cIS_NAMED_NON_MOVER_COMPONENT   = cIS_NON_MOVER_COMPONENT && cIS_NAMED;
   static constexpr bool cIS_UNNAMED_NON_MOVER_COMPONENT = cIS_NON_MOVER_COMPONENT && cIS_UNNAMED;
   static constexpr bool cIS_SIGNATURE                   = std::is_base_of<WsfSignature, T>::value;
};

template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_PLATFORM_COMPONENT, std::string>::type
NameForWarning(const DerivedComponent& derivedComponent)
{
   return "component " + derivedComponent.GetComponentName();
}

template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_SIGNATURE, std::string>::type
NameForWarning(const DerivedComponent& derivedComponent)
{
   return "signature " + derivedComponent.GetName();
}

//! @brief Calls SetMover on @a aPlatform with @a aComponent if DerivedComponent is WsfMover.
// This (with the below function) can be `if constexpr` in C++17
template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_MOVER, bool>::type
SetComponentOnPlatform(WsfPlatform& aPlatform, DerivedComponent* aComponent, const std::string& /*aComponentName*/)
{
   aPlatform.SetMover(aComponent);
   return true;
}

//! @brief Calls AddComponent on @a aPlatform with @a aComponent if DerivedComponent is anything but WsfMover.
// This can be `if constexpr` in C++17
template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_NAMED_NON_MOVER_COMPONENT, bool>::type
SetComponentOnPlatform(WsfPlatform& aPlatform, DerivedComponent* aComponent, const std::string& aComponentName)
{
   aComponent->SetName(aComponentName);
   return aPlatform.AddComponent(aComponent);
}

//! @brief Calls AddComponent on @a aPlatform with @a aComponent if DerivedComponent is anything but WsfMover.
// This (with the above) can be `if constexpr` in C++17
template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_UNNAMED_NON_MOVER_COMPONENT, bool>::type
SetComponentOnPlatform(WsfPlatform& aPlatform, DerivedComponent* aComponent, const std::string& /*aComponentName*/)
{
   return aPlatform.AddComponent(aComponent);
}

//! @brief Calls AddComponent on @a aPlatform with @a aComponent if DerivedComponent is anything but WsfMover.
// This can be `if constexpr` in C++17
template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_SIGNATURE, bool>::type
SetComponentOnPlatform(WsfPlatform& aPlatform, DerivedComponent* aComponent, const std::string& /*aComponentName*/)
{
   WsfSignatureInterface* interfacePtr = aPlatform.GetSignatureList().GetInterface(DerivedComponent::cSIGNATURE_INDEX);
   if (interfacePtr)
   {
      interfacePtr->SetSignature(aComponent);
   }
   return interfacePtr != nullptr;
}

//! Call PreInitialize on component types which support it.
// (Can be replaced with  if constexpr in C++17)
template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_PLATFORM_COMPONENT, bool>::type
PreInitializeComponent(WsfPlatform* const /*aPlatform*/, DerivedComponent* const aComponent, const double aSimTime)
{
   return aComponent->PreInitialize(aSimTime);
}

template<typename DerivedComponent>
typename std::enable_if<ObjectTypeTraits<DerivedComponent>::cIS_SIGNATURE, bool>::type
PreInitializeComponent(WsfPlatform* const aPlatform, DerivedComponent* const aComponent, const double aSimTime)
{
   return aComponent->Initialize(aSimTime, aPlatform);
}

} // namespace

template<typename DerivedComponent>
WsfMultiresolutionPlatformComponent<DerivedComponent>::WsfMultiresolutionPlatformComponent()
   : WsfPlatformComponent()
   , WsfObject()
{
   SetName(wsf::multiresolution::CommandName<DerivedComponent>());
}

template<typename DerivedComponent>
const int* WsfMultiresolutionPlatformComponent<DerivedComponent>::GetComponentRoles() const
{
   static int roles[] = {cMULTIRESOLUTION_COMPONENT_ROLE, cWSF_COMPONENT_NULL};
   return roles;
}

template<typename DerivedComponent>
void* WsfMultiresolutionPlatformComponent<DerivedComponent>::QueryInterface(int aRole)
{
   if (aRole == cMULTIRESOLUTION_COMPONENT_ROLE)
   {
      return this;
   }
   return nullptr;
}

template<typename DerivedComponent>
WsfMultiresolutionPlatformComponent<DerivedComponent>*
WsfMultiresolutionPlatformComponent<DerivedComponent>::Find(const WsfPlatform& aParent)
{
   WsfMultiresolutionPlatformComponent* componentPtr = nullptr;
   aParent.GetComponents().FindByRole<WsfMultiresolutionPlatformComponent>(componentPtr);
   return componentPtr;
}

template<typename DerivedComponent>
bool WsfMultiresolutionPlatformComponent<DerivedComponent>::ProcessInput(UtInput& aInput)
{
   const std::string command(aInput.GetCommand());
   if (command == "fidelity")
   {
      aInput.ReadValue(mFidelity);
      aInput.ValueInClosedRange(mFidelity, 0.0, 1.0);
      return true;
   }
   else if (WsfPlatformComponent::ProcessInput(aInput))
   {
      return true;
   }
   else if (WsfObject::ProcessInput(aInput))
   {
      return true;
   }
   else
   {
      throw UtInput::BadValue(aInput,
                              "Unexpected command " + command + " encountered in " +
                                 wsf::multiresolution::CommandName<DerivedComponent>());
   }
}

template<typename DerivedComponent>
double WsfMultiresolutionPlatformComponent<DerivedComponent>::GetFidelity() const
{
   assert(GetComponentParent());

   const WsfMultiresolutionMultirunTable* const multirunTable =
      WsfMultiresolutionMultirunTable::Find(GetComponentParent()->GetScenario());

   std::string componentName = wsf::multiresolution::RequiresUserInputName<DerivedComponent>() ? GetName() : "";
   return multirunTable
      ->GetFidelity({GetComponentParent()->GetName(),
                     wsf::multiresolution::ComponentName<DerivedComponent>::cLOWER_CASE_NAME,
                     std::move(componentName)})
      .value_or(mFidelity);
}

template<typename DerivedComponent>
bool WsfMultiresolutionPlatformComponent<DerivedComponent>::PreInitialize(const double aSimTime)
{
   bool               ok             = WsfComponent::PreInitialize(aSimTime);
   WsfPlatform* const parentPlatform = GetComponentParent();
   ok &= parentPlatform != nullptr;
   if (ok)
   {
      std::unique_ptr<DerivedComponent> componentModel = GetModel();
      ok &= componentModel != nullptr;
      if (ok)
      {
         ok &= SetComponentOnPlatform(*parentPlatform, componentModel.get(), GetName());
         if (ok)
         {
            // Finally release pointer. Since SetComponentOnPlatform was successful, the platform takes ownership
            ok &= PreInitializeComponent(parentPlatform, componentModel.release(), aSimTime);
         }
         else
         {
            ut::log::error() << "In " << wsf::multiresolution::CommandName<DerivedComponent>() << ": Unable to add "
                             << NameForWarning(*componentModel) << " to platform with name " << parentPlatform->GetName();
            // Documentation of AddComponentP states that if AddComponent returns false, it doesn't assume ownership.
            // If this branch is reached, then componentModel will be destroyed on exit
         }
      }
   }
   return ok;
}
