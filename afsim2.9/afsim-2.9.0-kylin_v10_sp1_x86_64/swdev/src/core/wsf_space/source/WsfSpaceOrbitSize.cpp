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

#include "WsfSpaceOrbitSize.hpp"

#include <cmath>

#include "UtCalendar.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"

namespace wsf
{
namespace space
{

OrbitSize::OrbitSize(UtLengthValue aSemiMajorAxis, double aGravitationalParameter)
   : mGravitationalParameter{aGravitationalParameter}
{
   SetSemiMajorAxis(aSemiMajorAxis);
}

//! Set the orbit size using the semi-major axis.
void OrbitSize::SetSemiMajorAxis(const UtLengthValue& aSemiMajorAxis)
{
   mSemiMajorAxis = aSemiMajorAxis;
   mInputType     = InputType::cSEMI_MAJOR_AXIS;
   AltitudeFromSMA();
   PeriodFromSMA();
   RevsFromPeriod();
}

//! Set the orbit size using the circular altitude.
void OrbitSize::SetCircularAltitude(const UtLengthValue& aAltitude)
{
   mCircularAlititude = aAltitude;
   mInputType         = InputType::cCIRCULAR_ALTITUDE;
   SMA_FromAltitude();
   PeriodFromSMA();
   RevsFromPeriod();
}

//! Set the orbit size using the orbital period.
void OrbitSize::SetOrbitalPeriod(const UtTimeValue& aPeriod)
{
   mOrbitalPeriod = aPeriod;
   mInputType     = InputType::cORBITAL_PERIOD;
   RevsFromPeriod();
   SMA_FromPeriod();
   AltitudeFromSMA();
}

//! Set the orbit size using the number of revolutions per day.
void OrbitSize::SetRevolutionsPerDay(double aRevolutionsPerDay)
{
   mRevolutionsPerDay = aRevolutionsPerDay;
   mInputType         = InputType::cREVS_PER_DAY;
   PeriodFromRevs();
   SMA_FromPeriod();
   AltitudeFromSMA();
}

//! Return an OrbitSize created from a semi-major axis.
OrbitSize OrbitSize::FromSemiMajorAxis(UtLengthValue aSemiMajorAxis, double aGravitationalParameter)
{
   return OrbitSize{aSemiMajorAxis, aGravitationalParameter};
}

//! Return an OrbitSize created from a circular altitude.
OrbitSize OrbitSize::FromCircularAltitude(UtLengthValue aAltitude, double aGravitationalParameter)
{
   OrbitSize retval{UtLengthValue{0.0, UtUnitLength::cKILOMETERS}, aGravitationalParameter};
   retval.SetCircularAltitude(aAltitude);
   return retval;
}

//! Return an OrbitSize created from an orbital period.
OrbitSize OrbitSize::FromOrbitalPeriod(UtTimeValue aPeriod, double aGravitationalParameter)
{
   OrbitSize retval{UtLengthValue{0.0, UtUnitLength::cKILOMETERS}, aGravitationalParameter};
   retval.SetOrbitalPeriod(aPeriod);
   return retval;
}

//! Return an OrbitSize created from a number of revolutions per day.
OrbitSize OrbitSize::FromRevolutionsPerDay(double aRevolutionsPerDay, double aGravitationalParameter)
{
   OrbitSize retval{UtLengthValue{0.0, UtUnitLength::cKILOMETERS}, aGravitationalParameter};
   retval.SetRevolutionsPerDay(aRevolutionsPerDay);
   return retval;
}

void OrbitSize::AltitudeFromSMA()
{
   double        sma = mSemiMajorAxis.GetAsUnit(UtUnitLength::cMETERS);
   UtLengthValue alt{sma - UtSphericalEarth::cEARTH_MEAN_RADIUS, UtUnitLength::cMETERS};
   alt.ConvertToUnit(mCircularAlititude.GetUnit());
   mCircularAlititude = alt;
}

void OrbitSize::SMA_FromAltitude()
{
   double        alt = mCircularAlititude.GetAsUnit(UtUnitLength::cMETERS);
   UtLengthValue sma{alt + UtSphericalEarth::cEARTH_MEAN_RADIUS, UtUnitLength::cMETERS};
   sma.ConvertToUnit(mSemiMajorAxis.GetUnit());
   mSemiMajorAxis = sma;
}

void OrbitSize::PeriodFromSMA()
{
   double      sma = mSemiMajorAxis.GetAsUnit(UtUnitLength::cMETERS);
   UtTimeValue period{UtMath::cTWO_PI * sqrt(sma * sma * sma / mGravitationalParameter), UtUnitTime::cSECONDS};
   period.ConvertToUnit(mOrbitalPeriod.GetUnit());
   mOrbitalPeriod = period;
}

void OrbitSize::SMA_FromPeriod()
{
   double        period = mOrbitalPeriod.GetAsUnit(UtUnitTime::cSECONDS);
   double        aCubed = mGravitationalParameter * period * period / (UtMath::cTWO_PI * UtMath::cTWO_PI);
   UtLengthValue sma{pow(aCubed, 1.0 / 3.0), UtUnitLength::cMETERS};
   sma.ConvertToUnit(mSemiMajorAxis.GetUnit());
   mSemiMajorAxis = sma;
}

void OrbitSize::RevsFromPeriod()
{
   mRevolutionsPerDay = UtCalendar::cSEC_IN_DAY / mOrbitalPeriod.GetAsUnit(UtUnitTime::cSECONDS);
}

void OrbitSize::PeriodFromRevs()
{
   UtTimeValue period{UtCalendar::cSEC_IN_DAY / mRevolutionsPerDay, UtUnitTime::cSECONDS};
   period.ConvertToUnit(mOrbitalPeriod.GetUnit());
   mOrbitalPeriod = period;
}

} // namespace space
} // namespace wsf
