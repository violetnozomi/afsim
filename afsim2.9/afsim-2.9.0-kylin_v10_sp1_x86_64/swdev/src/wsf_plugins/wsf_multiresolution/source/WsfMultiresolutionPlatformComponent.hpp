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

#ifndef WSF_MULTIRESOLUTION_PLATFORM_COMPONENT_HPP
#define WSF_MULTIRESOLUTION_PLATFORM_COMPONENT_HPP

#include <memory>
#include <vector>

#include "FidelityRange.hpp"
#include "UtCloneablePtr.hpp"
#include "UtOptional.hpp"
#include "WsfComponent.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
#include "WsfStringId.hpp"

class WsfScenario;
class UtInputBlock;

//! @brief A base class template for defining multiresolution WsfPlatformComponent types.
//!
//! The purpose of the base class is to manage input and model installation on a platform. The input
//! to a multiresolution model is a fidelity parameter and will be extended to other model attributes.
//! The fidelity may be specified directly in the input of this class, or externally using
//! WsfMultiresolutionMultirunTable. These inputs are used by the derived type to create a model
//! (returned from GetModel), which is then installed on the platform.
//!
//! @tparam DerivedComponent A type deriving from WsfPlatformComponent
template<typename DerivedComponent>
class WsfMultiresolutionPlatformComponent : public WsfPlatformComponent, public WsfObject
{
   static_assert(std::is_base_of<WsfPlatformComponent, DerivedComponent>::value ||
                    std::is_base_of<WsfSignature, DerivedComponent>::value,
                 "WsfMultiresolutionPlatformComponent must be instantiated with a type derived from "
                 "WsfPlatformComponent or WsfSignature");

public:
   WsfMultiresolutionPlatformComponent();

   //! @name Component infrastructure methods.
   //@{
   WsfStringId GetComponentName() const override { return GetNameId(); }
   const int*  GetComponentRoles() const override;
   void*       QueryInterface(int aRole) override;
   static WsfMultiresolutionPlatformComponent<DerivedComponent>* Find(const WsfPlatform& aParent);
   //@}

   //! @name Framework methods.
   //@{
   bool ProcessInput(UtInput& aInput) override;
   bool PreInitialize(double aSimTime) override;
   //@}

protected:
   //! The main responsibility of derived classes is to implement this method. It returns the model that will be
   //! installed on the platform by the base class.
   //! @return A model generated from inputs such as fidelity.
   virtual std::unique_ptr<DerivedComponent> GetModel() const = 0;
   //! @return The fidelity value associated with this component, which may be different from `mFidelity`.
   //! This checks with MultiresolutionMultirunTable for changes to the fidelity value in a multirun setting.
   double GetFidelity() const;

private:
   double mFidelity = 1.0;

   static constexpr int cMULTIRESOLUTION_COMPONENT_ROLE =
      cCOMPONENT_ROLE<WsfMultiresolutionPlatformComponent<DerivedComponent>>();
};

namespace wsf
{
namespace multiresolution
{
//! @return The WsfObjectTypeList list for the DerivedComponent from the WsfScenario stored in @a aInput.
template<typename DerivedComponent>
WsfObjectTypeList<DerivedComponent>& GetObjectTypeList(UtInput& aInput)
{
   WsfObjectTypeListBaseI* const componentList =
      WsfScenario::FromInput(aInput).GetTypes(wsf::multiresolution::BaseComponentName<DerivedComponent>());
   auto* const derivedComponentList = dynamic_cast<WsfObjectTypeList<DerivedComponent>*>(componentList);
   assert(derivedComponentList);
   return *derivedComponentList;
}
} // namespace multiresolution
} // namespace wsf

#include "WsfMultiresolutionPlatformComponentImpl.hpp"

#endif
