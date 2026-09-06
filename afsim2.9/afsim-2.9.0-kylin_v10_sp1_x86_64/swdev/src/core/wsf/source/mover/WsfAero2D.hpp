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

#ifndef WSFAERO2D_HPP
#define WSFAERO2D_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
#include "WsfAero.hpp"
class WsfPlatform;

//! A simple 2-D Aerodynamics class, supporting platforms
//! whose motion is influenced by aerodynamic lift and drag.
//!
//!   ASSUMPTIONS:
//!   - Two dimensional aerodynamics:
//!     - Lift and Drag, but no moments are developed.
//!   - Idealized parabolic drag polar:
//!     - Cd = Cd0 + (CL * CL / (Pi * AR * e)).
//!     - Cd = Drag Coefficient
//!     - CL = Lift Coefficient, AR = Aspect Ratio, e = Oswald's Efficiency Factor.
//!   - Compressibility (Mach) effects are ignored, unless a 2-D table for
//!     Cd0 versus Mach number is supplied.
//!   - North East Down (NED) roll angle is zero, and side force is developed to place
//!     the lift vector in whatever plane is desired.
//!   - The class does not consider the Angle of Attack (AOA) required to generate the forces.

class WSF_EXPORT WsfAero2D : public WsfAero
{
public:
   WsfAero2D(const UtAtmosphere& aAtmosphere);
   ~WsfAero2D() override;

   WsfAero* Clone() const override;

   bool Initialize(double aSimTime, UtEntity* aPlatformPtr) override;

   void Update(double aSimTime, double aRequestedYForce = 0.0, double aRequestedZForce = 0.0, double aTVC_Force = 0.0) override;

   bool ProcessInput(UtInput& aInput) override;

   double CalculateDrag(double aAltitude_m, double aMach, double aWeight_N) const override;

protected:
   WsfAero2D(const WsfAero2D& aSrc);

   // Prevent use of operator= by declaring, but not defining.
   WsfAero2D& operator=(const WsfAero2D& aRhs) = delete;

   bool SetInternalValues(double aSimTime);

   void CalcCd0();
   void CalcCdTotal();

   struct DragPoint
   {
   public:
      DragPoint(double aMach, double aCd0)
         : mMach(aMach)
         , mCd0(aCd0)
      {
      }
      double mMach;
      double mCd0;
   };

   enum DragSpecType
   {
      cINCOMPRESSIBLE, //!< A single zero-lift drag coefficient (Cd0) value has been supplied.
      //!< Drag is not a function of Mach number, and should not be used for
      //!< a high speed simulation (Mach > 0.7).
      cPARAMETRIC, //!< Drag has been specified as a low-speed Cd0, supersonic Cd0, and this class
      //!< applies a generic transonic drag-rise curve shape between the two points.
      //!< (See separate WSF User Input Guide for more information on this enumeration.)
      cCURVE_POINTS //!< Drag has been specified as a series of explicit (Mach, Cd0) curve points.
   };

   // Characteristic attributes are listed first (intrinsic to the object),
   // and State values (which may vary with time) are listed later:

   double mAR; // Aspect Ratio
   // Oswald's Efficiency Factor
   double mE;
   // Maximum Obtainable Lift Coefficient:
   double mClMax;
   // Run-time constant that aggregates 1.0 / Pi * mAR * mE
   // (Multiplied times CL squared to get delta Cd.)
   double mK;
   double mCdZeroSubsonic;
   double mCdZeroSupersonic;
   double mMachBeginCdRise;
   double mMachEndCdRise;
   double mSubsonicCdSlope;
   double mSupersonicCdSlope;
   double mMachMaxSupersonic;

   // Drag Coefficient at Zero Lift,
   // as a function of Mach Number:
   std::vector<DragPoint> mCd0s;

   double                            mCl;
   double                            mCd0;
   double                            mCdTotal;
   DragSpecType                      mType;
   std::vector<DragPoint>::size_type mIndex;

   double mLateralForce;


private:
   double GetCd0(double aMach) const;

   double GetCdTotal(double aRequiredForce_N, double aReferenceArea_m2, double aAltitude_m, double aMach) const;
};

#endif
