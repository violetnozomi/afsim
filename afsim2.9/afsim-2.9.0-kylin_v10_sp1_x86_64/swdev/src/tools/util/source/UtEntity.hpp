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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#ifndef UTENTITY_HPP
#define UTENTITY_HPP

#include "ut_export.h"

#include <memory>

class UtDCM;
#include "UtCalendar.hpp"
#include "UtCentralPoint.hpp"
#include "UtCloneablePtr.hpp"
#include "UtCoords.hpp"
#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtMemory.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3.hpp"

/** Represents the position, orientation, velocity and acceleration of an entity.
Five coordinate frames are used in this class.
<ul>
<li> The 'World Coordinate System' (WCS).  This is defined to be the WGS-84
coordinate system as defined in NIMA TR 8350.2 and utilized by DIS.
This is a right handed with:
  <ul>
  <li> The origin is at the center of the Earth.
  <li> The +X axis passes through 0N, 0E
  <li> The +Y axis passes through 0N, 90E
  <li> The +Z axis passes through 90N (the north pole).
  </ul>
<li> The 'Earth Centered Inertial' (ECI).  This system is like WCS except it has
been rotated by an angle, changing with time, in the WCS X-Y plane.
This is a right handed system with:
  <ul>
  <li> The origin is at the center of the Earth.
  <li> The +X axis points towards the vernal equinox (the point at which
  the Sun's path crosses the Earth's equator from south to north on 1st day of Spring [March
20th-21st]. This point defines the inertial reference with respect to the fixed background of
stars). <li> The +Y axis is perpendicular to the X axis in the equatorial plane. <li> The +Z axis
passes through 90N (the north pole).
  </ul>
<li> The 'Entity Coordinate System' (ECS).  This is sometimes referred to
as the 'Body' coordinate system.  This is a right handed system with:
  <ul>
  <li> The origin is at the center of the body.
  <li> The +X axis goes out the front of the body.
  <li> The +Y axis goes out the pilots right side.
  <li> The +Z axis goes out the bottom of the body.
  </ul>
  <ul>
  <li> Positive yaw   is to pilots right.
  <li> Positive pitch is nose up.
  <li> Positive roll  is right wing down.
  </ul>
<li> The local NED (North-East-Down) frame. This is like the ECS except it
has not been rotated by the entity orientation angles.
  <ul>
  <li> The origin is at the center of entity.
  <li> The +X axis is North.
  <li> The +Y axis is East.
  <li> The +Z axis is down.
  </ul>
<li> The 'Part Coordinate System' (PCS), which represents the local coordinate
system of a part attached to the entity (such as an antenna).  It is
described more fully in UtEntityPart.
</ul>
@see UtEntityPart
*/
class UT_EXPORT UtEntity
{
public:
   //! Define the coordinate frame used in the last SetLocationXXX call.
   enum CoordinateFrame
   {
      cCF_LLA,     //!< Last location was set with SetLocationLLA
      cCF_WCS,     //!< Last location was set with SetLocationWCS
      cCF_INERTIAL //!< Last location was set with SetLocationECI/TOD/TEME
   };

   explicit UtEntity(const ut::CentralPoint& aCentralPoint = ut::EarthWGS84());
   UtEntity(const UtEntity& aSrc) = default;
   UtEntity& operator=(const UtEntity& aRhs) = default;
   virtual ~UtEntity()                       = default;

   const ut::CentralBody& GetCentralBody() const;
   UT_DEPRECATED void     SetCentralBody(const ut::CentralBody& aCentralBody) { SetCentralPoint(aCentralBody); }

   //! Return the CentralPoint associated with this entity.
   const ut::CentralPoint& GetCentralPoint() const { return *mCentralPointPtr; }
   void                    SetCentralPoint(const ut::CentralPoint& aCentralPoint);

   //! @name Location Get/Set Methods
   //! Locations are in meters.
   //@{
   void            GetLocationWCS(double aLocationWCS[3]) const;
   ut::coords::WCS GetLocationWCS() const;
   void            SetLocationWCS(const ut::coords::WCS& aLocationWCS);
   void            SetLocationWCS(const UtVec3d& aLocationWCS);
   virtual void    SetLocationWCS(const double aLocationWCS[3]);

   void            GetLocationECI(double aLocationECI[3]) const;
   ut::coords::ECI GetLocationECI() const;
   void            SetLocation(const ut::coords::ECI& aLocationECI);
   void            SetLocationECI(const UtVec3d& aLocationECI);
   void            SetLocationECI(const double aLocationECI[3]);

   void    GetLocationTOD(double aLocationTOD[3]) const;
   UtVec3d GetLocationTOD() const;
   void    SetLocationTOD(const UtVec3d& aLocationTOD);
   void    SetLocationTOD(const double aLocationTOD[3]);

   void    GetLocationTEME(double aLocationECI[3]) const;
   UtVec3d GetLocationTEME() const;

   void    GetLocationJ2000(double aLocationJ2000[3]) const;
   UtVec3d GetLocationJ2000() const;

   void GetLocationLLA(double& aLat, double& aLon, double& aAlt) const;

   ut::coords::LLA GetLocationLLA() const;

   virtual void SetLocationLLA(double aLat, double aLon, double aAlt);

   void SetLocation(const ut::coords::LLA& aLLA);

   double GetAltitude() const;

   void SetSpatialECI(const double aSimTime,
                      const double aLocationECI[3],
                      const double aVelocityECI[3],
                      const double aAccelerationECI[3],
                      const double aOrientationECI[3]);

   void SetSpatialECI(const double                   aSimTime,
                      const ut::coords::ECI&         aLocationECI,
                      const ut::coords::ECI&         aVelocityECI,
                      const ut::coords::ECI&         aAccelerationECI,
                      const ut::coords::EulerAngles& aOrientationECI);

   void SetSpatialECI(const double           aSimTime,
                      const ut::coords::ECI& aLocationECI,
                      const ut::coords::ECI& aVelocityECI,
                      const ut::coords::ECI& aAccelerationECI,
                      const UtQuaternion&    aOrientationECI);

   void              SetECI_ReferenceEpoch(const UtCalendar& aEpoch);
   const UtCalendar& GetECI_ReferenceEpoch() const { return mInertialRefEpoch; }

   void SetPolarOffsetAngles(double aOffsetAngleX, double aOffsetAngleY) const;

   UT_DEPRECATED void SetPrecessionNutationUpdateInterval(double aNutationUpdateInterval);
   void               SetNutationUpdateInterval(double aNutationUpdateInterval);

   void SetTime(double aTime) { SetTimeP(aTime); }

   void IncrementLocationWCS(const double aLocationDeltaWCS[3]);
   void IncrementLocation(const ut::coords::WCS& aLocationDeltaWCS);
   void IncrementLocationWCS(const UtVec3d& aLocationDeltaWCS);
   void IncrementLocationNED(const double aLocationDeltaNED[3]);
   void IncrementLocation(const ut::coords::NED& aLocationDeltaNED);
   void IncrementLocationNED(const UtVec3d& aLocationDeltaNED);

   //! Return the coordinate frame used by the last SetLocationXXX call.
   //! This is provided so an application can determine how the position of the entity is defined.
   CoordinateFrame GetCoordinateFrame() const { return mCoordinateFrame; }

   //@}

   //! @name Velocity Get/Set Methods
   //! Velocities are in meters/second.
   //@{
   void            GetVelocityWCS(double aVelocityWCS[3]) const;
   ut::coords::WCS GetVelocityWCS() const;
   void            SetVelocityWCS(const double aVelocityWCS[3]);
   void            SetVelocity(const ut::coords::WCS& aVelocityWCS);
   void            SetVelocityWCS(const UtVec3d& aVelocityWCS);

   void            GetVelocityECI(double aVelocityECI[3]) const;
   ut::coords::ECI GetVelocityECI() const;
   void            SetVelocityECI(const double aVelocityECI[3]);
   void            SetVelocity(const ut::coords::ECI& aVelocityECI);
   void            SetVelocityECI(const UtVec3d& aVelocityECI);

   void    GetVelocityTOD(double aVelocityECI[3]) const;
   UtVec3d GetVelocityTOD() const;
   void    SetVelocityTOD(const double aVelocityECI[3]);
   void    SetVelocityTOD(const UtVec3d& aVelocityECI);

   void            GetVelocityNED(double aVelocityNED[3]) const;
   ut::coords::NED GetVelocityNED() const;
   void            SetVelocityNED(const double aVelocityNED[3]);
   void            SetVelocity(const ut::coords::NED& aVelocityNED);
   void            SetVelocityNED(const UtVec3d& aVelocityNED);

   void            GetVelocityECS(double aVelocityECS[3]) const;
   ut::coords::ECS GetVelocityECS() const;
   void            SetVelocityECS(const double aVelocityECS[3]);
   void            SetVelocity(const ut::coords::ECS& aVelocityECS);
   void            SetVelocityECS(const UtVec3d& aVelocityECS);

   void    GetVelocityTEME(double aVelocityTEME[3]) const;
   UtVec3d GetVelocityTEME() const;

   void    GetVelocityJ2000(double aVelocityJ2000[3]) const;
   UtVec3d GetVelocityJ2000() const;

   double GetSpeed() const;
   double GetSpeedECI() const;
   double GetSpeedWCS() const;
   double GetSpeedSquared() const;
   //@}

   //! @name Acceleration Get/Set Methods
   //! Accelerations are in meters/second^2
   //@{
   void            GetAccelerationWCS(double aAccelerationWCS[3]) const;
   ut::coords::WCS GetAccelerationWCS() const;
   void            SetAccelerationWCS(const double aAccelerationWCS[3]);
   void            SetAcceleration(const ut::coords::WCS& aAccelerationWCS);
   void            SetAccelerationWCS(const UtVec3d& aAccelerationWCS);

   void            GetAccelerationECI(double aAccelerationECI[3]) const;
   ut::coords::ECI GetAccelerationECI() const;
   void            SetAccelerationECI(const double aAccelerationECI[3]);
   void            SetAcceleration(const ut::coords::ECI& aAccelerationECI);
   void            SetAccelerationECI(const UtVec3d& aAccelerationECI);

   void    GetAccelerationTOD(double aAccelerationECI[3]) const;
   UtVec3d GetAccelerationTOD() const;
   void    SetAccelerationTOD(const double aAccelerationECI[3]);
   void    SetAccelerationTOD(const UtVec3d& aAccelerationECI);

   void            GetAccelerationNED(double aAccelerationNED[3]) const;
   ut::coords::NED GetAccelerationNED() const;
   void            SetAccelerationNED(const double aAccelerationNED[3]);
   void            SetAcceleration(const ut::coords::NED& aAccelerationNED);
   void            SetAccelerationNED(const UtVec3d& aAccelerationNED);

   void            GetAccelerationECS(double aAccelerationECS[3]) const;
   ut::coords::ECS GetAccelerationECS() const;
   void            SetAccelerationECS(const double aAccelerationECS[3]);
   void            SetAcceleration(const ut::coords::ECS& aAccelerationECS);
   void            SetAccelerationECS(const UtVec3d& aAccelerationECS);

   double GetAccelerationMagnitude() const;
   double GetAccelerationMagnitudeECI() const;
   double GetAccelerationMagnitudeWCS() const;
   //@}

   //! @name Orientation methods
   //!
   //! The orientation specifies the orientation of the ECS in WCS,
   //! ECI, or NED.  Psi/Theta/Phi (or Heading/Pitch/Roll) specify the
   //! angles of three consecutive rotations that orient the ECS.  The
   //! rotations occurs as follows:
   //!
   //! \li First    rotate about Z by Psi/Heading giving X'Y'Z'
   //! \li Next     rotate about Y by Theta/Pitch giving X'Y'Z'
   //! \li Finally, rotate about X by Phi/Roll    giving the entity coordinate system.
   //!
   //! The angles must be in radians in the following ranges:
   //! \li -PI \<= Psi \<= PI
   //! \li -PI/2 \<= Theta \<= PI/2
   //! \li -PI \<= Phi \<= PI
   //@{
   void                    GetOrientationWCS(double& aPsiWCS, double& aThetaWCS, double& aPhiWCS) const;
   ut::coords::EulerAngles GetOrientationWCS() const;
   UtQuaternion            GetOrientationWCSq() const;

   virtual void SetOrientationWCS(double aPsiWCS, double aThetaWCS, double aPhiWCS);
   void         SetOrientationWCS(const ut::coords::EulerAngles& aAnglesWCS);
   void         SetOrientationWCS(const UtQuaternion& aOrientationWCS);

   void                    GetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const;
   ut::coords::EulerAngles GetOrientationECI() const;
   UtQuaternion            GetOrientationECIq() const;

   virtual void SetOrientationECI(double aPsiECI, double aThetaECI, double aPhiECI);
   void         SetOrientationECI(const ut::coords::EulerAngles& aAnglesECI);
   void         SetOrientationECI(const UtQuaternion& aOrientationWCS);

   void                    GetOrientationNED(double& aHeadingNED, double& aPitchNED, double& aRollNED) const;
   ut::coords::EulerAngles GetOrientationNED() const;
   UtQuaternion            GetOrientationNEDq() const;

   virtual void SetOrientationNED(double aHeadingNED, double aPitchNED, double aRollNED);
   void         SetOrientationNED(const ut::coords::EulerAngles& aAnglesNED);
   void         SetOrientationNED(const UtQuaternion& aOrientationNED);

   UtDCM GetDcmWCStoECS() const;
   UtDCM GetDcmWCStoNED() const;

   //@}

   //! @name Relative Geometry Methods
   //@{
   void ComputeAspect(const double aRelativeLocationWCS[3], double& aRelativeAzimuth, double& aRelativeElevation) const;
   ut::coords::Aspect ComputeAspect(ut::coords::WCS aRelativeLocationWCS) const;

   void               ComputeAspectECI(double          aSimTime,
                                       const UtEntity* aOtherEntityPtr,
                                       double          aRelativeLocationECI[3],
                                       double&         aRelativeAzimuth,
                                       double&         aRelativeElevation) const;
   ut::coords::Aspect ComputeAspectECI(double aSimTime, const UtEntity* aOtherEntityPtr) const;

   void            GetRelativeLocationWCS(double aRelativeAzimuth,
                                          double aRelativeElevation,
                                          double aDistance,
                                          double aRelativeLocationWCS[3]) const;
   ut::coords::WCS GetRelativeLocationWCS(ut::coords::Aspect aAspect, double aDistance) const;

   void            GetRelativeLocationWCS(const double aOtherLocationWCS[3], double aLocationWCS[3]) const;
   ut::coords::WCS GetRelativeLocationWCS(const ut::coords::WCS& aOtherLocationWCS) const;

   void            GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr, double aLocationWCS[3]) const;
   ut::coords::WCS GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr) const;

   void GetRelativeLocationECI(double aSimTime, const UtEntity* aOtherEntityPtr, double aLocationECI[3]) const;
   ut::coords::ECI GetRelativeLocationECI(double aSimTime, const UtEntity* aOtherEntityPtr) const;

   void            GetRelativeLocationNED(const UtEntity* aOtherEntityPtr, double aLocationNED[3]) const;
   ut::coords::NED GetRelativeLocationNED(const UtEntity* aOtherEntityPtr) const;

   void            GetRelativeLocationECS(const UtEntity* aOtherEntityPtr, double aLocationECS[3]) const;
   ut::coords::ECS GetRelativeLocationECS(const UtEntity* aOtherEntityPtr) const;
   //@}

   //! @name Methods to convert between global and local coordinates frames.
   //@{
   void            ConvertECSToWCS(const double aLocationECS[3], double aLocationWCS[3]) const;
   ut::coords::WCS ConvertECSToWCS(const ut::coords::ECS& aLocationECS) const;

   void            ConvertNEDToWCS(const double aLocationNED[3], double aLocationWCS[3]) const;
   ut::coords::WCS ConvertNEDToWCS(const ut::coords::NED& aLocationNED) const;

   void            ConvertNEDToECI(const double aLocationNED[3], double aLocationECI[3]) const;
   ut::coords::ECI ConvertNEDToECI(const ut::coords::NED& aLocationNED) const;

   void            ConvertWCSToECI(const double aLocationWCS[3], double aLocationECI[3]) const;
   ut::coords::ECI ConvertWCSToECI(const ut::coords::WCS& aLocationWCS) const;

   void            ConvertECIToWCS(const double aLocationECI[3], double aLocationWCS[3]) const;
   ut::coords::WCS ConvertECIToWCS(const ut::coords::ECI& aLocationECI) const;

   void            ConvertNEDToLLA(const double aLocationNED[3], double& aLat, double& aLon, double& aAlt) const;
   ut::coords::LLA ConvertNEDToLLA(const ut::coords::NED& aLocationNED) const;

   void            ConvertLLAToNED(double aLat, double aLon, double aAlt, double aLocationNED[3]) const;
   ut::coords::NED ConvertLLAToNED(const ut::coords::LLA& aLLA) const;

   void            ConvertWCSToNED(const double aLocationWCS[3], double aLocationNED[3]) const;
   ut::coords::NED ConvertWCSToNED(const ut::coords::WCS& aLocationWCS) const;

   void            ConvertECIToNED(const double aLocationECI[3], double aLocationNED[3]) const;
   ut::coords::NED ConvertECIToNED(const ut::coords::ECI& aLocationECI) const;

   void            ConvertWCSToECS(const double aLocationWCS[3], double aLocationECS[3]) const;
   ut::coords::ECS ConvertWCSToECS(const ut::coords::WCS& aLocationWCS) const;

   void ComputeWCSToLocalTransform(double aWCSToLocalTransform[3][3], const double aECSToLocalTransform[3][3]) const;

   void            ConvertWCSVectorToNED(double aVectorNED[3], const double aVectorWCS[3]) const;
   ut::coords::NED ConvertWCSVectorToNED(const ut::coords::WCS& aVectorWCS) const;

   void            ConvertNEDVectorToWCS(double aVectorWCS[3], const double aVectorNED[3]) const;
   ut::coords::WCS ConvertNEDVectorToWCS(const ut::coords::NED& aVectorNED) const;

   void            ConvertECSVectorToWCS(double aVectorWCS[3], const double aVectorECS[3]) const;
   ut::coords::WCS ConvertECSVectorToWCS(const ut::coords::ECS& aVectorECS) const;

   void            ConvertWCSVectorToECS(double aVectorECS[3], const double aVectorWCS[3]) const;
   ut::coords::ECS ConvertWCSVectorToECS(const ut::coords::WCS& aVectorWCS) const;

   void ConvertVelocityECIToWCS(const double aLocationECI[3], const double aVelECI[3], double aVelWCS[3]) const;
   ut::coords::WCS ConvertVelocityECIToWCS(const ut::coords::ECI& aLocationECI, const ut::coords::ECI& aVelECI) const;

   void ConvertVelocityWCSToECI(const double aLocationWCS[3], const double aVelWCS[3], double aVelECI[3]) const;
   ut::coords::ECI ConvertVelocityWCSToECI(const ut::coords::WCS& aLocationWCS, const ut::coords::WCS& aVelWCS) const;

   //@}

   //! Static methods to convert between the various global frames.
   //@{

   static void
   ConvertLLAToWCS(double aLat, double aLon, double aAlt, const ut::CentralBodyEllipsoid& aGeoid, double aLocationWCS[3]);

   static void ConvertWCSToLLA(const double                    aLocationWCS[3],
                               const ut::CentralBodyEllipsoid& aGeoid,
                               double&                         aLat,
                               double&                         aLon,
                               double&                         aAlt);

   //! Compatibility methods that use the Earth (TODO evaluate)
   static void ConvertLLAToWCS(double aLat, double aLon, double aAlt, double aLocationWCS[3]);

   static void ConvertWCSToLLA(const double aLocationWCS[3], double& aLat, double& aLon, double& aAlt);

   //! @name Utility Routines
   //! The following routines are used by other routines in the class but are
   //! exposed because of their potential usefulness.
   //@{
   static void ComputeAzimuthAndElevation(const double aLocation[3], double& aAzimuth, double& aElevation);

   static void ComputeRotationalTransform(double aPsi, double aTheta, double aPhi, double aTransform[3][3]);

   static void ExtractEulerAngles(const double aTransform[3][3], double& aPsi, double& aTheta, double& aPhi);

   void GetNEDToECSTransform(double aNEDToECSTransform[3][3]) const;
   void GetWCSToNEDTransform(double aWCSToNEDTransform[3][3]) const;
   void GetWCSToECSTransform(double aWCSToECSTransform[3][3]) const;
   void GetECIToWCSTransform(double aWCSToECSTransform[3][3]) const;

   double RelativeBearing(const double aTargetLocWCS[3]) const;
   double RelativeBearing(const ut::coords::WCS& aTargetLocWCS) const;
   double RelativeBearing(const UtVec3d& aTargetLocWCS) const;

   double TrueBearing(const double aTargetLocWCS[3]) const;
   double TrueBearing(const ut::coords::WCS& aTargetLocWCS) const;
   double TrueBearing(const UtVec3d& aTargetLocWCS) const;

   //@}

private:
   ut::CloneablePtr<ut::CentralPoint> mCentralPointPtr;

   void UpdateLLALocation() const;
   void UpdateWCSLocation() const;
   void UpdateInertialLocation(double aSimTime) const; // Inertial update with time update.
   void UpdateInertialLocation() const;                // Inertial update with no time update.
   void UpdateInertialVelocity() const;
   void UpdateInertialAcceleration() const;

   void UpdateNEDToECSTransform() const;
   void UpdateWCSToNEDTransform() const;
   void UpdateWCSToECSTransform() const;
   void UpdateECIToECSTransform() const;

   void SetTimeP(double aTime) const;

   inline void WasMoved();
   inline void WasRotated();

   /* Mutable and Const Correctness
      This class make use of semantic const (vs syntactic const) to provide a public interface which
      uses const as a user would expect it to be used.  This allows a class to derive from UtEntity
      and have Accessors be const so they can be accessed from a const reference to the class.

      The mutable data member of this class are data caches to store the position, velocity, and
      acceleration information in different coordinate systems.  The desire is not to compute this
      information until it is needed, and then store it until the data is no longer valid.  This
      means an accessor such as GetLocationWCS() may have to update the cache to store data for the
      Location in WCS coordinate system.  The position is not actually changed, just the cache data
      within the class.
      */

   mutable double mUpdateTime         = 0.0;
   mutable double mInertialUpdateTime = -1.0; // If ECI is being used, keep current to mUpdateTime

   mutable double mLocationWCS[3]{0.0, 0.0, 0.0}; // position in the WCS
   mutable double mLat{0.0};
   mutable double mLon{0.0};
   mutable double mAlt{0.0};

   mutable double mVelocityWCS[3]{0.0, 0.0, 0.0};     // velocity in the WCS
   mutable double mVelocityNED[3]{0.0, 0.0, 0.0};     // velocity in the NED frame
   mutable double mAccelerationWCS[3]{0.0, 0.0, 0.0}; // acceleration in the WCS
   mutable double mAccelerationNED[3]{0.0, 0.0, 0.0}; // acceleration in the NED frame

   // Following are the three Euler angles needed to orient the entity coordinate
   // system in the world coordinate system.

   mutable double mPsiWCS{0.0};   // Radians
   mutable double mThetaWCS{0.0}; // Radians
   mutable double mPhiWCS{0.0};   // Radians

   mutable double mPsiECI{0.0};   // Radians
   mutable double mThetaECI{0.0}; // Radians
   mutable double mPhiECI{0.0};   // Radians

   mutable double mHeadingNED{0.0}; // Radians
   mutable double mPitchNED{0.0};   // Radians
   mutable double mRollNED{0.0};    // Radians

   // Transformation from WCS -> NED
   mutable double mWCSToNEDTransform[3][3]{{0.0}};

   // Transformation from NED -> ECS
   mutable double mNEDToECSTransform[3][3]{{0.0}};

   // Transformation from WCS -> ECS
   mutable double mWCSToECSTransform[3][3]{{0.0}};

   // Transformation from ECI -> ECS
   mutable double mECIToECSTransform[3][3]{{0.0}};

   CoordinateFrame mCoordinateFrame{cCF_LLA}; // Coordinate frame from last SetLocationXXX call

   union
   {
      struct
      {
         mutable bool mLLALocationIsValid : 1;
         mutable bool mWCSLocationIsValid : 1;
         mutable bool mInertialLocationIsValid : 1;
         mutable bool mWCSVelocityIsValid : 1;
         mutable bool mNEDVelocityIsValid : 1;
         mutable bool mInertialVelocityIsValid : 1;
         mutable bool mWCSAccelerationIsValid : 1;
         mutable bool mNEDAccelerationIsValid : 1;
         mutable bool mInertialAccelerationIsValid : 1;
         mutable bool mWCSOrientationIsValid : 1;
         mutable bool mNEDOrientationIsValid : 1;
         mutable bool mInertialOrientationIsValid : 1;
         mutable bool mWCSToNEDTransformIsValid : 1;
         mutable bool mNEDToECSTransformIsValid : 1;
         mutable bool mWCSToECSTransformIsValid : 1;
         mutable bool mECIToECSTransformIsValid : 1;
      };
      unsigned int mFlags;
   };

   UtECI_Conversion& GetECI_Conversion() const;

   mutable ut::CloneablePtr<UtECI_Conversion> mInertialConversionPtr{nullptr};

   // Initial values that are needed by mInertialConversionPtr (if used).
   UtCalendar mInertialRefEpoch;
   double     mNutationUpdateInterval = 1000.0; // s
};

#endif
