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

#ifndef WSFCOMMMEDIUMTYPES_HPP
#define WSFCOMMMEDIUMTYPES_HPP

#include "wsf_export.h"

#include <memory>

#include "WsfCommMedium.hpp"
#include "WsfCommMediumFactory.hpp"
#include "WsfObjectTypeList.hpp"
class WsfScenario;

namespace wsf
{
namespace comm
{
namespace medium
{

class Container;

//! This type list is unique, due to how mediums are managed
//! and shared across objects in the comm framework.
//!
//! Mediums should not be cloned outside of this type list.
//! Each medium is maintained here, and shared references
//! are provided to any object requiring it.
class Types : public WsfObjectTypeList<Medium>
{
public:
   static WSF_EXPORT Types& Get(const WsfScenario& aScenario);

   explicit Types(WsfScenario& aScenario);
   ~Types() override = default;

   //! Returns a instantiated medium being utilized.
   WSF_EXPORT std::shared_ptr<Medium> GetInstance(const std::string& aType) const;

   //! Mediums require custom load/unload methods.
   //! These are input methods for the handling
   //! of correct instantiation/assignment/removal
   //! of mediums for scenario input processing.
   //@{
   WSF_EXPORT bool LoadMedium(UtInput& aInput, Container& aContainer, bool aIsAdding);

   WSF_EXPORT bool DeleteMedium(UtInput& aInput, Container& aContainer);

   //! The AddDefaultMedium method should be
   //! utilized for every instance of a container
   //! that does not have a medium prior to
   //! initialization. The mediums used in this
   //! way are unique to each instance of the container,
   //! and will not contain a copy in the typelist.
   //! Thus, these mediums will be deallocated when
   //! the container is destroyed, unlike formally,
   //! user defined mediums.
   WSF_EXPORT bool AddDefaultMedium(Container& aContainer);
   //@}

   WSF_EXPORT FactoryManager&       GetManager() { return mManager; }
   WSF_EXPORT const FactoryManager& GetManager() const { return mManager; }

private:
   //! The list of required medium instantiations.
   std::vector<std::shared_ptr<Medium>> mInstances{};

   //! The manager for handling medium factories and related input.
   FactoryManager mManager;
};

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
