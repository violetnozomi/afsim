// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTCENTRALPOINT_HPP
#define UTCENTRALPOINT_HPP

#include "ut_export.h"

#include <string>

class UtCalendar;
namespace ut
{
class CentralBody;
}
#include "UtCloneablePtr.hpp"
class UtInput;
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"

namespace ut
{

//! @brief An inertial central point and orientation to be used as the origin of
//! rotating-frame coordinate conversions and satellite propagation.
//! @details This class provides two major services.  The first is to provide the time-dependent rotation matrices
//! that convert from an inertial frame of reference to a rotating frame.  The second service is to
//! provide an inertial translation vector and inertial rotation matrix to convert from Earth-Centered Inertial
//! (ECI) to the inertial frame of this central point (if the central point is the Earth, the translation vector
//! will be the null vector and the rotation matrix will be the identity matrix).  For example, in the case of the
//! Moon, the translation vector will be the ECI location of the Moon, and the rotation matrix is defined by the
//! right ascension and declination of the Moon's north pole, and an agreed-upon sidereal offset angle.
//! Of the rotation matrices that may be provided by a CentralPoint, sidereal motion (rotation about the z-axis as
//! defined by a rotation rate) is most common and provides the largest effect.  The other matrices: polar motion,
//! precession, and nutation, are currently only utilized by the Earth (see EarthCentralBody).  The description and
//! mathematical representation of these matrices are described in detail elsewhere (see esp. Vallado, Fundamentals of
//! Astrodynamics and Applications, 4th ed., sect. 3.7 "Transforming Celestial and Terrestrial Coordinates", pp.
//! 203-238).
//! @note UtECI_Conversion utilizes the rotation matrices provided by CentralPoint to convert among inertial and
//! rotating frames of reference.
//! @note OrbitalState utilizes the inertial transform and rotation matrices to transform orbital states from one
//! CentralPoint to another.
class UT_EXPORT CentralPoint
{
public:
   virtual ~CentralPoint() = default;

   //! @name Common infrastructure methods.
   //@{
   //! Create a copy of this object.
   virtual CentralPoint* Clone() const = 0;
   virtual bool          ProcessInput(UtInput& aInput);

   //! Initialize this object at the given epoch.
   //! @param aEpoch The epoch at which the object should be initialized.
   //! @return Whether the initialization was successful.
   virtual bool Initialize(const UtCalendar& /*aEpoch*/) { return true; }
   //@}

   virtual const char* GetName() const = 0;

   //! Return the rotation rate of the rotating frame with respect to the inertial frame.
   virtual double GetRotationRate() const { return mRotationRate; }
   virtual void   SetRotationRate(double aRotationRate) { mRotationRate = aRotationRate; }

   static std::unique_ptr<CentralPoint> Create(const std::string& aName);
   const CentralBody&                   GetAsCentralBody() const;
   CentralBody&                         GetAsCentralBody();

   //! Return the Earth-Centered Inertial (ECI) location of this CentralPoint.
   virtual UtVec3d GetLocationECI(const UtCalendar& aEpoch) const = 0;
   virtual UtVec3d GetVelocityECI(const UtCalendar& aEpoch) const;

   //! Return whether the CentralPoint is earth-referenced.
   //! This method is used to determine whether an inertial frame is ECI,
   //! and whether to perform the earth-specific "frame-tie" conversion.
   virtual bool IsEarth() const { return false; }

   //! Return whether the CentralPoint is a CentralBody.
   //! @note Central bodies can serve as a gravitating mass for a Keplerian propagator
   //! and will produce valid LLA results above an ellipsoid; central points do not.
   virtual bool IsCentralBody() const { return false; }

   //! Compute the Earth-Centered Inertial (ECI) to CentralPoint inertial transform for this CentralPoint.
   //! @param aEpoch The epoch at which the transform is valid.
   //! @return The Earth-Centered Inertial (ECI) to CentralPoint inertial transform.
   virtual UtMat3d ComputeECI_ToCentralPointInertialTransform(const UtCalendar& aEpoch) const = 0;
   virtual UtMat3d ComputeCentralPointInertialToECI_Transform(const UtCalendar& aEpoch) const;

   //! Return whether this CentralPoint utilizes a purely rotating frame about the z axis.
   bool            UsingSiderealMotion() const { return mRotationRate != 0.0; }
   virtual UtMat3d ComputeSiderealMotion(const UtCalendar& aEpoch) const;

   //! Return whether this central point's z-axis precesses.
   virtual bool UsingPrecession() const { return false; }

   //! Compute the rotation matrix accounting for precession.  By default this is the identity matrix.
   virtual UtMat3d ComputePrecession(const UtCalendar& /*aTime*/) const { return UtMat3d(); }

   //! Return whether this central point's z-axis undergoes nutation.
   virtual bool UsingNutation() const { return false; }

   //! Compute the nutation matrix (specifically, the precession to nutation transformation).
   //! @param aEpoch The epoch at which the nutation transform is valid.
   virtual UtMat3d ComputeNutation(const UtCalendar& /*aEpoch*/) const { return UtMat3d(); }

   //! Return whether this central point computes an offset polar motion coordinate transform.
   virtual bool UsingPolarMotion() const { return false; }

   //! Compute an offset due to polar motion.
   virtual UtMat3d ComputePolarMotion() const { return UtMat3d(); }

   //! @name Compatibility methods with CentralBody
   //@{
   virtual double GetGravitationalParameter() const;
   virtual double GetMeanRadius() const;
   //@}

protected:
   //! Return the sidereal "offset angle" to the central body's prime meridian, such that a sidereal angle calculation
   //! provides this angle at J2000.
   //! @note An average value may be returned, regardless of epoch.
   virtual double GetSiderealOffsetAngle(const UtCalendar& /*aEpoch*/) const { return 0.0; }

   virtual double ComputeSiderealAngle(const UtCalendar& aTime) const;

private:
   //! The right-handed rotation rate about the z axis.
   double mRotationRate{0.0};
};

} // namespace ut

#endif
