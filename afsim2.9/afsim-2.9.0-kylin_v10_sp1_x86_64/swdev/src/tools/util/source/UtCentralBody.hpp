// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTCENTRALBODY_HPP
#define UTCENTRALBODY_HPP

#include "ut_export.h"

#include <limits>
#include <memory>
#include <string>

class UtCalendar;
#include "UtCentralBodyEllipsoid.hpp"
#include "UtCentralPoint.hpp"
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"

namespace ut
{

class OrbitalStateVector;

//! Define a central body to be used for the origin of world coordinates, coordinate conversions,
//! and satellite propagation.
class UT_EXPORT CentralBody : public CentralPoint
{
public:
   explicit CentralBody(const CentralBodyEllipsoid& aEllipsoid)
      : mCentralBodyEllipsoidPtr(ut::clone(aEllipsoid))
   {
   }

   CentralBody* Clone() const override = 0; // return covariant type
   bool         ProcessInput(UtInput& aInput) override;

   static std::unique_ptr<CentralBody> Create(const std::string& aName);
   bool                                IsCentralBody() const override { return true; }

   static bool
   LineOfSight(const UtVec3d& aLocation1, const UtVec3d& aLocation2, const UtVec3d& aBodyLocation, double aBodyRadius);
   bool LineOfSight(const UtVec3d& aLocation1, const UtVec3d& aLocation2) const;

   const char*                     GetName() const override { return mCentralBodyEllipsoidPtr->GetName(); }
   const ut::CentralBodyEllipsoid& GetEllipsoid() const { return *mCentralBodyEllipsoidPtr; }
   void SetEllipsoid(const ut::CentralBodyEllipsoid& aEllipsoid) { mCentralBodyEllipsoidPtr = aEllipsoid.Clone(); }

   double GetGravitationalParameter() const override { return mCentralBodyEllipsoidPtr->GetGravitationalParameter(); }
   double GetRotationRate() const override { return mCentralBodyEllipsoidPtr->GetRotationRate(); }
   double GetMeanRadius() const override { return mCentralBodyEllipsoidPtr->GetMeanRadius(); }

   bool    UsingPolarMotion() const override { return (mPolarOffsetAngleX != 0.0) || (mPolarOffsetAngleY != 0.0); }
   UtMat3d ComputePolarMotion() const override;

   //! Return the right ascension of the central body's north pole.
   //! @param aEpoch The epoch at which the north pole RA is valid.
   //! @note An average value may be returned, regardless of epoch.
   virtual double GetNorthPoleRA(const UtCalendar& aEpoch) const = 0;

   //! Return the declination of the central body's north pole.
   //! @param aEpoch The epoch at which the north pole declination is valid.
   //! @note An average value may be returned, regardless of epoch.
   virtual double GetNorthPoleDec(const UtCalendar& aEpoch) const = 0;

   //! Return the "x" polar offset angle.  This is typically only used for the Earth.
   virtual double GetPolarOffsetAngleX() const { return mPolarOffsetAngleX; }

   //! Return the "y" polar offset angle.  This is typically only used for the Earth.
   virtual double GetPolarOffsetAngleY() const { return mPolarOffsetAngleY; }

   //! Return the obliquity of the central body (angle of pole with respect to the normal of its orbital plane).
   virtual double GetObliquity(const UtCalendar& /*aEpoch*/) const { return 0.0; }

   //! Return the ecliptic velocity of the central body.
   //! @note Calculating this value is optional in derived classes.  If it
   //! is not calculated, the null vector is returned.
   virtual UtVec3d GetVelocityEcliptic() const { return UtVec3d(); }

   virtual void SetPolarOffsetAngles(double aPolarOffsetAngleX, double aPolarOffsetAngleY);

   //! @name Methods to perform basic conversions.
   //! These Methods could be overridden to facilitate conversions with tides, undulation, etc.
   //@{
   virtual void ConvertLLA_ToWCS(double aLat, double aLon, double aAlt, UtVec3d& aWCS) const;
   virtual void ConvertWCS_ToLLA(const UtVec3d& aWCS, double& aLat, double& aLon, double& aAlt) const;
   virtual void ConvertLocalToLLA(const UtVec3d& aRefWCS,
                                  const double   aTransWCS[3][3],
                                  const UtVec3d& aLocal,
                                  double&        aLat,
                                  double&        aLon,
                                  double&        aAlt) const;
   virtual void ConvertLLA_ToLocal(const UtVec3d& aRefWCS,
                                   const double   aTransWCS[3][3],
                                   double         aLat,
                                   double         aLon,
                                   double         aAlt,
                                   UtVec3d&       aLocal) const;
   //@}

   //! @name low-level and helper methods.
   //@{
   double          GetSynchronousOrbitSemiMajorAxis() const;
   virtual UtVec3d GetAccelerationInertial(const UtVec3d& aLocation) const;
   virtual double  GetEscapeVelocity(const UtVec3d& aLocation) const;
   virtual void ComputeNutationAngles(const UtCalendar& aTime, double& aLongitudeNutation, double& aObliquityNutation) const;
   using CentralPoint::ComputeSiderealAngle;
   UT_DEPRECATED virtual double ComputeSiderealAngle(const UtCalendar& aTime, double aLongitudeNutation) const;

   UtMat3d ComputeECI_ToCentralPointInertialTransform(const UtCalendar& aEpoch) const override;

   //! Convert an orbital state vector from ecliptic to equatorial coordinates.
   static OrbitalStateVector ConvertEclipticToEquatorial(const UtCalendar&         aEpoch,
                                                         const OrbitalStateVector& aOrbitalStateVectorEcliptic);

   //! Convert an orbital state vector from equatorial to ecliptic coordinates.
   static OrbitalStateVector ConvertEquatorialToEcliptic(const UtCalendar&         aEpoch,
                                                         const OrbitalStateVector& aOrbitalStateVectorEquatorial);
   //@}

private:
   ut::CloneablePtr<CentralBodyEllipsoid> mCentralBodyEllipsoidPtr;
   double                                 mPolarOffsetAngleX{0.0};
   double                                 mPolarOffsetAngleY{0.0};
};

} // namespace ut

#endif
