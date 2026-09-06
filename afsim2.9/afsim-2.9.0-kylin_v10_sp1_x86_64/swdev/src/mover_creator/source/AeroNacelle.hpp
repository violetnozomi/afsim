// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AERO_NACELLE_HPP
#define AERO_NACELLE_HPP

#include <string>

#include "AeroBody.hpp"

// ****************************************************************************
// AeroNacelle is derived from AeroBody and provides aerodynamics for
// engine nacelles.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroNacelle : public AeroBody
   {
      public:
         AeroNacelle(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroNacelle() = default;

         // Calculate F&M using alpha, beta, q, and Mach.
         // SetVehicleAlphaBeta should be called prior to calling this function.
         void CalcForcesAndMoments(double    aDynamicPressure_lbssqft,
                                   double    aMach,
                                   UtVec3dX& aLiftVector_lbs,
                                   UtVec3dX& aDragVector_lbs,
                                   UtVec3dX& aSideVector_lbs,
                                   UtVec3dX& aMoment_ftlbs,
                                   UtVec3dX& aForcePoint_ft) override;

         // This returns the location where aero forces are acting (as a function of Mach)
         UtVec3dX CalcAeroForcePoint_ft(double aMach = 0.0) const override;

         std::string GetOverallShapeType() { return mOverallShapeType; }

         void SetData(const std::string& aName,
                      UtVec3dX           aRefPoint_ft,
                      double             aLength_ft,
                      double             aHeight_ft,
                      double             aHeightInner_ft,
                      double             aHeightOuter_ft,
                      double             aWidth_ft,
                      double             aThickness_ft,
                      double             aYaw_rad,
                      double             aPitch_rad,
                      double             aRoll_rad,
                      const std::string& aOverallShapeType,
                      double             aForwardSweepLength_ft,
                      const std::string& aAftSectionType,
                      double             aAftSectionLength_ft,
                      double             aSurfaceArea_ft2);

      protected:

         // Overall shapes include Rounded, Half-Round-Right, Half-Round-Left, Half-Round-Top,
         // Half-Round-Bottom, Flat-Sided, Flat-Swept-Right, and Flat-Swept-Left
         std::string mOverallShapeType;
         double      mForwardSweepLength_ft = 0.0;

         // Aft section types include Blunt and Tapered
         std::string mAftSectionType;
         double      mAftSectionLength_ft   = 0.0;

         // These are used for flat-sided nacelles that support and inner/outer height
         double      mHeightInner_ft        = 0.0;
         double      mHeightOuter_ft        = 0.0;
         double      mThickness_ft          = 0.0;  // Thickness of the nacelle ring
   };
}
#endif // ! AERO_NACELLE_HPP
