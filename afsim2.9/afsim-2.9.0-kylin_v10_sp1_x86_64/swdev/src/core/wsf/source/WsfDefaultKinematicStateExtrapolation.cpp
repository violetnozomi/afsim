//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#include "WsfDefaultKinematicStateExtrapolation.hpp"

#include "UtEntity.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfFilter.hpp"
#include "WsfMeasurement.hpp"

namespace
{

void ExtrapolateLocationWCS(double                aSimTime,
                            const WsfMeasurement& aData,
                            const double          aStartLocationWCS[3],
                            double                aEndLocationWCS[3])
{
   double deltaT = aSimTime - aData.GetUpdateTime();
   if ((deltaT != 0.0) && // Allow backward extrapolation.
       (aData.GetVelocityWCS().MagnitudeSquared() > 0.0))
   {
      // Start with a simple extrapolation.
      aEndLocationWCS[0] = aStartLocationWCS[0] + aData.GetVelocityWCS()[0] * deltaT;
      aEndLocationWCS[1] = aStartLocationWCS[1] + aData.GetVelocityWCS()[1] * deltaT;
      aEndLocationWCS[2] = aStartLocationWCS[2] + aData.GetVelocityWCS()[2] * deltaT;

      double range, bearing, elevation;
      UtMeasurementUtil::RangeBearingElevation(aStartLocationWCS, aEndLocationWCS, range, bearing, elevation);
      double isoAltRange = range * cos(elevation);
      double lat, lon, alt;
      aData.GetLocationLLA(lat, lon, alt);

      // The following is an approximation that assumes the altitude is << the earth radius.
      double altFalloff = isoAltRange * isoAltRange / (2.0 * (UtSphericalEarth::cEARTH_RADIUS + alt));
      UtEntity::ConvertWCSToLLA(aEndLocationWCS, lat, lon, alt);
      alt -= altFalloff;
      UtEntity::ConvertLLAToWCS(lat, lon, alt, aEndLocationWCS);
   }
   else
   {
      UtVec3d::Set(aEndLocationWCS, aStartLocationWCS);
   }
}

bool GetExtrapolatedLocationWCS(double aSimTime, const WsfMeasurement& aData, double aLocationWCS[3])
{
   bool result = true;

   // If a stable filter is present use the filter to return the estimate.
   WsfFilter* filterPtr = const_cast<WsfFilter*>(aData.GetFilter());
   if ((filterPtr != nullptr) && filterPtr->IsStable())
   {
      WsfMeasurement dataOut;
      filterPtr->GetKinematicEstimate(aSimTime, dataOut);
      if (dataOut.mLocationWCS.IsValid())
      {
         UtVec3d::Set(aLocationWCS, dataOut.mLocationWCS.Get().GetData());
      }
   }
   else // Attempt to use measurement data to generate a valid location.
   {
      if (aData.LocationValid())
      {
         auto locWCS = aData.GetLocationWCS();
         if (aData.VelocityValid())
         {
            ExtrapolateLocationWCS(aSimTime, aData, locWCS.GetData(), aLocationWCS);
         }
         else
         {
            UtVec3d::Set(aLocationWCS, locWCS.GetData());
         }
      }
      else if (aData.RangeValid() && aData.BearingValid())
      {
         double range = aData.GetRange();
         if (aData.RangeRateValid())
         {
            double deltaT = aSimTime - aData.GetUpdateTime();
            range += aData.GetRangeRate() * deltaT;
         }

         double elevation = 0.0;
         if (aData.ElevationValid())
         {
            elevation = aData.GetElevation();
         }
         else
         {
            // Compute an elevation angle using an assumed altitude.
            double altitude = aData.GetDefaultAltitude();

            // If the range is less than the assumed altitude then adjust the assumed altitude to prevent math issues.
            // (The resulting elevation angle gets really messed up). We also don't let the elevation angle get close to
            // 90 degrees.
            //
            // NOTE: This whole thing is kinda funky as we don't have 2D/RB filters. We currently convert to a pseudo-3D
            //       in order to give the 3D filter something to work with.
            if (fabs(altitude) > 0.9 * range)
            {
               altitude = 0.9 * range * UtMath::Sign(altitude);
            }
            elevation = UtMeasurementUtil::Elevation(aData.GetOriginatorLocationWCS().GetData(), range, altitude);

            // Set the elevation error in case this is being filtered.
            // MJM TODO evaluate; this is not in the baseline const_cast<WsfMeasurement*>(this)->mElevation.SetError(0.2 * elevation);
         }

         UtMeasurementUtil::LocationWCS(aData.GetOriginatorLocationWCS().GetData(),
                                        range,
                                        aData.GetBearing(),
                                        elevation,
                                        aLocationWCS);
         if (aData.VelocityValid())
         {
            ExtrapolateLocationWCS(aSimTime, aData, aLocationWCS, aLocationWCS);
         }
      }
      else
      {
         // Do not have enough information to fulfill this request:
         result = false;
      }
   }

   return result;
}

} // namespace

std::unique_ptr<UtAttribute<wsf::KinematicState>>
wsf::DefaultKinematicStateExtrapolation::Extrapolate(double aSimTime, const WsfMeasurement& aData) const
{
   UtVec3d locWCS;
   if (GetExtrapolatedLocationWCS(aSimTime, aData, locWCS.GetData()))
   {
      auto ksiPtr = ut::make_unique<wsf::KinematicStateInformation>();
      ksiPtr->Get().SetLocationWCS(locWCS);
      if (aData.VelocityValid())
      {
         ksiPtr->Get().SetVelocityWCS(aData.GetVelocityWCS());
      }
      return std::move(ksiPtr);
   }
   return nullptr;
}
