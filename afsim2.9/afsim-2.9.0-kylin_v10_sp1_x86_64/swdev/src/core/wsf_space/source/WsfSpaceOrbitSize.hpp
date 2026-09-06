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

#ifndef WSFSPACEORBITSIZE_HPP
#define WSFSPACEORBITSIZE_HPP

#include "wsf_space_export.h"

#include "UtEarth.hpp"
#include "UtUnitTypes.hpp"

namespace wsf
{
namespace space
{

//! This object provides the size of an orbit.
//!
//! There are a number of equivalent ways of specifying the size of a circular orbit
//! that are related via basic definitions or through Kepler's Third Law.
//! This object supports the semi-major axis, the altitude, the orbital period and
//! the number of revolutions per day.
class WSF_SPACE_EXPORT OrbitSize
{
public:
   OrbitSize(UtLengthValue aSemiMajorAxis          = UtLengthValue{1.0e-12, UtUnitLength::cKILOMETERS},
             double        aGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER);

   //! Return the semi-major axis of the orbit.
   const UtLengthValue& GetSemiMajorAxis() const { return mSemiMajorAxis; }

   //! Return the circular altitude of the orbit.
   const UtLengthValue& GetCircularAltitude() const { return mCircularAlititude; }

   //! Return the orbital period.
   const UtTimeValue& GetOrbitalPeriod() const { return mOrbitalPeriod; }

   //! Return the number of revolutions per day of the orbit.
   double GetRevolutionsPerDay() const { return mRevolutionsPerDay; }

   void SetSemiMajorAxis(const UtLengthValue& aSemiMajorAxis);
   void SetCircularAltitude(const UtLengthValue& aAltitude);
   void SetOrbitalPeriod(const UtTimeValue& aPeriod);
   void SetRevolutionsPerDay(double aRevolutionsPerDay);

   //! Return if the orbit size was specified using the semi-major axis.
   bool SpecifiedAsSemiMajorAxis() const { return mInputType == InputType::cSEMI_MAJOR_AXIS; }

   //! Return if the orbit size was specified using the circular altitude.
   bool SpecifiedAsCircularAltitude() const { return mInputType == InputType::cCIRCULAR_ALTITUDE; }

   //! Return if the orbit size was specified using the orbital period.
   bool SpecifiedAsOrbitalPeriod() const { return mInputType == InputType::cORBITAL_PERIOD; }

   //! Return if the orbit size was specified using the number of revolutions per day.
   bool SpecifiedAsRevsPerDay() const { return mInputType == InputType::cREVS_PER_DAY; }

   static OrbitSize FromSemiMajorAxis(UtLengthValue aSemiMajorAxis,
                                      double        aGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER);
   static OrbitSize FromCircularAltitude(UtLengthValue aAltitude,
                                         double        aGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER);
   static OrbitSize FromOrbitalPeriod(UtTimeValue aPeriod,
                                      double      aGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER);
   static OrbitSize FromRevolutionsPerDay(double aRevolutionsPerDay,
                                          double aGravitationalParameter = UtEarth::cGRAVITATIONAL_PARAMETER);

private:
   void AltitudeFromSMA();
   void SMA_FromAltitude();
   void PeriodFromSMA();
   void SMA_FromPeriod();
   void RevsFromPeriod();
   void PeriodFromRevs();

   enum InputType
   {
      cSEMI_MAJOR_AXIS,
      cCIRCULAR_ALTITUDE,
      cORBITAL_PERIOD,
      cREVS_PER_DAY
   };

   UtLengthValue mSemiMajorAxis{0.0, UtUnitLength::cKILOMETERS};
   UtLengthValue mCircularAlititude{0.0, UtUnitLength::cKILOMETERS};
   UtTimeValue   mOrbitalPeriod{0.0, UtUnitTime::cMINUTES};
   double        mRevolutionsPerDay{};
   double        mGravitationalParameter; // in SI units
   InputType     mInputType{InputType::cSEMI_MAJOR_AXIS};
};

} // namespace space
} // namespace wsf

#endif // WSFSPACEORBITSIZE_HPP
