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

#ifndef AERO_LANDING_GEAR_HPP
#define AERO_LANDING_GEAR_HPP

#include "AeroObject.hpp"
#include "UtMath.hpp"

#include <string>

// ****************************************************************************
// AeroLandingGear is derived from AeroObject and provides aerodynamics
// for landing gear.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroLandingGear : public AeroObject
   {
      public:
         AeroLandingGear(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroLandingGear() = default;

         // This is used to set a normalized control value for any movement that
         // may be supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         void SetNormalizedControlValue(double aControlValue) override;

         // This is used to set a control angle for any movement that may be
         // supported by the object. It should be called before any calls
         // to SetVehicleAlphaBeta. This may be overridden by derived classes.
         void SetControlAngle_deg(double aControlAngle_deg) override;

         // This is used to get the control angle for the landing gear
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

         // void ClearControlInputs() override;
         // void SetControlInput(double aAngle_deg) override;
         // double GetControlInputAngle_deg() override;

         double GetMaxAngle_deg() { return mMaxAngle_deg; }
         double GetCurrentAngle_deg() { return mCurrentAngle_deg; }
         double GetCdMultiplier() { return mCdMultiplier; }
         double GetCrossSectionalArea_sqft() { return mCrossSectionalArea_sqft; }

         double GetUncompressedLength_ft() { return mUncompressedLength_ft; }
         double GetStrutDiam_ft() { return mStrutDiam_ft; }
         double GetTireDiam_ft() { return mTireDiam_ft; }
         double GetTireWidth_ft() { return mTireWidth_ft; }

         void SetCurrentAngle_deg(double aAngle_deg) { mCurrentAngle_deg = aAngle_deg; }

         void SetData(const std::string& aName,
                      UtVec3dX           aRefPoint_ft,
                      double             aUncompressedLength_ft,
                      double             aStrutDiam_ft,
                      double             aTireDiam_ft,
                      double             aTireWidth_ft,
                      double             aMaxAngle_deg,
                      double             aCurrentAngle_deg,
                      double             aCdMultiplier,
                      double             aSurfaceArea_ft2,
                      double             aMaxRetractionRate_dps,
                      double             aMaxExtensionRate_dps);

         double GetMaxRetractionRate_dps() { return mMaxRetractionRate_dps; }
         double GetMaxExtensionRate_dps() { return mMaxExtensionRate_dps; }

         // Landing gear are always movable
         bool IsMovable() const override { return true; }

      protected:

         // Note:  It is assumed that landing gear will only produce drag, and will not
         // create an appreciable lift force, side force or moment

         void CalcDragVector(double    aAlpha_rad,
                             double    aBeta_rad,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aDragVector_lbs);

         double CalcDragCoefficient(double aAlpha_deg, double aBeta_deg, double aMach);

         void CalcGeometryDCM() override;

         // Returns true if landing gear are retracted (mCurrentAngle_deg = 0)
         bool GearIsRetracted();

         double    mUncompressedLength_ft   = 0.0;
         double    mStrutDiam_ft            = 0.0;
         double    mTireDiam_ft             = 0.0;
         double    mTireWidth_ft            = 0.0;
         double    mCrossSectionalArea_sqft = 0.0;
         double    mMaxAngle_deg            = 90.0;
         double    mCurrentAngle_deg        = 0.0;
         double    mCdMultiplier            = 1.0;
         double    mMaxRetractionRate_dps   = -300.00;
         double    mMaxExtensionRate_dps    = 300.00;
         UtVec3dX  mExtendedAeroCenter;
   };
}
#endif // ! AERO_LANDING_GEAR_HPP
