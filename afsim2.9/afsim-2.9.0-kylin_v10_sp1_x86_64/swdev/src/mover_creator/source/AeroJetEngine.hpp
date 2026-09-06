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

#ifndef AERO_JET_ENGINE_HPP
#define AERO_JET_ENGINE_HPP

#include "AeroEngine.hpp"

#include <vector>

namespace Designer
{
   class Vehicle;

   class AeroJetEngine : public AeroEngine
   {
      public:
         explicit AeroJetEngine(const std::string& aModelName);
         virtual ~AeroJetEngine() = default;

         double GetThrust(double aAltitude, double aMach, int aThrottle) const;
         double GetFuelFlow(double aAltitude, double aMach, int aThrottle) const;

         bool IncludeAfterburner() { return mIncludeAfterburner; }

         void LoadFromFile() override;
         void SaveToFile() const override;

         void Export(const std::string& aFileName, ExportMoverType aExportMoverType) const override;

         PropulsionParameters GetPropulsionData(double aMaxMach) const override;

         std::vector<double> GetEngineSpecs() const override;
         void SetEngineSpecs(const std::vector<double>& aEngineSpecs) override;

      private:
         // Returns the thrust at aAltitude at Mach 0.
         double GetModifiedThrustFromAltitude(double aAltitude, int aThrottle) const;

         // Returns the thrust at aMach at sea level.
         double GetModifiedThrustFromMach(double aMach, int aThrottle) const;

         double mRefArea_sqft       = 0.0;
         double mIdleTsfc           = 0.0;
         double mMilTsfc            = 0.0;
         double mABTsfc             = 0.0;
         bool   mIncludeAfterburner = true;
   };
}

#endif
