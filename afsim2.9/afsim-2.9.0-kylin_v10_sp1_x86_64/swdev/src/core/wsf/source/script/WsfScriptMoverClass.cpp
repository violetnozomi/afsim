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

#include "script/WsfScriptMoverClass.hpp"

#include "UtMemory.hpp"
#include "UtScriptRef.hpp"
#include "WsfMover.hpp"
#include "WsfMoverObserver.hpp"
#include "WsfMoverTypes.hpp"
#include "WsfPlatform.hpp"
#include "WsfRoute.hpp"
#include "WsfSimulation.hpp"
#include "WsfTSPI_Mover.hpp"
#include "script/WsfScriptContext.hpp"

WsfScriptMoverClass::WsfScriptMoverClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : WsfScriptPlatformPartClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfMover");

   AddStaticMethod(ut::make_unique<IsA_TypeOf>());

   AddStaticMethod(ut::make_unique<Create>());

   AddMethod(ut::make_unique<UpdateInterval>());
   AddMethod(ut::make_unique<Route>());
   AddMethod(ut::make_unique<DefaultRoute>());
   AddMethod(ut::make_unique<IsExtrapolating>());
   AddMethod(ut::make_unique<SetMode>());
   AddMethod(ut::make_unique<PropertyInt>());
   AddMethod(ut::make_unique<PropertyDouble>());
   AddMethod(ut::make_unique<PropertyString>());
   AddMethod(ut::make_unique<SetTSPI_FileName>());
   AddMethod(ut::make_unique<BurnedOut>());
   AddMethod(ut::make_unique<TurnOn>());
   AddMethod(ut::make_unique<TurnOff>());

   AddMethod(ut::make_unique<MaximumRadialAcceleration>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TurnRadius_1>("TurnRadius"));  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TurnRadius_2>("TurnRadius"));  // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<MaximumTurnRate>());           // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<CornerVelocity>());            // NO_DOC | DEPRECATED
}

#include "script/WsfScriptDefs.hpp"

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfScriptMoverClass, IsA_TypeOf, 2, "bool", "string, string")
{
   bool      isA_TypeOf = false;
   WsfMover* objectPtr  = WsfMoverTypes::Get(*SCENARIO).Find(aVarArgs[0].GetString());
   if (objectPtr != nullptr)
   {
      isA_TypeOf = objectPtr->IsA_TypeOf(aVarArgs[1].GetString());
   }
   aReturnVal.SetBool(isA_TypeOf);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, Create, 1, "WsfMover", "string")
{
   WsfMover* moverPtr = WsfMoverTypes::Get(*SCENARIO).Clone(aVarArgs[0].GetString());
   aReturnVal.SetPointer(new UtScriptRef(moverPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, UpdateInterval, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetUpdateInterval());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, Route, 0, "WsfRoute", "")
{
   auto route = aObjectPtr->GetRoute();
   if (route)
   {
      auto clonedRoute = ut::clone(route);
      aReturnVal.SetPointer(new UtScriptRef(clonedRoute.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, DefaultRoute, 0, "WsfRoute", "")
{
   auto route = aObjectPtr->GetDefaultRoute();
   if (route)
   {
      auto clonedRoute = ut::clone(route);
      aReturnVal.SetPointer(new UtScriptRef(clonedRoute.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
   }
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, IsExtrapolating, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->IsExtrapolating());
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, SetMode, 1, "void", "string")
{
   aObjectPtr->SetMode(aVarArgs[0].GetString());
}

// Array<int> intProperty = PropertyInt(string);
//
// this method returns an array to serve as a validity check
// if the array is empty, the given property does not exist
// otherwise, the first element in the array will contain the resulting value
UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, PropertyInt, 1, "Array<int>", "string")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   std::string propertyName  = aVarArgs[0].GetString();
   int         propertyValue = 0;
   if (aObjectPtr->GetPropertyInt(propertyName, propertyValue))
   {
      resultVecPtr->push_back(UtScriptData(propertyValue));
   }
   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Array<double> intProperty = PropertyDouble(string);
//
// this method returns an array to serve as a validity check
// if the array is empty, the given property does not exist
// otherwise, the first element in the array will contain the resulting value
UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, PropertyDouble, 1, "Array<double>", "string")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   std::string propertyName  = aVarArgs[0].GetString();
   double      propertyValue = 0.0;
   if (aObjectPtr->GetPropertyDouble(propertyName, propertyValue))
   {
      resultVecPtr->push_back(UtScriptData(propertyValue));
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// Array<string> intProperty = PropertyString(string);
//
// this method returns an array to serve as a validity check
// if the array is empty, the given property does not exist
// otherwise, the first element in the array will contain the resulting value
UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, PropertyString, 1, "Array<string>", "string")
{
   std::vector<UtScriptData>* resultVecPtr = new std::vector<UtScriptData>();

   std::string propertyName  = aVarArgs[0].GetString();
   std::string propertyValue = "";
   if (aObjectPtr->GetPropertyString(propertyName, propertyValue))
   {
      resultVecPtr->push_back(UtScriptData(propertyValue));
   }

   aReturnVal.SetPointer(new UtScriptRef(resultVecPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, SetTSPI_FileName, 1, "bool", "string")
{
   std::string    aFileName    = aVarArgs[0].GetString();
   WsfTSPI_Mover* tspiMoverPtr = dynamic_cast<WsfTSPI_Mover*>(aObjectPtr);
   bool           rv           = false;
   if (tspiMoverPtr != nullptr)
   {
      rv = tspiMoverPtr->SetFileName(aFileName);
   }
   aReturnVal.SetBool(rv);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, BurnedOut, 1, "void", "double")
{
   aObjectPtr->TriggerExtrapolation();
   WsfObserver::MoverBurnedOut(SIMULATION)(aVarArgs[0].GetDouble(), aObjectPtr);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptMoverClass, WsfMover, TurnOff, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOff(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD_NOCHECK(WsfScriptMoverClass, WsfMover, TurnOn, 0, "bool", "")
{
   bool ok = false;
   if (aObjectPtr != nullptr)
   {
      ok = SIMULATION->TurnPartOn(TIME_NOW, aObjectPtr);
   }
   aReturnVal.SetBool(ok);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, MaximumRadialAcceleration, 0, "double", "") // NO_DOC | DEPRECATED
{
   double radialAccel = 0.0;
   aObjectPtr->GetPropertyDouble("maximum_radial_acceleration", radialAccel);
   aReturnVal.SetDouble(radialAccel);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, TurnRadius_1, 0, "double", "") // NO_DOC | DEPRECATED
{
   double turnRadius = 0.0;
   aObjectPtr->GetPropertyDouble("turn_radius", turnRadius);
   aReturnVal.SetDouble(turnRadius);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, TurnRadius_2, 1, "double", "double") // NO_DOC | DEPRECATED
{
   aReturnVal.SetDouble(aObjectPtr->TurnRadius(aVarArgs[0].GetDouble()));
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, MaximumTurnRate, 0, "double", "") // NO_DOC | DEPRECATED
{
   double maxTurnRate = 0.0;
   aObjectPtr->GetPropertyDouble("maximum_turn_rate", maxTurnRate);
   aReturnVal.SetDouble(maxTurnRate);
}

UT_DEFINE_SCRIPT_METHOD(WsfScriptMoverClass, WsfMover, CornerVelocity, 1, "double", "WsfGeoPoint") // NO_DOC | DEPRECATED
{
   double cornerVelocity = 0.0;
   aObjectPtr->GetPropertyDouble("corner_velocity", cornerVelocity);
   aReturnVal.SetDouble(cornerVelocity);
}
