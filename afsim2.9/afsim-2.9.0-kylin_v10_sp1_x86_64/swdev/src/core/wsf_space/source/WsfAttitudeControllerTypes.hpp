// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFATTITUDECONTROLLERTYPES_HPP
#define WSFATTITUDECONTROLLERTYPES_HPP

#include "wsf_space_export.h"

#include "WsfAttitudeController.hpp"
#include "WsfObjectTypeList.hpp"

class WsfAttitudeControllerTypes : public WsfObjectTypeList<WsfAttitudeController>
{
public:
   static WSF_SPACE_EXPORT WsfAttitudeControllerTypes& Get(const WsfScenario& aScenario);

   WSF_SPACE_EXPORT explicit WsfAttitudeControllerTypes(WsfScenario& aScenario);
};

#endif // WSFATTITUDECONTROLLERTYPES_HPP
