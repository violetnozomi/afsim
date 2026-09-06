// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSPACEORIENTATION_HPP
#define WSFSPACEORIENTATION_HPP

#include "wsf_space_export.h"

#include <memory>
#include <string>

#include "UtException.hpp"
class UtInput;
#include "UtVec3.hpp"

class WsfAttitudeController;
#include "WsfGeoPoint.hpp"
#include "WsfObject.hpp"
#include "WsfTrackId.hpp"

namespace wsf
{

namespace space
{
//! A class hierarchy encapsulating various options for satellite attitude profiles:
//! The profiles are as follows:
//!   <ul>
//!   <li> ECI velocity with nadir constraint:\n
//!   The satellite's x-axis is pointing along the satellite's velocity vector,
//!   the z axis is constrained perpendicular to x and in the plane of the x-axis and the
//!   nadir vector.
//!   <li> ECI velocity with solar constraint:\n
//!   The satellite's x-axis is pointing along the satellite's velocity vector,
//!   the z axis is constrained perpendicular to x and in the plane of the x-axis and the
//!   sun vector.
//!   <li> Nadir pointing with ECI Velocity Constraint:\n
//!   The satellite's z-axis is pointing to the earth's center,
//!   the x axis is constrained perpendicular to z and in the plane of the z-axis and the
//!   ECI velocity vector.
//!   <li> Nadir pointing with ECEF Velocity Constraint:\n
//!   The satellite's z-axis is pointing to the earth's center,
//!   the x axis is constrained perpendicular to z and in the plane of the z-axis and the
//!   ECI velocity vector.
//!   <li> Solar pointing with nadir constraint:\n
//!   The satellite's negative z-axis is pointing to the sun,
//!   the x axis is constrained perpendicular to z and in the plane of the z-axis and the
//!   nadir.
//!   <li> Nadir pointing with solar constraint:\n
//!   The satellite's negative z-axis is pointing to the nadir,
//!   the x axis is constrained perpendicular to z and in the plane of the z-axis and the
//!   sun vector.
//!   <li> Entity (Resident Space Object) pointing with solar constraint: \n
//!   The satellite's x-axis points at the Entity,
//!   the z axis is constrained perpendicular to x and in the plane of the x-axis and the
//!   solar vector.
//!   <li> Entity (Resident Space Object) pointing with nadir constraint: \n
//!   The satellite's x-axis points at the Entity,
//!   the z axis is constrained perpendicular to x and in the plane of the x-axis and the
//!   nadir vector.
//!   <li> Entity (Resident Space Object) pointing with orbit constraint: \n
//!   The satellite's x-axis points at the Entity,
//!   the z axis is constrained perpendicular to x and in the orbital plane.
//!   <li> Orientation point pointing with orbit constraint: \n
//!   The satellite's x-axis points at a geo-point (WsfGeoPoint),
//!   the z axis is constrained perpendicular to x and in the orbital plane.
//!   </ul>
//!
//!   For all orientation types, the y-axis is perpendicular to x and z
//!   in a right-handed coordinate system.
class WSF_SPACE_EXPORT Orientation : public WsfObject
{
public:
   explicit Orientation(bool aIsDefaultX_Aligned)
      : mDefaultX_Aligned(aIsDefaultX_Aligned)
   {
   }

   Orientation(const Orientation& aSrc) = default;
   ~Orientation() override              = default;

   static std::unique_ptr<Orientation> Factory(const std::string& aOrientation);

   Orientation* Clone() const override = 0;

   const char* GetScriptClassName() const override;

   //! Set the automatic orientation to be used with the propagator.
   void SetAttitudeController(const WsfAttitudeController& aController) { mControllerPtr = &aController; }

   bool ProcessInput(UtInput& aInput) override { return false; }

   //! Return whether the pointing and/or constraint lines-of-sight vectors are valid.
   //!@note This method returns false if an object being pointed at (e.g., another platform)
   //!      does not or no longer exists.
   virtual bool IsValid() const { return true; }

   //! Calculate the desired spacecraft orientation for the current location and velocity.
   //!@param aPsi The calculated ECI 'heading' angle.
   //!@param aTheta The calculated ECI 'pitch' angle.
   //!@param aPhi The calculated ECI 'roll' angle.
   virtual bool Calculate(double& aPsi, double& aTheta, double& aPhi) const = 0;

   void CalculateAligned(const UtVec3d& aPointingAxisOrientationECI,
                         const UtVec3d& aConstraintAxisOrientationECI,
                         double&        aPsi,
                         double&        aTheta,
                         double&        aPhi) const;

   static void CalculateGeneral(double         aYawECS,
                                double         aPitchECS,
                                double         aRollECS,
                                const UtVec3d& aX_AxisOrientationECI,
                                const UtVec3d& aZ_AxisConstraintECI,
                                double&        aPsi,
                                double&        aTheta,
                                double&        aPhi);

   static void CalculateX_Aligned(const UtVec3d& aX_AxisOrientationECI,
                                  const UtVec3d& aZ_AxisConstraintECI,
                                  double&        aPsi,
                                  double&        aTheta,
                                  double&        aPhi);

   static void CalculateZ_Aligned(const UtVec3d& aZ_AxisOrientationECI,
                                  const UtVec3d& aX_AxisConstraintECI,
                                  double&        aPsi,
                                  double&        aTheta,
                                  double&        aPhi);

   static void CalculatePCS_DirectionCosines(const UtVec3d& aX_AxisOrientationECI,
                                             const UtVec3d& aZ_AxisConstraintECI,
                                             UtVec3d&       aX_DC,
                                             UtVec3d&       aY_DC,
                                             UtVec3d&       aZ_DC);

   void SetDefaultX_Aligned(bool aIsDefaultX_Aligned) { mDefaultX_Aligned = aIsDefaultX_Aligned; }
   bool GetDefaultX_Aligned() const { return mDefaultX_Aligned; }

protected:
   UtVec3d Nadir() const;
   UtVec3d Solar() const;
   UtVec3d VelocityECI() const;
   UtVec3d VelocityECEF() const;
   UtVec3d OrbitPlaneConstraint(const UtVec3d& aAlignVector) const;

   void HandleException(const UtException& aException) const;

   mutable bool                 mIssueErrorMessage{true};
   bool                         mDefaultX_Aligned;
   const WsfAttitudeController* mControllerPtr{nullptr};
};

class OrientationNone : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "none"; }
   OrientationNone();
   Orientation* Clone() const override { return new OrientationNone(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override { return false; }
};

class OrientationNadirECI_Velocity : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "nadir_with_eci_velocity_constraint"; }
   OrientationNadirECI_Velocity();
   Orientation* Clone() const override { return new OrientationNadirECI_Velocity(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(Nadir(), VelocityECI(), aPsi, aTheta, aPhi);
      return true;
   }
};

class OrientationSolarNadir : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "solar_with_nadir_constraint"; }
   OrientationSolarNadir();
   Orientation* Clone() const override { return new OrientationSolarNadir(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(Solar(), Nadir(), aPsi, aTheta, aPhi);
      return true;
   }
};

class OrientationNadirSolar : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "nadir_with_solar_constraint"; }
   OrientationNadirSolar();
   Orientation* Clone() const override { return new OrientationNadirSolar(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(Nadir(), Solar(), aPsi, aTheta, aPhi);
      return true;
   }
};

class OrientationNadirECEF_Velocity : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "nadir_with_ecef_velocity_constraint"; }
   OrientationNadirECEF_Velocity();
   Orientation* Clone() const override { return new OrientationNadirECEF_Velocity(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(Nadir(), VelocityECEF(), aPsi, aTheta, aPhi);
      return true;
   }
};

class OrientationECI_VelocityNadir : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "eci_velocity_with_nadir_constraint"; }
   OrientationECI_VelocityNadir();
   Orientation* Clone() const override { return new OrientationECI_VelocityNadir(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(VelocityECI(), Nadir(), aPsi, aTheta, aPhi);
      return true;
   }
};

class OrientationECI_VelocitySolar : public Orientation
{
public:
   static constexpr const char* GetTypeName() { return "eci_velocity_with_solar_constraint"; }
   OrientationECI_VelocitySolar();
   Orientation* Clone() const override { return new OrientationECI_VelocitySolar(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override
   {
      CalculateAligned(VelocityECI(), Solar(), aPsi, aTheta, aPhi);
      return true;
   }
};

class WSF_SPACE_EXPORT OrientationEntity : public Orientation
{
public:
   OrientationEntity();
   bool ProcessInput(UtInput& aInput) override;

   void               SetEntityName(const std::string& aEntityName);
   const std::string& GetEntityName() const { return mEntityName; }

   void              SetTrackId(const WsfTrackId& aTrackId);
   const WsfTrackId& GetLocalTrackId() const { return mTrackId; }

   bool IsValid() const override;
   class EntityNotFoundException : public UtException
   {
   public:
      explicit EntityNotFoundException(const std::string& aEntityName)
         : UtException("Could not find platform (Entity) " + aEntityName)
      {
      }
   };
   class TrackNotFoundException : public UtException
   {
   public:
      explicit TrackNotFoundException(const std::string& aTrackId)
         : UtException("Could not find track with track id " + aTrackId)
      {
      }
   };
   class TrackLocationInvalidException : public UtException
   {
   public:
      explicit TrackLocationInvalidException(const std::string& aTrackId)
         : UtException("Track location invalid for track id " + aTrackId)
      {
      }
   };

protected:
   UtVec3d TargetVector() const;

   std::string mEntityName;
   WsfTrackId  mTrackId;
   bool        mOrientToTrack = false;

private:
   WsfPlatform* GetTargetEntity() const;
};

class OrientationEntitySolar : public OrientationEntity
{
public:
   static constexpr const char* GetTypeName() { return "entity_with_solar_constraint"; }
   OrientationEntitySolar();
   Orientation* Clone() const override { return new OrientationEntitySolar(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override;
};

class OrientationEntityNadir : public OrientationEntity
{
public:
   static constexpr const char* GetTypeName() { return "entity_with_nadir_constraint"; }
   OrientationEntityNadir();
   Orientation* Clone() const override { return new OrientationEntityNadir(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override;
};

class OrientationEntityOrbitPlane : public OrientationEntity
{
public:
   static constexpr const char* GetTypeName() { return "entity_with_orbit_plane_constraint"; }
   OrientationEntityOrbitPlane();
   Orientation* Clone() const override { return new OrientationEntityOrbitPlane(*this); }

protected:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override;
};

class WSF_SPACE_EXPORT OrientationGeoPoint : public Orientation
{
public:
   OrientationGeoPoint();
   OrientationGeoPoint(const OrientationGeoPoint& aSrc);

   bool               ProcessInput(UtInput& aInput) override;
   bool               IsValid() const override;
   void               SetGeoPoint(const WsfGeoPoint& aGeoPoint);
   void               SetGeoPointName(const std::string& aName) { mGeoPointName = aName; }
   const std::string& GetGeoPointName() const { return mGeoPointName; }
   class GeoPointNotFoundException : public UtException
   {
   public:
      explicit GeoPointNotFoundException(const std::string& aGeoPointName)
         : UtException("Geo Point reference " + aGeoPointName + " not found ")
      {
      }
   };

protected:
   UtVec3d                              TargetVector() const;
   std::string                          mGeoPointName;
   mutable std::unique_ptr<WsfGeoPoint> mGeoPointPtr = nullptr;
};

class WSF_SPACE_EXPORT OrientationPointOrbitPlane : public OrientationGeoPoint
{
public:
   static constexpr const char* GetTypeName() { return "point_with_orbit_plane_constraint"; }
   OrientationPointOrbitPlane();
   OrientationPointOrbitPlane(const OrientationPointOrbitPlane& aSrc) = default;
   Orientation* Clone() const override { return new OrientationPointOrbitPlane(*this); }

private:
   bool Calculate(double& aPsi, double& aTheta, double& aPhi) const override;
};

}; // namespace space

}; // namespace wsf

#endif // WSFSPACEORIENTATION_HPP
