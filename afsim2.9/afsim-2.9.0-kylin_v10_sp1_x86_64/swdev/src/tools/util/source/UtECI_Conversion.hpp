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
#ifndef UTECI_CONVERSION_HPP
#define UTECI_CONVERSION_HPP

#include "ut_export.h"

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtCentralPoint.hpp"
#include "UtCloneablePtr.hpp"
#include "UtMat3.hpp"
#include "UtVec3.hpp"

//! A class that performs conversions to and from inertial and quasi-inertial coordinate frames,
//! as referenced to the WCS frame (World Coordinate System; also known as ITRF, International Terrestrial
//! Reference Frame).  The coordinate frames supported for conversion are the following:
//! - ECI   (AFSIM Earth / Ellipsoid-Centered Inertial; commonly GCRF, Geocentric Celestial Reference Frame)
//! - J2000 (ECI without the "Frame Tie" correction, typically within a meter of ECI)
//! - TOD   (True-of-Date; True Equator, True Equinox of date)
//! - TEME  (True Equator, Mean Equinox, as defined by SGP4 and related propagators)
//!
//! To be fully rigorous in the ECI or J2000 conversions, the x and y polar offset angles should be specified
//! that are correct for the given epoch.  These can be found online; e.g.,
//! http://maia.usno.navy.mil/.
//! These values are defaulted to zero, as they are very small (of order of an arcsecond or less);
//! most simulation applications will likely not need to use them.
class UT_EXPORT UtECI_Conversion final
{
public:
   UtECI_Conversion(const UtCalendar& aEpoch, const ut::CentralPoint& aCentralPoint);

   UtECI_Conversion* Clone() const { return new UtECI_Conversion(*this); }

   void              SetEpoch(const UtCalendar& aEpoch);
   const UtCalendar& GetEpoch() const;
#undef GetCurrentTime // Avoid conflict with Windows macro
   const UtCalendar& GetCurrentTime() const;

   const ut::CentralPoint& GetCentralPoint() const { return *mCentralPointPtr; }
   const ut::CentralBody&  GetCentralBody() const { return mCentralPointPtr->GetAsCentralBody(); }

   void               SetCentralPoint(const ut::CentralPoint& aCentralPoint);
   UT_DEPRECATED void SetCentralBody(const ut::CentralBody& aCentralBody) { SetCentralPoint(aCentralBody); }

   void SetTime(double aTimeSinceEpoch);

   void SetPolarOffsetAngles(double aXp, double aYp);

   double               GetNutationUpdateInterval() const { return mNutationUpdateInterval; }
   void                 SetNutationUpdateInterval(double aNutationUpdateInterval);
   UT_DEPRECATED double GetPrecessionNutationUpdateInterval() const { return mNutationUpdateInterval; }
   UT_DEPRECATED void   SetPrecessionNutationUpdateInterval(double aPrecessionNutationUpdateInterval);

   //! @name Legacy Location Get/Set Methods
   //! Locations are in meters.
   //@{
   void GetLocationWCS(double aLocationWCS[3]) const;
   void SetLocationWCS(const double aLocationWCS[3]);

   void GetLocationECI(double aLocationECI[3]) const;
   void SetLocationECI(const double aLocationECI[3]);
   //@}

   //! @name Location Get/Set Methods
   //! Locations are in meters.
   //@{
   UtVec3d GetLocationWCS() const;
   void    SetLocationWCS(const UtVec3d& aLocationWCS);

   UtVec3d GetLocationECI() const;
   void    SetLocationECI(const UtVec3d& aLocationECI);

   UtVec3d GetLocationTEME() const;
   void    SetLocationTEME(const UtVec3d& aLocationTEME);

   UtVec3d GetLocationTOD() const;
   void    SetLocationTOD(const UtVec3d& aLocationTOD);

   UtVec3d GetLocationJ2000() const;
   void    SetLocationJ2000(const UtVec3d& aLocationJ2000);
   //@}

   //! @name Velocity Get/Set Methods
   //! Velocities are in meters/second.
   //@{
   UtVec3d GetVelocityWCS() const;
   void    SetVelocityWCS(const UtVec3d& aVelocityWCS);

   UtVec3d GetVelocityECI() const;
   void    SetVelocityECI(const UtVec3d& aVelocityECI);

   UtVec3d GetVelocityTEME() const;
   void    SetVelocityTEME(const UtVec3d& aVelocityTEME);

   UtVec3d GetVelocityTOD() const;
   void    SetVelocityTOD(const UtVec3d& aVelocityTOD);

   UtVec3d GetVelocityJ2000() const;
   void    SetVelocityJ2000(const UtVec3d& aVelocityJ2000);
   //@}

   //! @name Legacy velocity Get/Set Methods
   //! Locations are in meters.
   //@{
   void GetVelocityWCS(double aVelocityWCS[3]) const;
   void SetVelocityWCS(const double aVelocityWCS[3]);

   void GetVelocityECI(double aVelocityECI[3]) const;
   void SetVelocityECI(const double aVelocityECI[3]);
   //@}

   //! @name Acceleration Get/Set Methods
   //! Accelerations are in meters/second^2
   //@{
   UtVec3d GetAccelerationWCS() const;
   void    SetAccelerationWCS(const UtVec3d& aAccelerationWCS);

   UtVec3d GetAccelerationECI() const;
   void    SetAccelerationECI(const UtVec3d& aAccelerationECI);

   UtVec3d GetAccelerationTOD() const;
   void    SetAccelerationTOD(const UtVec3d& aAccelerationTOD);

   UtVec3d GetAccelerationTEME() const;
   void    SetAccelerationTEME(const UtVec3d& aAccelerationTEME);
   //@}

   //! @name Legacy acceleration Get/Set Methods
   //! Locations are in meters.
   //@{
   void GetAccelerationWCS(double aAccelerationWCS[3]) const;
   void SetAccelerationWCS(const double aAccelerationWCS[3]);

   void GetAccelerationECI(double aAccelerationECI[3]) const;
   void SetAccelerationECI(const double aAccelerationECI[3]);
   //@}

   const UtMat3d& GetWCS_ToECI_Transform() const;
   const UtMat3d& GetECI_ToWCS_Transform() const;

   void GetWCS_ToECI_Transform(double aWCSToECSTransform[3][3]) const;
   void GetECI_ToWCS_Transform(double aWCSToECSTransform[3][3]) const;

   UtVec3d ConvertWCS_ToECI(const UtVec3d& aLocationWCS) const;
   void    ConvertWCS_ToECI(const double aLocationWCS[3], double aLocationECI[3]) const;

   UtVec3d ConvertECI_ToWCS(const UtVec3d& aLocationECI) const;
   void    ConvertECI_ToWCS(const double aLocationECI[3], double aLocationWCS[3]) const;

   UtVec3d ConvertVelocityWCS_ToECI(const UtVec3d& aLocationWCS, const UtVec3d& aVelWCS) const;
   void    ConvertVelocityWCS_ToECI(const double aLocationWCS[3], const double aVelWCS[3], double aVelECI[3]) const;

   UtVec3d ConvertVelocityECI_ToWCS(const UtVec3d& aLocationECI, const UtVec3d& aVelECI) const;
   void    ConvertVelocityECI_ToWCS(const double aLocationECI[3], const double aVelECI[3], double aVelWCS[3]) const;

   UtVec3d ConvertECI_ToTOD(const UtVec3d& aLocationECI) const;
   UtVec3d ConvertVelocityECI_ToTOD(const UtVec3d& aVelocityECI) const;

   UtVec3d ConvertECI_ToTEME(const UtVec3d& aLocationECI) const;
   UtVec3d ConvertVelocityECI_ToTEME(const UtVec3d& aVelocityECI) const;

   UtVec3d ConvertAccelerationECI_ToWCS(const UtVec3d& aLocationECI, const UtVec3d& aVelECI, const UtVec3d& aAccelECI) const;
   void ConvertAccelerationECI_ToWCS(const double aLocationECI[3],
                                     const double aVelECI[3],
                                     const double aAccelECI[3],
                                     double       aAccelWCS[3]) const;

   UtVec3d ConvertAccelerationWCS_ToECI(const UtVec3d& aLocationWCS, const UtVec3d& aVelWCS, const UtVec3d& aAccelWCS) const;
   void ConvertAccelerationWCS_ToECI(const double aLocationWCS[3],
                                     const double aVelWCS[3],
                                     const double aAccelWCS[3],
                                     double       aAccelECI[3]) const;

   // The following are primarily of interest for testing:
   UtVec3d GetLocationPEF() const;
   UtVec3d GetLocationMOD() const;

private:
   union
   {
      struct
      {
         mutable bool mWCSLocationIsSet : 1;
         mutable bool mWCSVelocityIsSet : 1;
         mutable bool mWCSAccelerationIsSet : 1;

         mutable bool mECILocationIsSet : 1;
         mutable bool mECIVelocityIsSet : 1;
         mutable bool mECIAccelerationIsSet : 1;

         mutable bool mTODLocationIsSet : 1;
         mutable bool mTODVelocityIsSet : 1;
         mutable bool mTODAccelerationIsSet : 1;

         mutable bool mPEFLocationIsSet : 1;
      };
      mutable unsigned int mSetFlags = 0;
   };

   union
   {
      struct
      {
         mutable bool mWCSLocationIsValid : 1;
         mutable bool mWCSVelocityIsValid : 1;
         mutable bool mWCSAccelerationIsValid : 1;

         mutable bool mECILocationIsValid : 1;
         mutable bool mECIVelocityIsValid : 1;
         mutable bool mECIAccelerationIsValid : 1;

         mutable bool mTODLocationIsValid : 1;
         mutable bool mTODVelocityIsValid : 1;
         mutable bool mTODAccelerationIsValid : 1;

         mutable bool mPEFLocationIsValid : 1;
      };
      mutable unsigned int mValidityFlags = 0;
   };

   mutable bool mECI_TransformsValid{false};
   mutable bool mN_TransformValid{false};

   void UpdateECI_Location() const;
   void UpdateECI_Velocity() const;
   void UpdateECI_Acceleration() const;

   void UpdateWCS_Location() const;
   void UpdateWCS_Velocity() const;
   void UpdateWCS_Acceleration() const;

   void UpdateTOD_Location() const;
   void UpdateTOD_Velocity() const;
   void UpdateTOD_Acceleration() const;

   void UpdatePEF_Location() const;

   void UpdateTransforms() const;

   void ComputeSiderealMotion() const;
   void ComputeSiderealMotionTEME() const;

   UtVec3d GetCoriolisAcceleration() const;
   UtVec3d GetCentripetalAcceleration() const;
   UtVec3d GetOmegaCrossR() const;

   static const UtMat3d mFrameTie;
   static const UtMat3d mFrameTieTranspose;

   ut::CloneablePtr<ut::CentralPoint> mCentralPointPtr;

   bool           mUsingPolarMotion{false};
   double         mTimeSinceEpoch{0.0};
   double         mNutationUpdateInterval{1000.0}; // seconds
   mutable double mNutationLastUpdateTime{-mNutationUpdateInterval};

   UtCalendar      mInitialEpoch; // The epoch that is set in the constructor or in SetEpoch
   UtCalendar      mEpoch;        // The epoch that is used internally and advanced with calls to SetTime.
   mutable UtMat3d mPolarMotion;
   mutable UtMat3d mFPN; // Frame Tie * Precession * nutation
   mutable UtMat3d mN;   // Nutation
   mutable UtMat3d mS;   // Sidereal Motion
   mutable UtMat3d mWCS_ToECI_Transform;
   mutable UtMat3d mTEME_ToPEF_Transform;

   mutable UtMat3d mPolarMotionTranspose;
   mutable UtMat3d mNtPtFt; // Nutation-transpose * Precession-transpose * Frame Tie-transpose
   mutable UtMat3d mNt;     // Nutation-transpose
   mutable UtMat3d mSt;     // Sidereal Motion-transpose
   mutable UtMat3d mECI_ToWCS_Transform;

   mutable UtVec3d mOmegaPEF; // Central body rotation vector.
   mutable UtVec3d mLocationPEF;

   mutable UtVec3d mLocationWCS;
   mutable UtVec3d mVelocityWCS;
   mutable UtVec3d mAccelerationWCS;

   mutable UtVec3d mLocationECI;
   mutable UtVec3d mVelocityECI;
   mutable UtVec3d mAccelerationECI;

   mutable UtVec3d mLocationTOD;
   mutable UtVec3d mVelocityTOD;
   mutable UtVec3d mAccelerationTOD;
};

#endif
