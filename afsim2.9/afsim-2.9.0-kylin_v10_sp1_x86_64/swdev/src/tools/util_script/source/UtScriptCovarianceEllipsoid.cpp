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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "UtScriptCovarianceEllipsoid.hpp"

#include "UtCovarianceEllipsoid.hpp"
#include "UtMemory.hpp"
#include "UtScriptMethodDefine.hpp"

// static
UtScriptRef* UtScriptEllipsoid::CreateRef(const UtCovarianceEllipsoid& aVec)
{
   return new UtScriptRef(new UtCovarianceEllipsoid(aVec), sClassPtr, UtScriptRef::cMANAGE);
}

// virtual
void UtScriptEllipsoid::Destroy(void* aEllipsoidPtr)
{
   delete static_cast<UtCovarianceEllipsoid*>(aEllipsoidPtr);
}

// virtual
void* UtScriptEllipsoid::Clone(void* aEllipsoidPtr)
{
   return new UtCovarianceEllipsoid(*static_cast<UtCovarianceEllipsoid*>(aEllipsoidPtr));
}

UtScriptEllipsoid* UtScriptEllipsoid::sClassPtr = nullptr;

UtScriptEllipsoid::UtScriptEllipsoid(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Ellipsoid", aScriptTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtCovarianceEllipsoid);

   sClassPtr = this;

   UT_SCRIPT_WRAP_MEMBER("SemiAxisForward", GetSemiAxisForward);
   UT_SCRIPT_WRAP_MEMBER("SemiAxisSide", GetSemiAxisSide);
   UT_SCRIPT_WRAP_MEMBER("SemiAxisUp", GetSemiAxisUp);
   UT_SCRIPT_WRAP_MEMBER("OrientationRoll", GetOrientationRoll);
   UT_SCRIPT_WRAP_MEMBER("OrientationPitch", GetOrientationPitch);
   UT_SCRIPT_WRAP_MEMBER("OrientationHeading", GetOrientationHeading);

   // Deprecated Methods
   UT_SCRIPT_DEPRECATED("Use OrientationRoll()*Math.DEG_PER_RAD() instead")
   UT_SCRIPT_METHOD(double, OrientationRollDeg, ()) // NO_DOC | DEPRECATED
   {
      return self.GetOrientationRoll() * UtMath::cDEG_PER_RAD;
   };
   UT_SCRIPT_DEPRECATED("Use OrientationPitch()*Math.DEG_PER_RAD() instead")
   UT_SCRIPT_METHOD(double, OrientationPitchDeg, ()) // NO_DOC | DEPRECATED
   {
      return self.GetOrientationPitch() * UtMath::cDEG_PER_RAD;
   };
   UT_SCRIPT_DEPRECATED("Use OrientationHeading()*Math.DEG_PER_RAD() instead")
   UT_SCRIPT_METHOD(double, OrientationHeadingDeg, ()) // NO_DOC | DEPRECATED
   {
      return self.GetOrientationHeading() * UtMath::cDEG_PER_RAD;
   };
}
