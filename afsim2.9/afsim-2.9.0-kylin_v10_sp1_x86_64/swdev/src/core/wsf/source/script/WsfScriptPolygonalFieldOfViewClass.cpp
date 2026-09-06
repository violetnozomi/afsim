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

#include "WsfScriptPolygonalFieldOfViewClass.hpp"

#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "WsfPolygonalFieldOfView.hpp"

// =================================================================================================
WsfScriptPolygonalFieldOfViewClass::WsfScriptPolygonalFieldOfViewClass(const std::string& aClassName,
                                                                       UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFieldOfViewClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfPolygonalFieldOfView");

   mConstructible      = false; // Prefer to use the Construct script method.
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<AzElPoints>());
}

// =================================================================================================
bool WsfScriptPolygonalFieldOfViewClass::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<WsfPolygonalFieldOfView*>(aLHS) == *static_cast<WsfPolygonalFieldOfView*>(aRHS);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptPolygonalFieldOfViewClass,
                        WsfPolygonalFieldOfView,
                        Construct,
                        1,
                        "WsfPolygonalFieldOfView",
                        "Array<Array<double>>")
{
   const auto inputRows = aVarArgs[0].GetPointer()->GetAppObject<std::vector<UtScriptData>>();
   auto       numRows   = inputRows->size();
   if (numRows < 3)
   {
      UT_SCRIPT_ABORT("At least three (az, el) pairs are required to define a polygonal field of view.");
   }
   auto fieldOfViewPtr = ut::make_unique<WsfPolygonalFieldOfView>();

   std::ostringstream oss;
   // Insert the (az, el) points into the stream object.
   for (const auto& row : *inputRows)
   {
      auto* rowValues = row.GetPointer()->GetAppObject<std::vector<UtScriptData>>();
      if (rowValues->size() != 2)
      {
         UT_SCRIPT_ABORT("Each point in the polygonal field of view must contain an (az, el) pair.");
      }
      double az = rowValues->at(0).GetDouble();
      double el = rowValues->at(1).GetDouble();

      oss << "azimuth_elevation " << az << " deg " << el << " deg ";
   }

   UtInput input;
   input.PushInputString(oss.str());
   // Set all (az, el) points in the FOV object.
   std::string cmd;
   while (input.TryReadCommand(cmd))
   {
      fieldOfViewPtr->ProcessInput(input);
   }

   aReturnVal.SetPointer(new UtScriptRef(fieldOfViewPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptPolygonalFieldOfViewClass, WsfPolygonalFieldOfView, AzElPoints, 0, "Array<Array<double>>", "")
{
   const auto& azElPoints = aObjectPtr->GetAzElPoints();
   auto        array2dPtr = ut::make_unique<std::vector<UtScriptData>>();
   for (const auto& point : azElPoints)
   {
      auto arrayPtr = ut::make_unique<std::vector<UtScriptData>>();
      arrayPtr->emplace_back(point.first * UtMath::cDEG_PER_RAD);
      arrayPtr->emplace_back(point.second * UtMath::cDEG_PER_RAD);
      array2dPtr->emplace_back(
         new UtScriptRef(arrayPtr.release(), aContext.GetTypes()->GetClass("Array<double>"), UtScriptRef::cMANAGE));
   }
   aReturnVal.SetPointer(new UtScriptRef(array2dPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}
