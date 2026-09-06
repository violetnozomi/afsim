// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptGuidedMoverClass.hpp"

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "WsfGuidedMover.hpp"
#include "WsfPlatform.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptGuidedMoverClass::WsfScriptGuidedMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptMoverClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfGuidedMover");

   AddMethod(ut::make_unique<DragForce>());
   AddMethod(ut::make_unique<LateralAerodynamicForce>());
   AddMethod(ut::make_unique<MaximumLateralAerodynamicForce>());
   AddMethod(ut::make_unique<AxialThrustForce>());
   AddMethod(ut::make_unique<LateralThrustForce>());
   AddMethod(ut::make_unique<TotalThrustForce>());
   AddMethod(ut::make_unique<DynamicPressure>());
   AddMethod(ut::make_unique<GravitationalAcceleration>());
   AddMethod(ut::make_unique<GravitationalForce>());
   AddMethod(ut::make_unique<EmptyMass_1>("EmptyMass"));
   AddMethod(ut::make_unique<EmptyMass_2>("EmptyMass"));
   AddMethod(ut::make_unique<FuelMass_1>("InitialFuelMass"));
   AddMethod(ut::make_unique<FuelMass_2>("InitialFuelMass"));
   AddMethod(ut::make_unique<PayloadMass_1>("PayloadMass"));
   AddMethod(ut::make_unique<PayloadMass_2>("PayloadMass"));
   AddMethod(ut::make_unique<TotalMass_1>("InitialTotalMass"));
   AddMethod(ut::make_unique<TotalMass_2>("InitialTotalMass"));
   AddMethod(ut::make_unique<CurrentFuelMass>());
   AddMethod(ut::make_unique<CurrentTotalMass>());
   AddMethod(ut::make_unique<CurrentFuelFlowRate>());

   // Undocumented test functions
   AddMethod(ut::make_unique<GetTestSupportObject>("__getTestObject"));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, DragForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double forcesECS[3];
   aObjectPtr->GetAeroForcesECS(forcesECS);
   // Note: the drag force in element 0 is always negative. We return the unsigned value.
   aReturnVal.SetDouble(fabs(forcesECS[0]));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, LateralAerodynamicForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double forcesECS[3];
   aObjectPtr->GetAeroForcesECS(forcesECS);
   double force = sqrt(forcesECS[1] * forcesECS[1] + forcesECS[2] * forcesECS[2]);
   aReturnVal.SetDouble(force);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, MaximumLateralAerodynamicForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double force = aObjectPtr->GetMaximumLateralAerodynamicForce();
   aReturnVal.SetDouble(force);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, AxialThrustForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double forcesECS[3];
   aObjectPtr->GetThrustForcesECS(forcesECS);
   aReturnVal.SetDouble(forcesECS[0]);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, LateralThrustForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double forcesECS[3];
   aObjectPtr->GetThrustForcesECS(forcesECS);
   double force = sqrt(forcesECS[1] * forcesECS[1] + forcesECS[2] * forcesECS[2]);
   aReturnVal.SetDouble(force);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, TotalThrustForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double forcesECS[3];
   aObjectPtr->GetThrustForcesECS(forcesECS);
   aReturnVal.SetDouble(UtVec3d::Magnitude(forcesECS));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, DynamicPressure, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double q = aObjectPtr->GetDynamicPressure();
   aReturnVal.SetDouble(q);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, GravitationalAcceleration, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double g = aObjectPtr->GetGravitationalAcceleration();
   aReturnVal.SetDouble(g);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, GravitationalForce, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   double g = aObjectPtr->GetGravitationalAcceleration();
   double m = aObjectPtr->GetPlatform()->GetMass();
   aReturnVal.SetDouble(m * g);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, EmptyMass_1, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetEmptyMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, EmptyMass_2, 1, "double", "int")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   int index     = aVarArgs[0].GetInt();
   int numStages = ut::cast_to_int(aObjectPtr->GetNumStages());
   UT_SCRIPT_ASSERT((index >= 1) && (index <= numStages));
   aReturnVal.SetDouble(aObjectPtr->GetEmptyMass(ut::cast_to_size_t(index - 1)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, FuelMass_1, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetFuelMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, FuelMass_2, 1, "double", "int")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   int index     = aVarArgs[0].GetInt();
   int numStages = ut::cast_to_int(aObjectPtr->GetNumStages());
   UT_SCRIPT_ASSERT((index >= 1) && (index <= numStages));
   aReturnVal.SetDouble(aObjectPtr->GetFuelMass(ut::cast_to_size_t(index - 1)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, PayloadMass_1, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetPayloadMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, PayloadMass_2, 1, "double", "int")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   int index     = aVarArgs[0].GetInt();
   int numStages = ut::cast_to_int(aObjectPtr->GetNumStages());
   UT_SCRIPT_ASSERT((index >= 1) && (index <= numStages));
   aReturnVal.SetDouble(aObjectPtr->GetPayloadMass(ut::cast_to_size_t(index - 1)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, TotalMass_1, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetTotalMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, TotalMass_2, 1, "double", "int")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   int index     = aVarArgs[0].GetInt();
   int numStages = ut::cast_to_int(aObjectPtr->GetNumStages());
   UT_SCRIPT_ASSERT((index >= 1) && (index <= numStages));
   aReturnVal.SetDouble(aObjectPtr->GetTotalMass(ut::cast_to_size_t(index - 1)));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, CurrentFuelMass, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentFuelMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, CurrentTotalMass, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentTotalMass());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, CurrentFuelFlowRate, 0, "double", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetDouble(aObjectPtr->GetCurrentFuelFlowRate());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptGuidedMoverClass, WsfGuidedMover, GetTestSupportObject, 0, "WsfGuidedMoverTestObject", "")
{
   aObjectPtr->GetPlatform()->Update(TIME_NOW);
   aReturnVal.SetPointer(new UtScriptRef(aObjectPtr->GetTestSupportObject(), aReturnClassPtr));
}
