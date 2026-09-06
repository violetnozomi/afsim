// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERATTACKTYPES_HPP
#define WSFCYBERATTACKTYPES_HPP

#include "WsfCyberAttack.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace cyber
{

class AttackTypes : public WsfObjectTypeList<Attack>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_CYBER_EXPORT AttackTypes&       Get(WsfScenario& aScenario);
   static WSF_CYBER_EXPORT const AttackTypes& Get(const WsfScenario& aScenario);
   //@}

   AttackTypes(WsfScenario& aScenario);
};

} // namespace cyber
} // namespace wsf

#endif
