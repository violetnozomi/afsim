// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSENSORTYPES_HPP
#define WSFSENSORTYPES_HPP

#include "WsfObjectTypeList.hpp"
#include "WsfSensor.hpp"

class WsfSensorTypes : public WsfObjectTypeList<WsfSensor>
{
public:
   //! @name Static methods to return a reference to the type list associated with a scenario.
   //@{
   static WSF_EXPORT WsfSensorTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfSensorTypes& Get(const WsfScenario& aScenario);
   //@}

   WsfSensorTypes(WsfScenario& aScenario);
};

#endif
