// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef AERO_RAMJET_ENGINE_HPP
#define AERO_RAMJET_ENGINE_HPP

#include "AeroEngine.hpp"

#include <vector>

namespace Designer
{
   class Vehicle;

   class AeroRamjetEngine : public AeroEngine
   {
      public:
         explicit AeroRamjetEngine(const std::string& aModelName);
         virtual ~AeroRamjetEngine() = default;

         double GetThrust(double aAltitude, double aMach) const;
         double GetFuelFlow(double aAltitude, double aMach) const;

         void LoadFromFile() override;
         void SaveToFile() const override;

         void Export(const std::string& aFileName, ExportMoverType aExportMoverType) const override;

         PropulsionParameters GetPropulsionData(double aMaxMach) const override;

         std::vector<double> GetEngineSpecs() const override;
         void SetEngineSpecs(const std::vector<double>& aEngineSpecs) override;

         void SetData(const std::string&         aName,
                      GeometryEngine::EngineType aEngineType,
                      UtVec3dX                   aRefPoint,
                      double                     aYaw_deg,
                      double                     aPitch_deg,
                      double                     aRoll_deg,
                      double                     aLength_ft,
                      double                     aDiameter_ft,
                      double                     aMass_lbs,
                      double                     aThrustOffset_ft,
                      double                     aIgnitionDelay_sec,
                      bool                       aLatchFuelInjection,
                      bool                       aUseProportionalThrottle,
                      double                     aMinProportionalThrottle);

      private:
         double GetNormalizedThrustFromAltitude(double aAltitude) const;
         double GetNormalizedThrustFromMach(double aMach) const;

         double GetModifiedThrustFromAltitude(double aAltitude) const;
         double GetModifiedThrustFromMach(double aMach) const;

         double mRefArea_sqft      = 0.0;
         double mDesignAltitude_ft = 0.0;
         double mDesignMach        = 0.0;
         double mDesignThrust_lbs  = 0.0;
         double mTsfc_lbs_per_hr   = 0.0;

         bool   mLatchFuelInjection      = false;
         bool   mUseProportionalThrottle = false;
         double mMinProportionalThrottle = false;
   };
}

#endif