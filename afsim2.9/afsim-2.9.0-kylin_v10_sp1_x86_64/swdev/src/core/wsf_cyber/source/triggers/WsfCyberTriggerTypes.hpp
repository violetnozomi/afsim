// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRIGGERTYPES_HPP
#define WSFCYBERTRIGGERTYPES_HPP

#include "wsf_cyber_export.h"

#include "WsfObjectTypeList.hpp"
#include "triggers/WsfCyberTrigger.hpp"

namespace wsf
{
namespace cyber
{

class TriggerTypes : public WsfObjectTypeList<trigger::Trigger>
{
public:
   static WSF_CYBER_EXPORT TriggerTypes&       Get(WsfScenario& aScenario);
   static WSF_CYBER_EXPORT const TriggerTypes& Get(const WsfScenario& aScenario);

   TriggerTypes(WsfScenario& aScenario);

   static void RegisterComponentFactory(WsfScenario& aScenario);
};

} // namespace cyber
} // namespace wsf

#endif
