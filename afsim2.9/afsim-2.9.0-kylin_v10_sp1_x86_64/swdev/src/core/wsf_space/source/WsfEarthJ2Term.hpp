// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFEARTHJ2TERM_HPP
#define WSFEARTHJ2TERM_HPP

#include "UtECI_Conversion.hpp"
#include "UtEarth.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfOrbitalDynamicsTerm.hpp"
class UtInput;

class WsfEarthJ2Term : public WsfOrbitalDynamicsTerm
{
public:
   WsfEarthJ2Term();
   WsfEarthJ2Term(const WsfEarthJ2Term& aOther) = default;
   ~WsfEarthJ2Term() override                   = default;

   bool ProcessInput(UtInput& aInput) override;

   WsfEarthJ2Term* Clone() const override { return new WsfEarthJ2Term{*this}; }

   const char* GetScriptClassName() const override { return "WsfEarthJ2Term"; }

   UtVec3d ComputeAcceleration(double            aMass,
                               const UtCalendar& aTime,
                               const UtVec3d&    aPosition,
                               const UtVec3d&    aVelocity) const override;

   std::string GetTermType() const override { return cTYPE; }

   double GetGravitationalParameter() const { return mGravitationalParameter; }
   void   SetGravitationalParameter(double aMu) { mGravitationalParameter = aMu; }

   double GetEarthMeanRadius() const { return mEarthMeanRadius; }
   void   SetEarthMeanRadius(double aMeanRadius) { mEarthMeanRadius = aMeanRadius; }

   double GetJ2() const { return mJ2; }
   void   SetJ2(double aJ2) { mJ2 = aJ2; }

   static constexpr double GetDefaultJ2Value() { return cDEFAULT_J2_VALUE; }

   constexpr static const char* cTYPE{"earth_j2"};

private:
   void RecomputePrefactor();

   // Vallado, Fundamentals of Astrodynamics and Applications (4th Edition), table inside back cover.
   constexpr static double cDEFAULT_J2_VALUE{0.0010826267};

   double mGravitationalParameter{UtEarth::cGRAVITATIONAL_PARAMETER};
   double mEarthMeanRadius{UtSphericalEarth::cEARTH_MEAN_RADIUS};
   double mJ2{cDEFAULT_J2_VALUE};
   double mPrefactor{0.0};
};

#endif // WSFEARTHJ2TERM_HPP
