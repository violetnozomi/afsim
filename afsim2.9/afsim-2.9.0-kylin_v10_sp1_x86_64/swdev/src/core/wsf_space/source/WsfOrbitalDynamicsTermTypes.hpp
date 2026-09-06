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

#ifndef WSFORBITALDYNAMICSTERMTYPES_HPP
#define WSFORBITALDYNAMICSTERMTYPES_HPP

#include "wsf_space_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"

class WsfOrbitalDynamicsTermTypes : public WsfObjectTypeList<WsfOrbitalDynamicsTerm>
{
public:
   static WSF_SPACE_EXPORT WsfOrbitalDynamicsTermTypes& Get(const WsfScenario& aScenario);

   WSF_SPACE_EXPORT explicit WsfOrbitalDynamicsTermTypes(WsfScenario& aScenario);
};

#endif // WSFORBITALDYNAMICSTERMTYPES_HPP
