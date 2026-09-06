// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERCONSTRAINTTYPES_HPP
#define WSFCYBERCONSTRAINTTYPES_HPP

#include "WsfCyberConstraint.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace cyber
{
class ConstraintTypes : public WsfObjectTypeList<Constraint>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_CYBER_EXPORT ConstraintTypes&       Get(WsfScenario& aScenario);
   static WSF_CYBER_EXPORT const ConstraintTypes& Get(const WsfScenario& aScenario);
   //@}

   ConstraintTypes(WsfScenario& aScenario);

   static void RegisterComponentFactory(WsfScenario& aScenario);
};

} // namespace cyber

} // namespace wsf

#endif
