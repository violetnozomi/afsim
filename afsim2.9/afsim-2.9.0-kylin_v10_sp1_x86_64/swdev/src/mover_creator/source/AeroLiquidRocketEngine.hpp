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

#ifndef AERO_LIQUID_ROCKET_ENGINE_HPP
#define AERO_LIQUID_ROCKET_ENGINE_HPP

#include "AeroEngine.hpp"

namespace Designer
{
   class AeroLiquidRocketEngine : public AeroEngine
   {
      public:
         explicit AeroLiquidRocketEngine(const std::string& aModelName);
         virtual ~AeroLiquidRocketEngine() = default;

         double GetThrust(double aAltitude) const;
         double GetFuelFlow(double aAltitude) const;

         void LoadFromFile() override;
         void SaveToFile() const override;

         void Export(const std::string& aFileName, ExportMoverType aExportMoverType) const override;

         PropulsionParameters GetPropulsionData(double aMaxMach) const override;

         std::vector<double> GetEngineSpecs() const override;
         void SetEngineSpecs(const std::vector<double>& aEngineSpecs) override;

      private:
         double GetSpecificImpulse(double aAltitude) const;

         double mRefArea_sqft = 0.0;
         double mSpinUpRate = 0.0; // percent throttle per second
         double mSpinDownRate = 0.0; // percent throttle per second
         double mThrustSeaLevel_lbs = 0.0;
         double mThrustVacuum_lbs = 0.0;
         double mSpecificImpulseSeaLevel_sec = 0.0;
         double mSpecificImpulseVacuum_sec = 0.0;
   };
}


#endif
