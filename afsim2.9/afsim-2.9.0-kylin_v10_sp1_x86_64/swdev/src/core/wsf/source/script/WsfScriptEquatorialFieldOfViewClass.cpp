// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptEquatorialFieldOfViewClass.hpp"

#include "UtMath.hpp"
#include "WsfEquatorialFieldOfView.hpp"

// =================================================================================================
WsfScriptEquatorialFieldOfViewClass::WsfScriptEquatorialFieldOfViewClass(const std::string& aClassName,
                                                                         UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFieldOfViewClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfEquatorialFieldOfView");

   mConstructible      = false; // Prefer to use the Construct script method.
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<EquatorialFieldOfView>());
   AddMethod(ut::make_unique<PolarFieldOfView>());
}

// =================================================================================================
bool WsfScriptEquatorialFieldOfViewClass::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<WsfEquatorialFieldOfView*>(aLHS) == *static_cast<WsfEquatorialFieldOfView*>(aRHS);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEquatorialFieldOfViewClass,
                        WsfEquatorialFieldOfView,
                        Construct,
                        4,
                        "WsfEquatorialFieldOfView",
                        "double, double, double, double")
{
   auto fieldOfViewPtr = ut::make_unique<WsfEquatorialFieldOfView>();

   // Set the equatorial field of view.
   double minEq = UtMath::NormalizeAngleMinusPi_Pi(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   double maxEq = UtMath::NormalizeAngleMinusPi_Pi(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   if (minEq > maxEq)
   {
      std::swap(minEq, maxEq);
   }
   fieldOfViewPtr->SetEquatorialFieldOfView(minEq, maxEq);

   // Set the polar field of view.
   double minPolar = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   double maxPolar = aVarArgs[3].GetDouble() * UtMath::cRAD_PER_DEG;
   if (minPolar > maxPolar)
   {
      UT_SCRIPT_ABORT("The maximum polar angle should be greater than the minimum polar angle.");
   }
   if (minPolar < -UtMath::cPI_OVER_2)
   {
      UT_SCRIPT_ABORT("The minimum polar angle must be greater than or equal to -90 degrees.");
   }
   if (maxPolar > UtMath::cPI_OVER_2)
   {
      UT_SCRIPT_ABORT("The maximum polar angle must be less than or equal to 90 degrees");
   }
   fieldOfViewPtr->SetPolarFieldOfView(minPolar, maxPolar);

   aReturnVal.SetPointer(new UtScriptRef(fieldOfViewPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEquatorialFieldOfViewClass, WsfEquatorialFieldOfView, EquatorialFieldOfView, 0, "Array<double>", "")
{
   double min, max;
   aObjectPtr->GetEquatorialFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptEquatorialFieldOfViewClass, WsfEquatorialFieldOfView, PolarFieldOfView, 0, "Array<double>", "")
{
   double min, max;
   aObjectPtr->GetPolarFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}
