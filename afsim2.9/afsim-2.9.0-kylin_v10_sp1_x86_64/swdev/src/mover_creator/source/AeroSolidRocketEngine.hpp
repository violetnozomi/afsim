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

#ifndef AERO_SOLID_ROCKET_ENGINE_HPP
#define AERO_SOLID_ROCKET_ENGINE_HPP

#include <vector>

#include "AeroEngine.hpp"

namespace Designer
{
   class Vehicle;

   class AeroSolidRocketEngine : public AeroEngine
   {
      public:
         explicit AeroSolidRocketEngine(const std::string& aModelName);
         virtual ~AeroSolidRocketEngine() = default;

         double GetThrust(double aTime, double aAltitude) const;
         double GetFuelFlow(double aTime, double aAltitude) const;

         void LoadFromFile() override;
         void SaveToFile() const override;

         void CalcEngineData() override;
         void GenerateEngineData() override;

         void Export(const std::string& aFileName, ExportMoverType aExportMoverType) const override;

         PropulsionParameters GetPropulsionData(double aMaxMach) const override;

         std::vector<double> GetEngineSpecs() const override;
         void SetEngineSpecs(const std::vector<double>& aEngineSpecs) override;

         std::vector<QString> GetEngineSpecsOptions() const override;

         void SetEngineSpecsOptions(const std::vector<QString>& aEngineSpecsOptions) override;

         double GetPropellantMass_lbs() const { return mPropellantMass_lbs; }

      private:
         double GetSpecificImpulse(double aAltitude_ft) const;
         void CalcThrust();
         void CalcBurnTime();
         void CalcPropellantMass();
         void CalcPropellantMassFraction();

         // Note: not all of these variables are independent of each other.
         // Some are calculated based on the others.
         // Thrust, propellant mass, and burn time are related to each other.
         // any 2 out of 3 are specified by the user, the other is calculated

         double mSpecificImpulseSeaLevel_sec = 0.0; // Technically, units are lbf/(lbm/s)), dependent on mSpecificImpulseVacuum_sec
         double mSpecificImpulseVacuum_sec   = 0.0; // Technically, units are lbf/(lbm/s)), dependent on mSpecificImpulseSeaLevel_sec
         double mRampUpTime_sec              = 0.0; // The time it takes for the thrust to go from zero to the first max point.
         double mRampDownTime_sec            = 0.0; // The time it takes for the thrust to go from the second max point to zero.
         double mRampUpThrust_lbs            = 0.0; // The thrust just after ramp up
         double mRampDownThrust_lbs          = 0.0; // The thrust just before ramp down
         double mBurnTime_sec                = 0.0; // The total burn time, including ramp-up and ramp-down time
         double mPropellantMass_lbs          = 0.0; // Dependent on mPropellantMassFraction
         double mPropellantMassFraction      = 0.0; // Dependent on mPropellantMass_lbs
         double mEmptyMass_lbs               = 0.0; // Mass without propellant
         double mTotalMass_lbs               = 0.0; // Mass including propellant

         QString mSpecifiedVariables;
         QString mMassSpecification;
         QString mAltitudeSpecification;
         QString mPropellantMassSpecification;
   };
}

#endif //AERO_SOLID_ROCKET_ENGINE_HPP
