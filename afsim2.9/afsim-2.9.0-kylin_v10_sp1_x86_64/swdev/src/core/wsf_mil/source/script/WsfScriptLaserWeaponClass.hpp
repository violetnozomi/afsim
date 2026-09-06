// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTLASERWEAPONCLASS_HPP
#define WSFSCRIPTLASERWEAPONCLASS_HPP

#include "wsf_mil_export.h"

#include "WsfScriptDirectedEnergyWeaponClass.hpp"

//! Define script methods for WsfLaserWeapon.
class WSF_MIL_EXPORT WsfScriptLaserWeaponClass : public WsfScriptDirectedEnergyWeaponClass
{
public:
   WsfScriptLaserWeaponClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(Fire_2);
   UT_DECLARE_SCRIPT_METHOD(CanLase);
   UT_DECLARE_SCRIPT_METHOD(TargetOccluded);
   UT_DECLARE_SCRIPT_METHOD(TestLasing1);
   UT_DECLARE_SCRIPT_METHOD(TestLasing2);
   UT_DECLARE_SCRIPT_METHOD(TestLasing3);
   UT_DECLARE_SCRIPT_METHOD(DurationEnergy_1);
   UT_DECLARE_SCRIPT_METHOD(DurationFluence_1);
   UT_DECLARE_SCRIPT_METHOD(DurationEnergy_2);
   UT_DECLARE_SCRIPT_METHOD(DurationFluence_2);
   UT_DECLARE_SCRIPT_METHOD(Energy);
   UT_DECLARE_SCRIPT_METHOD(EdgeRadius);
   UT_DECLARE_SCRIPT_METHOD(EnergyDensity);
   UT_DECLARE_SCRIPT_METHOD(PeakFluence);
   UT_DECLARE_SCRIPT_METHOD(Power);
   UT_DECLARE_SCRIPT_METHOD(SpotSize);
   UT_DECLARE_SCRIPT_METHOD(IncidenceAngle);
   UT_DECLARE_SCRIPT_METHOD(PeakIrradiance);
   UT_DECLARE_SCRIPT_METHOD(EdgeIrradiance);
   UT_DECLARE_SCRIPT_METHOD(AverageIrradiance);
   UT_DECLARE_SCRIPT_METHOD(SpotRadius);
   UT_DECLARE_SCRIPT_METHOD(WindowStrehl);
   UT_DECLARE_SCRIPT_METHOD(SetWindowStrehl);
   UT_DECLARE_SCRIPT_METHOD(AeroStrehl);
   UT_DECLARE_SCRIPT_METHOD(SetAeroStrehl);
   UT_DECLARE_SCRIPT_METHOD(SystemStrehl);
   UT_DECLARE_SCRIPT_METHOD(SystemTransmission);
};

#endif
