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

#ifndef WSFCYBEREFFECTTYPES_HPP
#define WSFCYBEREFFECTTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "effects/WsfCyberEffect.hpp"

namespace wsf
{
namespace cyber
{

class EffectTypes : public WsfObjectTypeList<Effect>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_CYBER_EXPORT EffectTypes&       Get(WsfScenario& aScenario);
   static WSF_CYBER_EXPORT const EffectTypes& Get(const WsfScenario& aScenario);
   //@}

   EffectTypes(WsfScenario& aScenario);
};

} // namespace cyber
} // namespace wsf

#endif
