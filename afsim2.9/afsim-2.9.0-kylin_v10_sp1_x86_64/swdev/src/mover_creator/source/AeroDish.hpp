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

#ifndef AERO_DISH_HPP
#define AERO_DISH_HPP

#include <string>

#include "AeroObject.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// AeroDish is derived from AeroObject and provides aerodynamics for
// any body of revolution such as a fuselage or missile body.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroDish : public AeroObject
   {
      public:
         AeroDish(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroDish() = default;

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

         double GetDiameter_ft() { return mDiameter_ft; }
         double GetThickness_ft() { return mThickness_ft; }
         double GetCrossSectionalArea_sqft() { return mCrossSectionalArea_sqft; }

         void SetData(const std::string& aName,
                      UtVec3dX           aRefPoint_ft,
                      double             aDiameter_ft,
                      double             aThickness_ft,
                      double             aSurfaceArea_ft2);

      protected:
         void CalcLiftVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             UtVec3dX& aLiftVector_lbs);

         void CalcDragVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aDragVector_lbs);

         void CalcSideForceVector(double    aAlpha_rad,
                                  double    aBeta_rad,
                                  double    aDynamicPressure_lbssqft,
                                  UtVec3dX& aSideForceVector_lbs);

         void CalcMomentVector(double    aAlpha_rad,
                               double    aBeta_rad,
                               double    aDynamicPressure_lbssqft,
                               UtVec3dX& aMoment_ftlbs);

         double CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad);
         double CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach);
         double CalcSideForceCoefficient(double aAlpha_rad, double aBeta_rad);
         double CalcZeroLiftPitchingMoment(double aAlpha_rad, double aBeta_rad);

         void CalcGeometryDCM() override;

         double  mDiameter_ft                = 0.0;
         double  mThickness_ft               = 0.0;
         double  mYaw_rad                    = 0.0;
         double  mPitch_rad                  = 0.0;
         double  mRoll_rad                   = 0.0;
         double  mCrossSectionalArea_sqft    = 0.0;
         double  mFinenessRatio              = 0.0;
         double  mConeRelativeTotalAngle_deg = 0.0;
   };
}
#endif // ! AERO_DISH_HPP
