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

#ifndef WSFEM_RF_TYPE1ATTENUATION_HPP
#define WSFEM_RF_TYPE1ATTENUATION_HPP

#include <string>

#include "WsfEM_Attenuation.hpp"

//! An RF attenuation model that can include the effects of dust, rain and clouds.
//!
//! This model utilizes frequency-dependent lookup-tables and formulas to compute
//! the attenuation of the atmosphere, dust, rain and clouds on RF signals in the
//! 1 - 90 GHZ frequency range.
//!
//! @note This model was provided for a specific project application. It is assumed that
//! the sensor is on an aircraft at a reasonable high altitude looking down at ground targets.

class WsfEM_RF_Type1Attenuation : public WsfEM_Attenuation
{
public:
   WsfEM_RF_Type1Attenuation();
   WsfEM_RF_Type1Attenuation(const WsfEM_RF_Type1Attenuation& aSrc);
   ~WsfEM_RF_Type1Attenuation() override = default;
   WsfEM_RF_Type1Attenuation& operator=(const WsfEM_RF_Type1Attenuation&) = delete;

   static WsfEM_Attenuation* ObjectFactory(const std::string& aTypeName);

   WsfEM_Attenuation* Clone() const override;

   bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr) override;

protected:
   double ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency) override;

private:
   double ComputeAltitude(double aRange, double aElevation);

   double ComputeAtmosphericAttenuation(double aRange, double aElevation, double aFrequency);

   double ComputeCloudAttenuation(double aRange, double aElevation, double aFrequency);

   double ComputeDustAttenuation(double aRange, double aElevation, double aFrequency);

   double ComputeRainAttenuation(double aRange, double aElevation, double aFrequency);


   const WsfScenario* mScenarioPtr;
};

#endif
