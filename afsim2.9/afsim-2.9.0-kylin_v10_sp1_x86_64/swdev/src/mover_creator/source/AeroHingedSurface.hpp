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

#ifndef AERO_HINGED_SURFACE_HPP
#define AERO_HINGED_SURFACE_HPP

#include "AeroMovableSurface.hpp"
#include "AeroPanel.hpp"

namespace Designer
{
   class Vehicle;

   class AeroHingedSurface : public AeroSurface
   {
      public:
         AeroHingedSurface(const std::string& aName, Vehicle* aVehicle) : AeroSurface(aName, aVehicle) { mAeroTypeString = "AeroHingedSurface"; }

         AeroPanel* GetControlSurface() { return &mAeroPanel; };

         double GetControlSurfaceMinAngle_deg() const { return mAeroPanel.GetControlSurfaceMinAngle_deg(); }
         double GetControlSurfaceMaxAngle_deg() const { return mAeroPanel.GetControlSurfaceMaxAngle_deg(); }
         double GetControlAngle_deg() const override { return mAeroPanel.GetControlAngle_deg(); }
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

      private:
         AeroPanel mAeroPanel = AeroPanel(this);
   };

}

#endif