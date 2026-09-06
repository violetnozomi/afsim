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

#include "UtScriptVec3.hpp"

#include <sstream>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"

UtScriptVec3* UtScriptVec3::sClassPtr = nullptr;

UtScriptVec3::UtScriptVec3(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(UtStringIdLiteral("Vec3"), aScriptTypesPtr)
{
   sClassPtr      = this;
   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_WRAP_CLASS(UtVec3d);

   // Script Methods
   // Note: Some script methods allow UtScriptData as parameters.
   //       These methods work for either Vec3 types or arrays of doubles/ints.

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, Construct, (double x, double y, double z))
   {
      return ut::make_unique<UtVec3d>(x, y, z);
   };

   UT_SCRIPT_STATIC_METHOD(UtVec3d, Add, (const UtScriptData& objA, const UtScriptData& objB))
   {
      UtVec3d a = UtScriptVec3::ConvertToVec3(objA);
      UtVec3d b = UtScriptVec3::ConvertToVec3(objB);
      return a + b;
   };

   UT_SCRIPT_STATIC_METHOD(UtVec3d, Subtract, (const UtScriptData& objA, const UtScriptData& objB))
   {
      UtVec3d a = UtScriptVec3::ConvertToVec3(objA);
      UtVec3d b = UtScriptVec3::ConvertToVec3(objB);
      return a - b;
   };

   UT_SCRIPT_STATIC_METHOD(double, Dot, (const UtScriptData& objA, const UtScriptData& objB))
   {
      UtVec3d a = UtScriptVec3::ConvertToVec3(objA);
      UtVec3d b = UtScriptVec3::ConvertToVec3(objB);
      return a.DotProduct(b);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtVec3d>, Cross, (const UtScriptData& objA, const UtScriptData& objB))
   {
      UtVec3d a      = UtScriptVec3::ConvertToVec3(objA);
      UtVec3d b      = UtScriptVec3::ConvertToVec3(objB);
      auto    result = ut::make_unique<UtVec3d>();
      result->CrossProduct(a, b);
      return result;
   };

   UT_SCRIPT_STATIC_METHOD(double, AngleWith, (const UtScriptData& objA, const UtScriptData& objB))
   {
      UtVec3d a = UtScriptVec3::ConvertToVec3(objA);
      UtVec3d b = UtScriptVec3::ConvertToVec3(objB);
      return a.AngleWith(b) * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_METHOD(void, Scale, (double aScalar)) { self *= aScalar; };
   UT_SCRIPT_METHOD(void, Negate, ()) { self *= -1.0; };
   UT_SCRIPT_METHOD(double, Magnitude, ()) { return self.Magnitude(); };
   UT_SCRIPT_METHOD(double, MagnitudeSquared, ()) { return self.MagnitudeSquared(); };
   UT_SCRIPT_METHOD(double, Normalize, ()) { return self.Normalize(); };
   UT_SCRIPT_METHOD(std::unique_ptr<UtVec3d>, Normal, ())
   {
      auto normal = ut::make_unique<UtVec3d>(self);
      normal->Normalize();
      return normal;
   };

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

   // Converts a Vec3 to the "Extended" Vec3X object.
   UT_SCRIPT_METHOD(std::unique_ptr<UtVec3dX>, Extend, ()) { return ut::make_unique<UtVec3dX>(self.GetData()); };
}

//! Creates a new script reference of a new UtVec object
UtScriptRef* UtScriptVec3::Create(const UtVec3d& aVec)
{
   return new UtScriptRef(new UtVec3d(aVec), sClassPtr, UtScriptRef::cMANAGE);
}

void* UtScriptVec3::Create(const UtScriptContext& aContext)
{
   return new UtVec3d();
}

void UtScriptVec3::Destroy(void* aObject)
{
   delete static_cast<UtVec3d*>(aObject);
}

void* UtScriptVec3::Clone(void* aObject)
{
   return new UtVec3d(*static_cast<UtVec3d*>(aObject));
}

//! Return the string representation of a Vec3
std::string UtScriptVec3::ToString(void* aObjectPtr) const
{
   std::string rv;
   if (aObjectPtr != nullptr)
   {
      auto              v = static_cast<UtVec3d*>(aObjectPtr);
      std::stringstream ss;
      ss << *v;
      rv = ss.str();
   }
   return rv;
}

//! Create a vector given a script variable.
//! The script variable should either be a Vec3 or an array<> containing at least 3 numbers.
UtVec3d UtScriptVec3::ConvertToVec3(const UtScriptData& aData)
{
   UtVec3d rv;
   if (aData.GetType() == ut::script::Data::Type::cPOINTER)
   {
      UtScriptRef* refPtr       = aData.GetPointer();
      void*        appObjectPtr = refPtr->GetAppObject();
      if (appObjectPtr != nullptr)
      {
         UtScriptClass* classPtr = refPtr->GetScriptClass();
         if (classPtr->GetClassName() == UtStringIdLiteral("Vec3"))
         {
            rv = *refPtr->GetAppObject<UtVec3d>();
         }
         else if (classPtr->IsContainer() && dynamic_cast<UtScriptArray*>(classPtr) != nullptr)
         {
            auto* arrayPtr = static_cast<std::vector<UtScriptData>*>(appObjectPtr);
            if (arrayPtr != nullptr && arrayPtr->size() >= 3)
            {
               for (size_t i = 0; i < 3; ++i)
               {
                  rv[i] = (*arrayPtr)[i].GetDouble();
               }
            }
         }
      }
   }
   else if (aData.GetType() == ut::script::Data::Type::cDOUBLE || aData.GetType() == ut::script::Data::Type::cINT)
   {
      // Try our best if given a number
      double val = aData.GetDouble();
      rv.Set(val, val, val);
   }
   return rv;
}
