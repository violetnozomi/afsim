// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AERO_FULL_FLYING_SURFACE_HPP
#define AERO_FULL_FLYING_SURFACE_HPP

#include "AeroMovableSurface.hpp"

namespace Designer
{
   class Vehicle;

   class AeroFullFlyingSurface : public AeroSurface
   {
      public:
         AeroFullFlyingSurface(const std::string& aName, Vehicle* aVehicle) : AeroSurface(aName, aVehicle) { mAeroTypeString = "AeroFullFlyingSurface"; }

         double GetControlSurfaceMinAngle_deg() const { return mControls.mControlSurfaceMinAngle_deg; }
         double GetControlSurfaceMaxAngle_deg() const { return mControls.mControlSurfaceMaxAngle_deg; }
         double GetControlAngle_deg() const override { return mControls.mCurrentControlSurfaceAngle_deg; }
         double CombinedControlAndIncidenceAngle_rad() const override;
         void   SetControlAngle_deg(double aControlAngle_deg) override;
         bool   IsMovable() const override { return true; }

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

         void SetAdditionalData(const std::string&    aAirfoilFilename,
                                bool                  aUseExponentialAngleMapping,
                                double                aControlSurfaceMinAngle_deg,
                                double                aControlSurfaceMaxAngle_deg,
                                double                aActuatorMinRate_dps,
                                double                aActuatorMaxRate_dps,
                                double                aActuatorMinAngle_deg,
                                double                aActuatorMaxAngle_deg);

      private:
         Controls mControls;
   };

}

#endif