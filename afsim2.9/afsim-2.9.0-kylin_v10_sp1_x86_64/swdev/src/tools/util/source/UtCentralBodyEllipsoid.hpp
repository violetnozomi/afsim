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

#ifndef UTCENTRALBODYELLIPSOID_HPP
#define UTCENTRALBODYELLIPSOID_HPP

#include "ut_export.h"

#include <limits>

class UtCalendar;
#include "UtVec3.hpp"

namespace ut
{

//! A base class containing basic data applicable to any sufficiently large gravitating central body.
class UT_EXPORT CentralBodyEllipsoid
{
public:
   virtual ~CentralBodyEllipsoid() = default;

   //! Return the name of the ellipsoid.
   virtual const char* GetName() const = 0;

   //! Return the gravitational parameter (G * mass of ellipsoid),
   //! including the mass of the atmosphere.
   virtual double GetGravitationalParameter() const = 0;

   //! Return the rotation rate in a precessing reference frame.
   virtual double GetRotationRate() const = 0;

   //! Return the semi-major axis of the ellipsoid.
   virtual double GetSemiMajorAxis() const = 0;

   //! Return the semi-minor axis of the ellipsoid.
   virtual double GetSemiMinorAxis() const { return GetSemiMajorAxis(); }

   //! Return the mean radius,
   //! defined to be the radius of a sphere of equal volume as the ellipsoid.
   virtual double GetMeanRadius() const { return GetSemiMajorAxis(); }

   //! Return the flattening reciprocal,
   //! defined to be a/(a-b), where "a" is the semi-major axis, and "b"
   //! is the semi-minor axis.
   virtual double GetFlatteningReciprocal() const { return std::numeric_limits<double>::max(); }

   //! Return the first eccentricity squared,
   //! defined to be 1 - b^2/a^2, where "a" is the semi-major axis, and "b"
   //! is the semi-minor axis.
   virtual double GetFirstEccentricitySquared() const { return 0.0; }


   //! Return the second eccentricity squared,
   //! defined to be a^2/b^2 - 1, where "a" is the semi-major axis, and "b"
   //! is the semi-minor axis.
   virtual double GetSecondEccentricitySquared() const { return 0.0; }

   virtual CentralBodyEllipsoid* Clone() const = 0;

   //! @name Pass-through methods to perform basic conversions based solely on the ellipsoid.
   //@{
   void ConvertLLA_ToWCS(double aLat, double aLon, double aAlt, UtVec3d& aWCS) const;
   void ConvertWCS_ToLLA(const UtVec3d& aWCS, double& aLat, double& aLon, double& aAlt) const;
   void ConvertLocalToLLA(const UtVec3d& aRefWCS,
                          const double   aTransWCS[3][3],
                          const UtVec3d& aLocal,
                          double&        aLat,
                          double&        aLon,
                          double&        aAlt) const;
   void ConvertLLA_ToLocal(const UtVec3d& aRefWCS,
                           const double   aTransWCS[3][3],
                           double         aLat,
                           double         aLon,
                           double         aAlt,
                           UtVec3d&       aLocal) const;
   //@}
};

} // namespace ut

#endif
