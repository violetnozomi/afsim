// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFORBITALINTEGRATORTYPES_HPP
#define WSFORBITALINTEGRATORTYPES_HPP

#include "wsf_space_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOrbitalIntegrator.hpp"

class WsfOrbitalIntegratorTypes : public WsfObjectTypeList<WsfOrbitalIntegrator>
{
public:
   static WSF_SPACE_EXPORT WsfOrbitalIntegratorTypes& Get(const WsfScenario& aScenario);

   WSF_SPACE_EXPORT explicit WsfOrbitalIntegratorTypes(WsfScenario& aScenario);
};

#endif // WSFORBITALINTEGRATORTYPES_HPP
