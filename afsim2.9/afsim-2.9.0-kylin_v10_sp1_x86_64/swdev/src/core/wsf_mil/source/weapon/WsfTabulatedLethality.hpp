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

#ifndef WSFTABULATEDLETHALITY_HPP
#define WSFTABULATEDLETHALITY_HPP

#include "wsf_mil_export.h"

#include <map>
#include <vector>

#include "WsfExplicitWeaponEffects.hpp"

//! A weapon effect that yields a Pk based upon a table lookup of target type and CEP.
//! This specialization of WsfExplicitWeaponEffects implements a table lookup to determine
//! probability of kill (Pk) after a weapon flyout and detonation.  Independent variables
//! in the table consist of:
//!
//!   - 1.  Target Class (i.e. Mobile, Fixed SR, MR, LR, SAM, Bunker, etc.)
//!   - 2.  For each target class, and as a function of engagement
//!         parameters, Total_CEP = Sum of CEP contributors =
//!                  Intrinsic Weapon CEP + Mensuration Errors, etc.
//!       - A.  This parameter is either specified by the caller,
//!             through the aCEP argument, or
//!       - B.  (If not specified by the caller) this parameter is the true
//!             miss distance as modeled during the fly out.
//!   - 3.  Total_CEP for this particular engagement is interpolated from the
//!         table to yield a Pk value.  A uniform random number will be drawn to
//!         compare to Pk for kill determination.
//!
//!   A high-fidelity tool (such as JMEM) may be used off-line to provide
//!   values for the tables above.
//!
//!   @note
//!   Certain assumptions will be required to reduce the number of independent
//!   variables.  Each class of weapon will encapsulate representative
//!   vulnerable areas, etc.  All weapon deliveries during the simulation will
//!   be assumed capable of achieving their optimum impact angle during descent.

class WSF_MIL_EXPORT WsfTabulatedLethality : public WsfExplicitWeaponEffects
{
public:
   WsfTabulatedLethality(WsfScenario& aScenario);
   ~WsfTabulatedLethality() override = default;
   WsfWeaponEffects* Clone() const override;
   bool              PreInitialize() override;
   bool              Initialize(double aSimTime, const WsfWeaponEngagement* aEngagementPtr) override;

   bool ProcessInput(UtInput& aInput) override;

   class Point
   {
   public:
      Point()
         : mCEP(0.0)
         , mPk(0.0)
      {
      }
      Point(double aCEP, double aPk)
         : mCEP(aCEP)
         , mPk(aPk)
      {
      }

      double CEP() const { return mCEP; }
      double Pk() const { return mPk; }

      bool operator<=(const Point& rhs) const { return mCEP <= rhs.mCEP; } // Required for sort (SGI only).
      bool operator>=(const Point& rhs) const { return mCEP >= rhs.mCEP; } // Required for sort (SGI only).
      bool operator>(const Point& rhs) const { return mCEP > rhs.mCEP; }   // Required for sort (SGI only).
      bool operator<(const Point& rhs) const { return mCEP < rhs.mCEP; }   // Required for sort.

   private:
      double mCEP; // Independent Variable
      double mPk;  // Dependent   Variable
   };

protected:
   void CalcInterceptPk(double aSimTime, WsfPlatform* aTargetPtr) override;

   WsfTabulatedLethality(const WsfTabulatedLethality& aSrc) = default;
   WsfTabulatedLethality& operator=(const WsfTabulatedLethality&) = delete;

private:
   using Cell    = std::vector<Point>;
   using Targets = std::map<WsfStringId, Cell>;

   Targets     mTargets;
   WsfStringId mInputTargetId;
};

#endif
