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

#ifndef WSFAERO_HPP
#define WSFAERO_HPP

#include "wsf_export.h"

#include "UtAtmosphere.hpp"
class UtInput;
#include "WsfObject.hpp"
class UtEntity;

//! Base class for aerodynamics classes.
//! A utility class for supporting platforms
//! whose motion is influenced by aerodynamic lift, drag, and other forces.

class WSF_EXPORT WsfAero : public WsfObject
{
public:
   WsfAero(const UtAtmosphere& aAtmosphere);
   ~WsfAero() override;

   WsfAero* Clone() const override = 0;

   virtual bool Initialize(double aSimTime, UtEntity* aPlatformPtr);

   virtual void Update(double aSimTime, double aRequestedYForce = 0.0, double aRequestedZForce = 0.0, double aTVC_Force = 0.0);

   bool ProcessInput(UtInput& aInput) override;

   //! Get the aerodynamic drag force.
   //! A positive value opposes the direction of motion.
   double GetDrag() const { return mDrag; }

   //! Get the 3-D resultant aerodynamic forces in the Entity Coordinate System frame.
   virtual void GetECSForces(double aECSForces[3]) const
   {
      aECSForces[0] = -mDrag;
      aECSForces[1] = mYForce;
      aECSForces[2] = mZForce;
   }

   double GetQS() const { return mDynPressure * mRefArea; }

   //! Return the maximum possible applicable force that could have been applied as calculated by the
   //! last call to Update().
   //!
   //! This is the force that is used to limit the requested forces.
   double GetMaximumForce() const { return mMaximumForce; }

   //! Return the partial derivative of lift coefficient with angle of attack.  Assumed constant.
   double LiftCurveSlope() const { return mLiftCurveSlope; }

   virtual double CalculateDrag(double aAltitude_m, double aMach, double aWeight_N) const { return 0.0; }

protected:
   std::string GetPlatformName() const;
   WsfAero(const WsfAero& aSrc);

   // Calculates and returns the dynamic pressure in N/m^2 given altitude in meters and speed in m/s
   double GetDynamicPressure(double aAltitude_m, double aSpeed_mps) const;

   // Calculates and returns the Mach number given the altitude and speed.
   double GetMach(double aAltitude_m, double aSpeed_mps) const;

   // Calculates and returns the speed in m/s given the altitude and Mach number
   double GetSpeed(double aAltitude_m, double aMach) const;

   // Prevent use of operator= by declaring, but not defining.
   WsfAero& operator=(const WsfAero& aRhs) = delete;

   //! Entity to which this Aero applies.
   //! Position, velocity, pitch, roll and yaw rates, etc.
   //! will be obtained from this platform data.
   UtEntity* mEntityPtr;

   UtAtmosphere mAtm;

   // Characteristic attributes are listed first (intrinsic to the object),
   // and State values (which may vary with time) are listed later:

   double mRefArea;

   // -------------------------------------------------------
   // The State values below are refreshed with each call to Update():

   double mAltitude;
   double mDynPressure;
   double mSpeed;
   double mMach;

   double mDrag;
   double mZForce;
   double mYForce;
   double mMaximumForce;
   double mLiftCurveSlope; // dCL/dAlpha (off body X-axis) assumed constant.
   bool   mDebug;
   bool   mInitialized;
};

#endif
