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

#include "WsfScriptCircularFieldOfViewClass.hpp"

#include "UtMath.hpp"
#include "WsfCircularFieldOfView.hpp"

// =================================================================================================
WsfScriptCircularFieldOfViewClass::WsfScriptCircularFieldOfViewClass(const std::string& aClassName,
                                                                     UtScriptTypes*     aScriptTypesPtr)
   : WsfScriptFieldOfViewClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCircularFieldOfView");

   mConstructible      = false; // Prefer to use the Construct script method.
   mEqualityComparable = true;

   AddStaticMethod(ut::make_unique<Construct>());
   AddMethod(ut::make_unique<HalfAngle>());
}

// =================================================================================================
bool WsfScriptCircularFieldOfViewClass::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<WsfCircularFieldOfView*>(aLHS) == *static_cast<WsfCircularFieldOfView*>(aRHS);
}

//=================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCircularFieldOfViewClass, WsfCircularFieldOfView, Construct, 1, "WsfCircularFieldOfView", "double")
{
   double halfAngle = aVarArgs[0].GetDouble() * UtMath::cRAD_PER_DEG;
   if (halfAngle < 0.0)
   {
      UT_SCRIPT_ABORT("The half angle must be greater than or equal to zero.");
   }
   if (halfAngle > UtMath::cPI_OVER_2)
   {
      UT_SCRIPT_ABORT("The half angle must be less than or equal to 90 degrees.");
   }
   auto circularFieldOfViewPtr = ut::make_unique<WsfCircularFieldOfView>();
   circularFieldOfViewPtr->SetHalfAngle(halfAngle);
   aReturnVal.SetPointer(new UtScriptRef(circularFieldOfViewPtr.release(), aReturnClassPtr, UtScriptRef::cMANAGE));
}

//=================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptCircularFieldOfViewClass, WsfCircularFieldOfView, HalfAngle, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHalfAngle() * UtMath::cDEG_PER_RAD);
}
