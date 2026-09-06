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

#include "WsfMeasurement.hpp"

#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMeasurementUtil.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfDefaultTrackExtrapolationStrategy.hpp"
#include "WsfKinematicStateExtrapolation.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackExtrapolationStrategyTypes.hpp"

// static
double WsfMeasurement::cDEFAULT_ALTITUDE_AIR        = 7000.0; // meters
double WsfMeasurement::cDEFAULT_ALTITUDE_LAND       = 100.0;
double WsfMeasurement::cDEFAULT_ALTITUDE_SPACE      = 400000.0;
double WsfMeasurement::cDEFAULT_ALTITUDE_SURFACE    = 0.0;
double WsfMeasurement::cDEFAULT_ALTITUDE_SUBSURFACE = -100.0;

// =================================================================================================
WsfMeasurement::WsfMeasurement()
   : UtMeasurementData()
{
   mSpatialDomain.Set(WSF_SPATIAL_DOMAIN_UNKNOWN);
}

// =================================================================================================
WsfMeasurement* WsfMeasurement::Clone() const
{
   return new WsfMeasurement(*this);
}

bool WsfMeasurement::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "spatial_domain")
   {
      std::string spatialDomain;
      aInput.ReadValue(spatialDomain);
      WsfSpatialDomain enumSpatialDomain;
      if (!WsfTypes::StringToEnum(spatialDomain, enumSpatialDomain))
      {
         throw UtInput::BadValue(aInput);
      }
      // todo instead call SetSpatialDomain, which in turn sets a default extrapolator per spatial domain.
      mSpatialDomain.Set(enumSpatialDomain);
   }
   else if (command == "default_altitude")
   {
      myCommand = true;
      aInput.ReadCommand(command);
      double alt;
      aInput.ReadValueOfType(alt, UtInput::cLENGTH);

      if (command == "air")
      {
         WsfMeasurement::cDEFAULT_ALTITUDE_AIR = alt;
      }
      else if (command == "land")
      {
         WsfMeasurement::cDEFAULT_ALTITUDE_LAND = alt;
      }
      else if (command == "space")
      {
         WsfMeasurement::cDEFAULT_ALTITUDE_SPACE = alt;
      }
      else if (command == "surface")
      {
         WsfMeasurement::cDEFAULT_ALTITUDE_SURFACE = alt;
      }
      else if (command == "subsurface")
      {
         WsfMeasurement::cDEFAULT_ALTITUDE_SUBSURFACE = alt;
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// virtual
//! Update the measurement/track to the given time using filtering and extrapolation.
void WsfMeasurement::KinematicUpdate(double aSimTime)
{
   if (aSimTime > GetUpdateTime())
   {
      double locationWCS[3];

      // We perform a simple calculation.
      // We require the velocity to be valid here; otherwise history data are saved off,
      // and one essentially creates a default filter.
      if (VelocityValid() && GetExtrapolatedLocationWCS(aSimTime, locationWCS))
      {
         SetLocationWCS(locationWCS);

         if (StateCovarianceValid())
         {
            // GetStateCovarianceMatrix(aSimTime, *(mStateCovariancePtr.Get()));
            UtCovariance predictedCovar;
            predictedCovar.ComputePredictedState(aSimTime, *GetStateCovariance(), *this);
            SetStateCovariance(aSimTime, predictedCovar);
         }
      }
   }
}

// =================================================================================================
// static
//! Given a spatial domain, return the default altitude of that domain.
double WsfMeasurement::GetDefaultAltitude(WsfSpatialDomain aDomain)
{
   double altitude = cDEFAULT_ALTITUDE_SURFACE;
   if (aDomain == WSF_SPATIAL_DOMAIN_AIR)
   {
      altitude = cDEFAULT_ALTITUDE_AIR;
   }
   else if (aDomain == WSF_SPATIAL_DOMAIN_LAND)
   {
      altitude = cDEFAULT_ALTITUDE_LAND;
   }
   else if (aDomain == WSF_SPATIAL_DOMAIN_SPACE)
   {
      altitude = cDEFAULT_ALTITUDE_SPACE;
   }
   else if (aDomain == WSF_SPATIAL_DOMAIN_SUBSURFACE)
   {
      altitude = cDEFAULT_ALTITUDE_SUBSURFACE;
   }
   return altitude;
}

// =================================================================================================
//! If WsfMeasurement::mSimulationPtr is non-null, this method performs a just-in-time initialization of
//! WsfMeasurement::mExtrapolators. This is necessary because the desired extrapolation objects are unknown until the
//! measurement data are populated (i.e., post-construction/initialization).
//! @note This method must be called to access WsfMeasurement::mExtrapolators from within this class implementation for
//! correct extrapolation.
const wsf::TrackExtrapolators* WsfMeasurement::GetTrackExtrapolators() const
{
   if (mSimulationPtr != nullptr)
   {
      auto extrapolatorsPtr = ut::make_unique<wsf::TrackExtrapolators>();
      // Use the default extrapolation strategy to populate the track's extrapolation objects.
      // This could be generalized in the future to enable use of per-tracker extrapolation strategies (not just the default).
      wsf::TrackExtrapolationStrategyTypes::Get(mSimulationPtr->GetScenario())
         .GetDefaultStrategy()
         .InitializeExtrapolation(*this, *extrapolatorsPtr);
      mExtrapolatorsPtr = std::move(extrapolatorsPtr);
   }
   return mExtrapolatorsPtr;
}

// =================================================================================================
//! Get the predicted perceived target WCS location at a time other than the last updated time.
//! Returns true if successful or false if not enough information is present (e.g.: this is a 'range-only' track).
//! @param aSimTime The time at which the location estimate is desired.
//! @param aLocationWCS (output) The estimated location at the given time.
//! @return Whether an extrapolated location could be computed based on data in the measurement.
// virtual
bool WsfMeasurement::GetExtrapolatedLocationWCS(double aSimTime, double aLocationWCS[3]) const
{
   auto ksInfo = wsf::KinematicStateExtrapolation::Type();
   if (GetExtrapolatedData(aSimTime, wsf::KinematicStateExtrapolation::GetTypeId(), ksInfo))
   {
      UtVec3d::Set(aLocationWCS, ksInfo.GetLocationWCS().GetData());
      return true;
   }
   return false;
}

// =================================================================================================
//! @brief Convenience method to set a 3D spherical measurement result.
//! @param aSimTime The time at which the measurement is valid.
//! @param aAzimuth The measurement azimuth.
//! @param aElevation The measurement elevation.
//! @param aRange The range of the measurement.
//! @param aRangeRate The measurement range rate.
void WsfMeasurement::SetSphericalMeasurement(double aSimTime, double aAzimuth, double aElevation, double aRange, double aRangeRate)
{
   SetUpdateTime(aSimTime);
   SetRange(aRange);
   SetRangeValid(true);
   SetSensorAzimuth(aAzimuth);
   SetSensorAzimuthValid(true);
   SetSensorElevation(aElevation);
   SetSensorElevationValid(true);
   if (RangeRateValid())
   {
      SetRangeRate(aRangeRate);
   }
}
