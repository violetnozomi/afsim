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

#include "UtScriptDCM.hpp"

#include <sstream>

#include "UtDCM.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtVec3dX.hpp"

UtScriptDCM* UtScriptDCM::sClassPtr = nullptr;

UtScriptDCM::UtScriptDCM(UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass("DCM", aScriptTypesPtr)
{
   sClassPtr      = this;
   mConstructible = true;
   mCloneable     = true;

   UT_SCRIPT_WRAP_CLASS(UtDCM);

   // Methods
   UT_SCRIPT_WRAP_MEMBER("Test", Test); // NO_DOC | FOR_TEST_ONLY

   UT_SCRIPT_METHOD(UtVec3dX, Transform, (const UtVec3dX aRhs)) { return self.Transform(aRhs); };

   UT_SCRIPT_METHOD(UtVec3dX, InverseTransform, (const UtVec3dX aRhs)) { return self.InverseTransform(aRhs); };
}

//! Creates a new script reference of a new UtDCM object
UtScriptRef* UtScriptDCM::Create(const UtDCM& aDCM)
{
   return new UtScriptRef(new UtDCM(aDCM), sClassPtr, UtScriptRef::cMANAGE);
}

void* UtScriptDCM::Create(const UtScriptContext& aInstance)
{
   return new UtDCM();
}

void UtScriptDCM::Destroy(void* aObject)
{
   delete static_cast<UtDCM*>(aObject);
}

void* UtScriptDCM::Clone(void* aObject)
{
   return new UtDCM(*static_cast<UtDCM*>(aObject));
}
