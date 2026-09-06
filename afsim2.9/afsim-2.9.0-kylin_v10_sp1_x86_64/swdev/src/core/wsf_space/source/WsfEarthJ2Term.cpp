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

#include "WsfEarthJ2Term.hpp"

#include <string>

#include "UtCalendar.hpp"
#include "UtECI_Conversion.hpp"
#include "UtInput.hpp"
#include "WsfOrbitalDynamics.hpp"

WsfEarthJ2Term::WsfEarthJ2Term()
   : WsfOrbitalDynamicsTerm{}
{
   RecomputePrefactor();
}

bool WsfEarthJ2Term::ProcessInput(UtInput& aInput)
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
   else if (command == "j2")
   {
      aInput.ReadValue(mJ2);
   }
   else if (command == "mean_radius")
   {
      aInput.ReadValueOfType(mEarthMeanRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mEarthMeanRadius, 0.0);
   }
   else
   {
      retval = false;
   }
   RecomputePrefactor();
   return retval;
}

UtVec3d WsfEarthJ2Term::ComputeAcceleration(double /*aMass*/,
                                            const UtCalendar& aTime,
                                            const UtVec3d&    aPosition,
                                            const UtVec3d& /*aVelocity*/) const
{
   // NOTE: The moments in the gravitational field of the Earth should be computed
   // in an inertial frame (no angular velocity), but that frame needs to be
   // aligned with the surface of the Earth so that the moments as provided from geodesy can
   // be used as provided. To that end, this method uses the WCS frame to provide the
   // correct orientation of the axes, but it is not considered to be rotating with
   // time. When converting the resulting acceleration back to the ECI frame,
   // UtECI_Conversion::ConvertAccelerationWCS_ToECI is *not* used because that will
   // add in terms for the acceleration of the WCS frame.

   // 1) Convert input ECI position to Earth-fixed frame
   UtVec3d retval{0.0, 0.0, 0.0};
   auto    dynamicsPtr = GetDynamics();
   if (dynamicsPtr)
   {
      dynamicsPtr->UpdateConverterTime(aTime);
      UtVec3d positionWCS = dynamicsPtr->GetWCS_PositionFromECI(aPosition);

      // 2) Compute acceleration in Earth-fixed frame
      double  r2          = positionWCS.MagnitudeSquared();
      double  r4          = r2 * r2;
      double  r           = sqrt(r2);
      double  coeff       = mPrefactor / (r4 * r);
      double  parenfactor = 5.0 * (positionWCS[2] * positionWCS[2] / r2);
      UtVec3d accWCS{positionWCS[0] * coeff * (parenfactor - 1.0),
                     positionWCS[1] * coeff * (parenfactor - 1.0),
                     positionWCS[2] * coeff * (parenfactor - 3.0)};

      // 3) Convert acceleration to ECI frame and return
      retval = dynamicsPtr->GetECI_AccelerationFromWCS(accWCS);
   }
   return retval;
}

void WsfEarthJ2Term::RecomputePrefactor()
{
   mPrefactor = (3.0 / 2.0) * mGravitationalParameter * mEarthMeanRadius * mEarthMeanRadius * mJ2;
}
