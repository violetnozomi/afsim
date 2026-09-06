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

#include "WsfEarthMonopoleTerm.hpp"

#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"

bool WsfEarthMonopoleTerm::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command = aInput.GetCommand();
   if (command == "wgs84")
   {
      mGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER_WGS84;
   }
   else if (command == "egm96")
   {
      mGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER;
   }
   else if (command == "gravitational_parameter")
   {
      aInput.ReadValue(mGravitationalParameter);
      aInput.ValueGreater(mGravitationalParameter, 0.0);
   }
   else
   {
      retval = false;
   }
   return retval;
}

UtVec3d WsfEarthMonopoleTerm::ComputeAcceleration(double /*aMass*/,
                                                  const UtCalendar& /*aTime*/,
                                                  const UtVec3d& aPosition,
                                                  const UtVec3d& /*aVelocity*/) const
{
   UtVec3d retval   = aPosition; // Starts as displacement (Earth is at the origin)
   double  radius   = aPosition.Magnitude();
   double  radcubed = radius * radius * radius;
   retval *= -mGravitationalParameter / radcubed; // Now becomes acceleration
   return retval;
}
