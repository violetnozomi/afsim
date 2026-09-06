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

#include "WsfScriptRectangularFieldOfViewClass.hpp"

#include "UtMath.hpp"
#include "WsfRectangularFieldOfView.hpp"

// =================================================================================================
WsfScriptRectangularFieldOfViewClass::WsfScriptRectangularFieldOfViewClass(const std::string& aClassName,
                                                                           UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFieldOfViewClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfRectangularFieldOfView");

   mConstructible      = false; // Prefer to use the Construct script method.
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<AzimuthFieldOfView>());
   AddMethod(ut::make_unique<ElevationFieldOfView>());
}

// =================================================================================================
bool WsfScriptRectangularFieldOfViewClass::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<WsfRectangularFieldOfView*>(aLHS) == *static_cast<WsfRectangularFieldOfView*>(aRHS);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRectangularFieldOfViewClass,
                        WsfRectangularFieldOfView,
                        Construct,
                        4,
                        "WsfRectangularFieldOfView",
                        "double, double, double, double")
{
   auto fieldOfViewPtr = ut::make_unique<WsfRectangularFieldOfView>();

   // Set the azimuth field of view.
   double minAz = UtMath::NormalizeAngleMinusPi_Pi(aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG);
   double maxAz = UtMath::NormalizeAngleMinusPi_Pi(aVarArgs[1].GetDouble() * UtMath::cRAD_PER_DEG);
   if (minAz > maxAz)
   {
      std::swap(minAz, maxAz);
   }
   fieldOfViewPtr->SetAzimuthFieldOfView(minAz, maxAz);

   // Set the elevation field of view.
   double minEl = aVarArgs[2].GetDouble() * UtMath::cRAD_PER_DEG;
   double maxEl = aVarArgs[3].GetDouble() * UtMath::cRAD_PER_DEG;
   if (minEl > maxEl)
   {
      UT_SCRIPT_ABORT("The maximum elevation angle should be greater than the minimum elevation angle.");
   }
   if (minEl < -UtMath::cPI_OVER_2)
   {
      UT_SCRIPT_ABORT("The minimum elevation angle must be greater than or equal to -90 degrees.");
   }
   if (maxEl > UtMath::cPI_OVER_2)
   {
      UT_SCRIPT_ABORT("The maximum elevation angle must be less than or equal to 90 degrees");
   }
   fieldOfViewPtr->SetElevationFieldOfView(minEl, maxEl);

   aReturnVal.SetPointer(new UtScriptRef(fieldOfViewPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRectangularFieldOfViewClass, WsfRectangularFieldOfView, AzimuthFieldOfView, 0, "Array<double>", "")
{
   double min, max;
   aObjectPtr->GetAzimuthFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptRectangularFieldOfViewClass,
                        WsfRectangularFieldOfView,
                        ElevationFieldOfView,
                        0,
                        "Array<double>",
                        "")
{
   double min, max;
   aObjectPtr->GetElevationFieldOfView(min, max);
   auto data = ut::make_unique<std::vector<UtScriptData>>();
   data->reserve(2);
   data->emplace_back(min * UtMath::cDEG_PER_RAD);
   data->emplace_back(max * UtMath::cDEG_PER_RAD);
   auto scriptRef = ut::make_unique<UtScriptRef>(data.release(), aReturnClassPtr, UtScriptRef::cMANAGE);
   aReturnVal.SetPointer(scriptRef.release());
}
