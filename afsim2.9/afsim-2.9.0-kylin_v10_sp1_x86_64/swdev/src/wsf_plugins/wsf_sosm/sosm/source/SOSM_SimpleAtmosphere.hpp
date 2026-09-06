// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SOSM_SIMPLEATMOSPHERE_HPP
#define SOSM_SIMPLEATMOSPHERE_HPP

#include "SOSM_Atmosphere.hpp"
#include "UtAtmosphere.hpp"

class SOSM_SimpleAtmosphere : public SOSM_Atmosphere
{
public:
   SOSM_SimpleAtmosphere(const UtAtmosphere& aAtmosphere);

   SOSM_Atmosphere* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   void ComputeAtmosphereData(const SOSM_Selector& aSelector,
                              SOSM_Interaction&    aInteraction,
                              std::vector<float>&  aBackgroundRadiance,
                              std::vector<float>&  aForegroundRadiance,
                              std::vector<float>&  aForegroundTransmittance) override;

   void GetSamplePoints(std::vector<float>& aAltValues, std::vector<float>& aElValues, std::vector<float>& aRangeValues) override;

   double ComputeSimpleTransmittance(double aAttenuationPerMeter, double aHeightMSL_1, double aHeightMSL_2, double aGroundRange);

   double GetAttenuation() const { return mAtmosphericAttenuation; }

   UtAtmosphere& GetAtmosphere() { return mAtmosphere; }

private:
   //! Fixed atmospheric attenuation (1/m)
   double mAtmosphericAttenuation;

   //! Air density at sea level (kg/m^3)
   double mRhoSeaLevel;

   //! Atmosphere model.
   UtAtmosphere mAtmosphere;
};

#endif
