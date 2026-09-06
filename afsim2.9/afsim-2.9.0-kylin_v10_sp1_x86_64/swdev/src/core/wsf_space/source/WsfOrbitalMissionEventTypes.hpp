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

#ifndef WSFORBITALMISSIONEVENTTYPES_HPP
#define WSFORBITALMISSIONEVENTTYPES_HPP

#include "wsf_space_export.h"

#include "WsfObjectTypeList.hpp"
#include "WsfOrbitalEvent.hpp"
#include "WsfOrbitalManeuver.hpp"

//! A types factory used to store prototypes for all mission event types and make copies of them as needed.
class WsfOrbitalMissionEventTypes : public WsfObjectTypeList<WsfOrbitalEvent>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_SPACE_EXPORT WsfOrbitalMissionEventTypes& Get(const WsfScenario& aScenario);
   //@}

   WSF_SPACE_EXPORT explicit WsfOrbitalMissionEventTypes(WsfScenario& aScenario);
};
#endif
