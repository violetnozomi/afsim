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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTENTITYPART_HPP
#define UTENTITYPART_HPP

#include "ut_export.h"
class UtEntity;
#include "UtCoords.hpp"
#include "UtQuaternion.hpp"

//! Represents the position and orientation of a sub-part of an entity.
//!
//! UtEntityPart is used to represent a part of an entity such as an antenna for
//! a transmitter or receiver.  It contains the position and orientation of the
//! part RELATIVE TO ITS OWNING PART.
//!
//! The 'Part Coordinate System' (PCS) uses the same orientation conventions as
//! the 'Entity Coordinate System' (ECS) described in UtEntity.
//!
//! @see UtEntity

class UT_EXPORT UtEntityPart
{
public:
   UtEntityPart(UtEntity* aOwningEntityPtr = nullptr);
   UtEntityPart(const UtEntityPart& aOther);
   UtEntityPart& operator              =(const UtEntityPart& aRhs);
   UtEntityPart(UtEntityPart&& aOther) = default;
   UtEntityPart& operator=(UtEntityPart&& aRhs) = default;
   virtual ~UtEntityPart()                      = default;

   //! The following routine \b MUST be called if the owning entity
   //! changes position or orientation.
   virtual void InvalidateTransform()
   {
      mLocationWCSIsValid       = false;
      mWCSToPCSTransformIsValid = false;
   }

   //! Define the entity to which this part is attached.
   //! This \b MUST be defined if the owning entity was not supplied on the constructor.
   virtual void SetOwningEntity(UtEntity* aOwningEntityPtr)
   {
      mOwningEntityPtr = aOwningEntityPtr;
      InvalidateTransform();
   }

   //! Get the pointer to the entity to which this part is attached.
   UtEntity* GetOwningEntity() const { return mOwningEntityPtr; }

   //! @name Location Methods
   //! Distances are in meters.
   //@{
   virtual void    GetLocation(double aLocationECS[3]) const;
   ut::coords::ECS GetLocation() const;
   ut::coords::ECS GetLocationECS() const;

   void         SetLocation(const ut::coords::ECS& aLocationECS);
   void         SetLocationECS(const ut::coords::ECS& aLocationECS);
   virtual void SetLocation(const double aLocationECS[3]);

   virtual void    GetLocationLLA(double& aLat, double& aLon, double& aAlt) const;
   ut::coords::LLA GetLocationLLA() const;

   ut::coords::WCS GetLocationWCS() const;
   virtual void    GetLocationWCS(double aLocationWCS[3]) const;

   ut::coords::ECI GetLocationECI() const;
   virtual void    GetLocationECI(double aLocationECI[3]) const;
   //@}

   //! @name Orientation Methods
   //! Angles are in radians.
   //@{
   ut::coords::EulerAngles GetOrientationECS() const;
   ut::coords::EulerAngles GetOrientation() const;
   UtQuaternion            GetOrientationECSq() const;
   virtual void            GetOrientation(double& aYaw, double& aPitch, double& aRoll) const;

   void         SetOrientation(const ut::coords::EulerAngles& aOrientation);
   void         SetOrientation(const UtQuaternion& aOrientation);
   void         SetOrientationECS(const ut::coords::EulerAngles& aOrientation);
   void         SetOrientationECS(const UtQuaternion& aOrientation);
   virtual void SetOrientation(double aYaw, double aPitch, double aRoll);

   ut::coords::EulerAngles GetOrientationWCS() const;
   virtual void            GetOrientationWCS(double& aPsiWCS, double& aThetaWCS, double& aPhiWCS) const;
   UtQuaternion            GetOrientationWCSq() const;

   ut::coords::EulerAngles GetOrientationECI() const;
   virtual void            GetOrientationECI(double& aPsiECI, double& aThetaECI, double& aPhiECI) const;
   UtQuaternion            GetOrientationECIq() const;

   void         Rotate(const ut::coords::EulerAngles& aRotation);
   void         Rotate(const UtQuaternion& aRotation);
   virtual void Rotate(double aYaw, double aPitch, double aRoll);
   //@}

   //! @name Relative Geometry Methods
   //@{
   ut::coords::Aspect ComputeAspect(const ut::coords::WCS& aRelativeLocationWCS);
   virtual void ComputeAspect(const double aRelativeLocationWCS[3], double& aRelativeAzimuth, double& aRelativeElevation);

   // No vector version included due to ambiguity with entity location GetLocationWCS above
   virtual void GetLocationWCS(const double aRelativeLocationWCS[3], double aLocationWCS[3]);

   ut::coords::WCS GetRelativeLocationWCS(const ut::coords::Aspect& aAspect, double aDistance);
   virtual void    GetRelativeLocationWCS(double aRelativeAzimuth,
                                          double aRelativeElevation,
                                          double aDistance,
                                          double aRelativeLocationWCS[3]);

   ut::coords::WCS GetRelativeLocationWCS(const ut::coords::WCS& aOtherLocationWCS) const;
   virtual void    GetRelativeLocationWCS(const double aOtherLocationWCS[3], double aRelativeLocationWCS[3]) const;

   ut::coords::WCS GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr) const;
   virtual void    GetRelativeLocationWCS(const UtEntity* aOtherEntityPtr, double aRelativeLocationWCS[3]) const;

   ut::coords::WCS GetRelativeLocationWCS(const UtEntityPart* aOtherEntityPtr) const;
   virtual void    GetRelativeLocationWCS(const UtEntityPart* aOtherPartPtr, double aRelativeLocationWCS[3]) const;
   //@}

   //! @name Utility Routines
   //! The following routines are used by other routines in the class but are
   //! exposed because of their potential usefulness.
   //@{
   virtual void ComputeRotationalTransform(double aYaw, double aPitch, double aRoll, double aWCS_ToPartTransform[3][3]);

   ut::coords::WCS ConvertPCSToWCS(const ut::coords::PCS& aLocationPCS) const;
   ut::coords::ECS ConvertPCSToECS(const ut::coords::PCS& aLocationPCS) const;

   ut::coords::PCS ConvertWCSToPCS(const ut::coords::WCS& aLocationWCS) const;
   ut::coords::PCS ConvertECSToPCS(const ut::coords::ECS& aLocationECS) const;

   ut::coords::PCS ConvertWCSVectorToPCS(const ut::coords::WCS& aVectorWCS) const;
   virtual void    ConvertWCSVectorToPCS(double aVectorPCS[3], const double aVectorWCS[3]) const;

   ut::coords::WCS ConvertPCSVectorToWCS(const ut::coords::PCS& aVectorPCS) const;
   virtual void    ConvertPCSVectorToWCS(double aVectorWCS[3], const double aVectorPCS[3]) const;

   ut::coords::PCS ConvertECSVectorToPCS(const ut::coords::ECS& aVectorECS) const;
   virtual void    ConvertECSVectorToPCS(double aVectorPCS[3], const double aVectorECS[3]) const;

   ut::coords::ECS ConvertPCSVectorToECS(const ut::coords::PCS& aVectorPCS) const;
   virtual void    ConvertPCSVectorToECS(double aVectorECS[3], const double aVectorPCS[3]) const;

   virtual void GetECSToPCSTransform(double aECSToPCSTransform[3][3]) const;

   virtual void GetWCSToPCSTransform(double aWCSToPCSTransform[3][3]) const;
   //@}

private:
   void UpdateLocationWCS() const;
   void UpdateECSToPCSTransform() const;
   void UpdateWCSToPCSTransform() const;

   UtEntity* mOwningEntityPtr; // pointer to owning entity

   //! The origin of the PCS with respect to the host entity.
   double mLocationECS[3];

   //! The orientation of the PCS with respect to the host entity.
   double mYaw;
   double mPitch;
   double mRoll;

   mutable double mLocationWCS[3];
   mutable double mECSToPCSTransform[3][3];
   mutable double mWCSToPCSTransform[3][3];

   mutable bool mLocationWCSIsValid;
   mutable bool mECSToPCSTransformIsValid;
   mutable bool mWCSToPCSTransformIsValid;
};

#endif
