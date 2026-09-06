// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFUCI_COMPONENTTYPES_HPP
#define WSFUCI_COMPONENTTYPES_HPP

#include "wsf_oms_uci_export.h"
#include "WsfObjectTypeList.hpp"

#include "WsfScenario.hpp"
#include "WsfUCI_Component.hpp"

namespace wsf
{
   //! A type that only uses functionality of base class to register itself with the scenario.
   class UCI_ComponentTypes : public WsfObjectTypeList<UCI_Component>
   {
      public:
         explicit UCI_ComponentTypes(WsfScenario& aScenario);
         static void RegisterComponentFactory(WsfScenario& aScenario);
         static WSF_OMS_UCI_EXPORT UCI_ComponentTypes& Get(WsfScenario& aScenario);
         static WSF_OMS_UCI_EXPORT const UCI_ComponentTypes& Get(const WsfScenario& aScenario);
   };
   
} // namespace wsf

#endif
