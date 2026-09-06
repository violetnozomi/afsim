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

#ifndef AERO_SPEED_BRAKE_HPP
#define AERO_SPEED_BRAKE_HPP

#include <string>

#include "AeroObject.hpp"
#include "UtMath.hpp"

// ****************************************************************************
// AeroSpeedBrake is derived from AeroObject and provides aerodynamics
// for speed brakes.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroSpeedBrake : public AeroObject
   {
      public:
         AeroSpeedBrake(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroSpeedBrake() = default;

         // This is used to set a normalized control value for any movement that
         // may be supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         void SetNormalizedControlValue(double aControlValue) override;

         // This is used to set a control angle for any movement that may be
         // supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         void SetControlAngle_deg(double aControlAngle_deg) override;

         // This is used to get the control angle for the speed brake
         double GetControlAngle_deg() const override { return mCurrentAngle_deg; }

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

         // This returns the location where aero forces are acting (as a function of angle)
         UtVec3dX CalcAngleBasedAeroForcePoint_ft(double aMach, double aAngle_deg) const;

         // This returns the surface normal as a function of angle
         UtVec3dX GetSurfaceNormal(double aAngle_deg);

         virtual void CalcAlphaBeta(double& aAlpha_deg, double& aBeta_deg) override;

         // void ClearControlInputs() override;
         // void SetControlInput(double aAngle_deg) override;
         // double GetControlInputAngle_deg() override;

         double GetMaxAngle_deg() { return mMaxAngle_deg; }
         double GetCurrentAngle_deg() { return mCurrentAngle_deg; }
         double GetCdMultiplier() { return mCdMultiplier; }
         double GetCrossSectionalArea_sqft() { return mCrossSectionalArea_sqft; }

         double GetLength_ft() { return mLength_ft; }
         double GetWidth_ft() { return mWidth_ft; }
         double GetRollAngle_deg() { return mRoll_rad * UtMath::cDEG_PER_RAD; }

         void SetCurrentAngle_deg(double aAngle_deg) { mCurrentAngle_deg = aAngle_deg; }

         void SetData(const std::string& aName,
                      UtVec3dX           aRefPoint_ft,
                      double             aLength_ft,
                      double             aWidth_ft,
                      double             aRoll_rad,
                      double             aMaxAngle_deg,
                      double             aCurrentAngle_deg,
                      double             aCdMultiplier,
                      double             aSurfaceArea_ft2,
                      double             aActuatorMinRate_dps,
                      double             aActuatorMaxRate_dps);

         double GetMaxRetractionRate_dps() { return mMaxRetractionRate_dps; }
         double GetMaxExtensionRate_dps() { return mMaxExtensionRate_dps; }

         // Speed brakes are always movable
         bool IsMovable() const override { return true; }

         void CalcAeroCoeffDelta(const std::string& aControlSurfaceName,
                                 double             aControlAngle_deg,
                                 double             aAltitude_ft,
                                 double             aSpeed_fps,
                                 double             aMach,
                                 double             aKTAS,
                                 double             aKCAS,
                                 double             aDynamicPressure_psf,
                                 double             aDensity_slugs_ft3,
                                 double             aVehicleAlpha_deg,
                                 double             aVehicleBeta_deg,
                                 double             aVehicleYawRate_dps,
                                 double             aVehiclePitchRate_dps,
                                 double             aVehicleRollRate_dps,
                                 double&            aDeltaCL,
                                 double&            aDeltaCd,
                                 double&            aDeltaCY,
                                 double&            aDeltaCm,
                                 double&            aDeltaCn,
                                 double&            aDeltaCl) override;

      protected:
         void CalcLiftVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aLiftVector_lbs);

         void CalcSideVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aLiftVector_lbs);

         void CalcDragVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aDragVector_lbs);

         void CalcMomentVector(double    aAlpha_rad,
                               double    aBeta_rad,
                               double    aDynamicPressure_lbssqft,
                               UtVec3dX& aDragVector_lbs);

         double CalcLiftCoefficient(double aAlpha_rad, double aBeta_rad, double aMach);
         double CalcDragCoefficient(double aAlpha_rad, double aBeta_rad, double aMach);
         double CalcZeroLiftPitchingMomentCoefficient(double aAlpha_rad, double aBeta_rad);

         void CalcGeometryDCM() override;

         // Returns true if speed brake is retracted (mCurrentAngle_deg = 0)
         bool SpeedBrakeIsRetracted();

         double  mLength_ft               = 0.0;
         double  mWidth_ft                = 0.0;
         double  mRoll_rad                = 0.0;
         double  mCrossSectionalArea_sqft = 0.0;
         double  mMaxAngle_deg            = 90.0;
         double  mCurrentAngle_deg        = 0.0;
         double  mCdMultiplier            = 1.0;
         double  mMaxRetractionRate_dps   = -300.00;
         double  mMaxExtensionRate_dps    = 300.00;
   };
}
#endif // ! AERO_SPEED_BRAKE_HPP
