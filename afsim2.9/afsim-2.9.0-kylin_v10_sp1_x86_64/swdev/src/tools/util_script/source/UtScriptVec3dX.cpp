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

#include "UtScriptVec3dX.hpp"

#include <sstream>

#include "UtScriptMethodDefine.hpp"
#include "UtVec3dX.hpp"

UtScriptVec3dX* UtScriptVec3dX::sClassPtr = nullptr;

UtScriptVec3dX::UtScriptVec3dX(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("Vec3X", aScriptTypesPtr)
{
   sClassPtr      = this;
   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_WRAP_CLASS(UtVec3dX);

   UT_SCRIPT_STATIC_METHOD(UtVec3dX, Add, (const UtVec3dX& a, const UtVec3dX& b)) { return a + b; };
   UT_SCRIPT_STATIC_METHOD(UtVec3dX, Subtract, (const UtVec3dX& a, const UtVec3dX& b)) { return a - b; };
   UT_SCRIPT_STATIC_METHOD(double, Dot, (const UtVec3dX& a, const UtVec3dX& b)) { return a.Dot(b); };
   UT_SCRIPT_STATIC_METHOD(UtVec3dX, Cross, (const UtVec3dX& a, const UtVec3dX& b)) { return a.Cross(b); };

   UT_SCRIPT_STATIC_METHOD(UtVec3dX,
                           LineOfSightRates,
                           (const UtVec3dX& aPrevEyeLoc,
                            const UtVec3dX& aCurrEyeLoc,
                            const UtVec3dX& aPrevTgtLoc,
                            const UtVec3dX& aCurrTgtLoc,
                            double          aDeltaT))
   {
      return UtVec3dX::LineOfSightRates(aPrevEyeLoc, aCurrEyeLoc, aPrevTgtLoc, aCurrTgtLoc, aDeltaT);
   };

   UT_SCRIPT_STATIC_METHOD(UtVec3dX,
                           LineOfSightRates,
                           (const UtVec3dX& aEyeLoc, const UtVec3dX& aEyeVel, const UtVec3dX& aTgtLoc, const UtVec3dX& aTgtVel))
   {
      return UtVec3dX::LineOfSightRates(aEyeLoc, aEyeVel, aTgtLoc, aTgtVel);
   };

   // Methods
   UT_SCRIPT_METHOD(double, MagXY, ()) { return self.MagXY(); };
   UT_SCRIPT_METHOD(double, MagXZ, ()) { return self.MagXZ(); };
   UT_SCRIPT_METHOD(double, MagYZ, ()) { return self.MagYZ(); };
   UT_SCRIPT_METHOD(double, Magnitude, ()) { return self.Magnitude(); };
   UT_SCRIPT_METHOD(double, MagnitudeSquared, ()) { return self.MagnitudeSquared(); };
   UT_SCRIPT_METHOD(double, Normalize, ()) { return self.Normalize(); };
   UT_SCRIPT_METHOD(double, X, ()) { return self[0]; };
   UT_SCRIPT_METHOD(double, Y, ()) { return self[1]; };
   UT_SCRIPT_METHOD(double, Z, ()) { return self[2]; };
   UT_SCRIPT_METHOD(void, Set, (double x, double y, double z)) { self.Set(x, y, z); };
   UT_SCRIPT_METHOD(void, SetX, (double val)) { self[0] = val; };
   UT_SCRIPT_METHOD(void, SetY, (double val)) { self[1] = val; };
   UT_SCRIPT_METHOD(void, SetZ, (double val)) { self[2] = val; };
   UT_SCRIPT_METHOD(double, Get, (int index))
   {
      if (index < 0 || index > 2)
      {
         index = 0;
      }
      return self[index];
   };
   UT_SCRIPT_METHOD(UtVec3dX, Multiply, (double aScalar)) { return self * aScalar; };
   UT_SCRIPT_METHOD(UtVec3dX, Divide, (double aScalar)) { return self / aScalar; };
}

//! Creates a new script reference of a new UtVec3dX object
UtScriptRef* UtScriptVec3dX::Create(const UtVec3d& aVec)
{
   return new UtScriptRef(new UtVec3dX(aVec.GetData()), sClassPtr, UtScriptRef::cMANAGE);
}

void* UtScriptVec3dX::Create(const UtScriptContext& aContext)
{
   return new UtVec3dX();
}

void UtScriptVec3dX::Destroy(void* aObject)
{
   delete static_cast<UtVec3dX*>(aObject);
}

void* UtScriptVec3dX::Clone(void* aObject)
{
   return new UtVec3dX(*static_cast<UtVec3dX*>(aObject));
}

//! Return the string representation of a Vec3X
std::string UtScriptVec3dX::ToString(void* aObjectPtr) const
{
   std::string rv;
   if (aObjectPtr != nullptr)
   {
      UtVec3dX&         v = *static_cast<UtVec3dX*>(aObjectPtr);
      std::stringstream ss;
      ss << '(' << v[0] << ", " << v[1] << ", " << v[2] << ')';
      rv = ss.str();
   }
   return rv;
}
