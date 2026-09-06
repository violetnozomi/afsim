// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTGUIDEDMOVERCLASS_HPP
#define WSFSCRIPTGUIDEDMOVERCLASS_HPP

#include "wsf_mil_export.h"

#include "script/WsfScriptMoverClass.hpp"

class UtScriptTypes;

//! The script interface 'class' for WsfGuidedMover.
class WSF_MIL_EXPORT WsfScriptGuidedMoverClass : public WsfScriptMoverClass
{
public:
   WsfScriptGuidedMoverClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   UT_DECLARE_SCRIPT_METHOD(DragForce);
   UT_DECLARE_SCRIPT_METHOD(LateralAerodynamicForce);
   UT_DECLARE_SCRIPT_METHOD(MaximumLateralAerodynamicForce);
   UT_DECLARE_SCRIPT_METHOD(AxialThrustForce);
   UT_DECLARE_SCRIPT_METHOD(LateralThrustForce);
   UT_DECLARE_SCRIPT_METHOD(TotalThrustForce);
   UT_DECLARE_SCRIPT_METHOD(DynamicPressure);
   UT_DECLARE_SCRIPT_METHOD(GravitationalAcceleration);
   UT_DECLARE_SCRIPT_METHOD(GravitationalForce);
   UT_DECLARE_SCRIPT_METHOD(EmptyMass_1);
   UT_DECLARE_SCRIPT_METHOD(EmptyMass_2);
   UT_DECLARE_SCRIPT_METHOD(FuelMass_1);
   UT_DECLARE_SCRIPT_METHOD(FuelMass_2);
   UT_DECLARE_SCRIPT_METHOD(PayloadMass_1);
   UT_DECLARE_SCRIPT_METHOD(PayloadMass_2);
   UT_DECLARE_SCRIPT_METHOD(TotalMass_1);
   UT_DECLARE_SCRIPT_METHOD(TotalMass_2);
   UT_DECLARE_SCRIPT_METHOD(CurrentFuelMass);
   UT_DECLARE_SCRIPT_METHOD(CurrentTotalMass);
   UT_DECLARE_SCRIPT_METHOD(CurrentFuelFlowRate);

   // TESTING SUPPORT

   // The following functions are for Testing and should not be documented,
   // to hide them from normal users
   UT_DECLARE_SCRIPT_METHOD(GetTestSupportObject);
};
#endif
