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

#ifndef AERO_WING_HPP
#define AERO_WING_HPP

#include <string>

#include "AeroSurface.hpp"
#include "AeroPanel.hpp"
// ****************************************************************************
// AeroWing is derived from AeroSurface and provides aerodynamics for
// the wing, which is a surface object with additional capabilities.
// ****************************************************************************

namespace Designer
{
   class Vehicle;

   class AeroWing : public AeroSurface
   {
      public:
         AeroWing(const std::string& aGeneratedFromName, Vehicle* aVehicle);
         virtual ~AeroWing() = default;

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

         // void ClearControlInputs() override;
         // void SetControlInput(double aAngle_deg) override;
         // double GetControlInputAngle_deg() override;

         double GetAspectRatio() override;
         double GetPlanformArea_ft2() override;

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

         bool       AileronsPresent()    { return mAileronsPresent; }
         bool       SpoileronsPresent()  { return mSpoileronsPresent; }
         bool       SpoilersPresent()    { return mSpoilersPresent; }
         bool       DrageronsPresent()   { return mDrageronsPresent; }
         bool       ElevonsPresent()     { return mElevonsPresent; }

         AeroPanel* GetAilerons()        { return &mAilerons; }
         AeroPanel* GetSpoilerons()      { return &mSpoilerons; }
         AeroPanel* GetSpoilers()        { return &mSpoilers; }
         AeroPanel* GetDrageronsTop()    { return &mDrageronsTop; }
         AeroPanel* GetDrageronsBottom() { return &mDrageronsBottom; }
         AeroPanel* GetElevons()         { return &mElevons;}

         void SetAileronsData(bool               aIsPresent,
                              const std::string& aName,
                              double             aChordFractionStart,
                              double             aChordFractionEnd,
                              double             aSpanFractionStart,
                              double             aSpanFractionEnd,
                              bool               aUseExponentialAngleMapping,
                              double             aControlSurfaceMinAngle_deg,
                              double             aControlSurfaceMaxAngle_deg,
                              double             aActuatorMinAngle_deg,
                              double             aActuatorMaxAngle_deg,
                              double             aActuatorMinRate_dps,
                              double             aActuatorMaxRate_dps);

         void SetDrageronsData(bool               aIsPresent,
                               const std::string& aName,
                               double             aChordFractionStart,
                               double             aChordFractionEnd,
                               double             aSpanFractionStart,
                               double             aSpanFractionEnd,
                               bool               aUseExponentialAngleMapping,
                               double             aControlSurfaceMinAngle_deg,
                               double             aControlSurfaceMaxAngle_deg,
                               double             aActuatorMinAngle_deg,
                               double             aActuatorMaxAngle_deg,
                               double             aActuatorMinRate_dps,
                               double             aActuatorMaxRate_dps);

         void SetSpoileronsData(bool               aIsPresent,
                                const std::string& aName,
                                double             aChordFractionStart,
                                double             aChordFractionEnd,
                                double             aSpanFractionStart,
                                double             aSpanFractionEnd,
                                bool               aUseExponentialAngleMapping,
                                double             aControlSurfaceMinAngle_deg,
                                double             aControlSurfaceMaxAngle_deg,
                                double             aActuatorMinAngle_deg,
                                double             aActuatorMaxAngle_deg,
                                double             aActuatorMinRate_dps,
                                double             aActuatorMaxRate_dps);

         void SetSpoilersData(bool               aIsPresent,
                              const std::string& aName,
                              double             aChordFractionStart,
                              double             aChordFractionEnd,
                              double             aSpanFractionStart,
                              double             aSpanFractionEnd,
                              bool               aUseExponentialAngleMapping,
                              double             aControlSurfaceMinAngle_deg,
                              double             aControlSurfaceMaxAngle_deg,
                              double             aActuatorMinAngle_deg,
                              double             aActuatorMaxAngle_deg,
                              double             aActuatorMinRate_dps,
                              double             aActuatorMaxRate_dps);


         void SetElevonsData(bool               aIsPresent,
                             const std::string& aName,
                             double             aChordFractionStart,
                             double             aChordFractionEnd,
                             double             aSpanFractionStart,
                             double             aSpanFractionEnd,
                             bool               aUseExponentialAngleMapping,
                             double             aControlSurfaceMinAngle_deg,
                             double             aControlSurfaceMaxAngle_deg,
                             double             aActuatorMinAngle_deg,
                             double             aActuatorMaxAngle_deg,
                             double             aActuatorMinRate_dps,
                             double             aActuatorMaxRate_dps);

      protected:

         void CalcLiftVector(double    aAlpha_deg,
                             double    aBeta_deg,
                             double    aEffectiveSweep_deg,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aLiftVector_lbs) override;

         void CalcDragVector(double    aAlpha_deg,
                             double    aBeta_deg,
                             double    aEffectiveSweep_deg,
                             double    aDynamicPressure_lbssqft,
                             double    aMach,
                             UtVec3dX& aDragVector_lbs) override;

         void CalcSideForceVector(double    aAlpha_deg,
                                  double    aBeta_deg,
                                  double    aEffectiveSweep_deg,
                                  double    aDynamicPressure_lbssqft,
                                  double    aMach,
                                  UtVec3dX& aSideForceVector_lbs) override;

         double CalcLiftCoefficient(double aAlpha_deg,
                                    double aBeta_deg,
                                    double aEffectiveSweep_deg,
                                    double aMach) override;

         double CalcDragCoefficient(double aAlpha_deg,
                                    double aBeta_deg,
                                    double aEffectiveSweep_deg,
                                    double aMach) override;

         double CalcSideForceCoefficient(double aAlpha_deg,
                                         double aBeta_deg,
                                         double aEffectiveSweep_deg,
                                         double aMach) override;

         bool      mAileronsPresent   = false;
         bool      mDrageronsPresent  = false;
         bool      mSpoileronsPresent = false;
         bool      mSpoilersPresent   = false;
         bool      mElevonsPresent    = false;

         AeroPanel mAilerons          = AeroPanel(this);                                   
         AeroPanel mSpoilerons        = AeroPanel(this);
         AeroPanel mSpoilers          = AeroPanel(this);
         AeroPanel mDrageronsTop      = AeroPanel(this);
         AeroPanel mDrageronsBottom   = AeroPanel(this);
         AeroPanel mElevons           = AeroPanel(this);
   };
}
#endif // !AERO_WING_HPP
