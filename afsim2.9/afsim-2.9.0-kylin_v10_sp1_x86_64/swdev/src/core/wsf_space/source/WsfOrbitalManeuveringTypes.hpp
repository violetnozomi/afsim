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

#ifndef WSFORBITALMANEUVERINGTYPES_HPP
#define WSFORBITALMANEUVERINGTYPES_HPP

#include "wsf_space_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOrbitalManeuvering.hpp"

class WsfOrbitalManeuveringTypes : public WsfObjectTypeList<WsfOrbitalManeuvering>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_SPACE_EXPORT WsfOrbitalManeuveringTypes& Get(const WsfScenario& aScenario);
   //@}

   WSF_SPACE_EXPORT explicit WsfOrbitalManeuveringTypes(WsfScenario& aScenario);
};
#endif
