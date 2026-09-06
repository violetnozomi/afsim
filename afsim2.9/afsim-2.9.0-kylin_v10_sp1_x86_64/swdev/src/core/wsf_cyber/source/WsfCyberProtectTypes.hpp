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

#ifndef WSFCYBERPROTECTTYPES_HPP
#define WSFCYBERPROTECTTYPES_HPP

#include "WsfCyberProtect.hpp"
#include "WsfObjectTypeList.hpp"

namespace wsf
{
namespace cyber
{

class ProtectTypes : public WsfObjectTypeList<Protect>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_CYBER_EXPORT ProtectTypes&       Get(WsfScenario& aScenario);
   static WSF_CYBER_EXPORT const ProtectTypes& Get(const WsfScenario& aScenario);
   //@}

   ProtectTypes(WsfScenario& aScenario);
};

} // namespace cyber
} // namespace wsf

#endif
