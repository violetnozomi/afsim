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

#include "UtScriptQuaternion.hpp"

#include <sstream>

#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtQuaternion.hpp"
#include "UtScriptBasicTypes.hpp"
#include "UtScriptException.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtVec3.hpp"

UtScriptQuaternion* UtScriptQuaternion::sClassPtr = nullptr;

UtScriptQuaternion::UtScriptQuaternion(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(UtStringIdLiteral("Quaternion"), aScriptTypesPtr)
{
   sClassPtr      = this;
   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_WRAP_CLASS(UtQuaternion);

   // Static Methods
   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtQuaternion>, Construct, (double aPsi, double aTheta, double aPhi))
   {
      return ut::make_unique<UtQuaternion>(aPsi * UtMath::cRAD_PER_DEG,
                                           aTheta * UtMath::cRAD_PER_DEG,
                                           aPhi * UtMath::cRAD_PER_DEG);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtQuaternion>, Construct, (double aA, double aI, double aJ, double aK))
   {
      return ut::make_unique<UtQuaternion>(aA, aI, aJ, aK);
   };

   UT_SCRIPT_STATIC_METHOD(UtQuaternion, Multiply, (const UtQuaternion& aQuatLHS, const UtQuaternion& aQuatRHS))
   {
      return aQuatLHS * aQuatRHS;
   };

   UT_SCRIPT_STATIC_METHOD(UtQuaternion, Slerp, (const UtQuaternion& aQuatFrom, const UtQuaternion& aQuatTo, double aFactor))
   {
      float factor = static_cast<float>(aFactor);
      if ((factor < 0.0) || (factor > 1.0))
      {
         auto err = ut::log::error() << "Interpolation coefficient NOT in expected range of [0..1] (" << factor
                                     << " provided).";
         UT_SCRIPT_ABORT("Interpolation coefficient NOT in expected range of [0..1]");
      }

      return UtQuaternion::Slerp(aQuatFrom, aQuatTo, factor);
   };

   // Methods

   UT_SCRIPT_WRAP_MEMBER("Normalize", Normalize);

   UT_SCRIPT_METHOD(UtQuaternion, Rotate, (const UtVec3d& bodyRatesDeg, double deltaT))
   {
      double bodyRatesRad[3] = {bodyRatesDeg[0] * UtMath::cRAD_PER_DEG,
                                bodyRatesDeg[1] * UtMath::cRAD_PER_DEG,
                                bodyRatesDeg[2] * UtMath::cRAD_PER_DEG};
      return self.Rotate(bodyRatesRad, deltaT);
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtVec3d>, Rotate, (const UtVec3d& aVecIn))
   {
      auto rotatedVec = ut::make_unique<UtVec3d>();
      self.Rotate(aVecIn.GetData(), rotatedVec->GetData());
      return rotatedVec;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtVec3d>, ReverseRotate, (const UtVec3d& aVecIn))
   {
      auto derotatedVec = ut::make_unique<UtVec3d>();
      self.ReverseRotate(aVecIn.GetData(), derotatedVec->GetData());
      return derotatedVec;
   };


   UT_SCRIPT_METHOD(void, SetRate, (const UtQuaternion& aQuat, const UtVec3d& bodyRatesDeg))
   {
      double bodyRatesRad[3] = {bodyRatesDeg[0] * UtMath::cRAD_PER_DEG,
                                bodyRatesDeg[1] * UtMath::cRAD_PER_DEG,
                                bodyRatesDeg[2] * UtMath::cRAD_PER_DEG};
      self.SetRate(aQuat, bodyRatesRad);
   };


   UT_SCRIPT_METHOD(void, Set, (double psi_deg, double theta_deg, double phi_deg))
   {
      self.Set(psi_deg * UtMath::cRAD_PER_DEG, theta_deg * UtMath::cRAD_PER_DEG, phi_deg * UtMath::cRAD_PER_DEG);
   };

   UT_SCRIPT_METHOD(double, Psi, ())
   {
      double psi, theta, phi;
      self.Get(psi, theta, phi);
      return psi * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_METHOD(double, Theta, ())
   {
      double psi, theta, phi;
      self.Get(psi, theta, phi);
      return theta * UtMath::cDEG_PER_RAD;
   };

   UT_SCRIPT_METHOD(double, Phi, ())
   {
      double psi, theta, phi;
      self.Get(psi, theta, phi);
      return phi * UtMath::cDEG_PER_RAD;
   };
}

//! Creates a new script reference of a new UtQuaternion object
UtScriptRef* UtScriptQuaternion::Create(const UtQuaternion& aQuat)
{
   return new UtScriptRef(new UtQuaternion(aQuat), sClassPtr, UtScriptRef::cMANAGE);
}

void* UtScriptQuaternion::Create(const UtScriptContext& aInstance)
{
   return new UtQuaternion();
}

void UtScriptQuaternion::Destroy(void* aObject)
{
   delete static_cast<UtQuaternion*>(aObject);
}

void* UtScriptQuaternion::Clone(void* aObject)
{
   return new UtQuaternion(*static_cast<UtQuaternion*>(aObject));
}

//! Return the string representation of a Quaternion
std::string UtScriptQuaternion::ToString(void* aObjectPtr) const
{
   std::string rv;
   if (aObjectPtr != nullptr)
   {
      auto&             v = *static_cast<UtQuaternion*>(aObjectPtr);
      std::stringstream ss;
      ss << '(' << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ')';
      rv = ss.str();
   }
   return rv;
}

//! Create a quaternion given a script variable.
//! The script variable should either be a Quaternion or an array<> containing at least 3 Euler angles as numbers.
UtQuaternion UtScriptQuaternion::ConvertToQuaternion(const UtScriptData& aData)
{
   UtQuaternion rv;
   if (aData.GetType() == ut::script::Data::Type::cPOINTER)
   {
      UtScriptRef* refPtr       = aData.GetPointer();
      void*        appObjectPtr = refPtr->GetAppObject();
      if (appObjectPtr != nullptr)
      {
         UtScriptClass* classPtr = refPtr->GetScriptClass();
         if (classPtr->GetClassName() == UtStringIdLiteral("Quaternion"))
         {
            rv = *refPtr->GetAppObject<UtQuaternion>();
         }
         else if (classPtr->IsContainer() && dynamic_cast<UtScriptArray*>(classPtr) != nullptr)
         {
            auto*  arrayPtr = static_cast<std::vector<UtScriptData>*>(appObjectPtr);
            double data[3]  = {0, 0, 0};
            if (arrayPtr != nullptr && arrayPtr->size() >= 3)
            {
               for (size_t i = 0; i < 3; ++i)
               {
                  data[i] = (*arrayPtr)[i].GetDouble();
               }
            }
            rv.Set(data[0], data[1], data[2]);
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
