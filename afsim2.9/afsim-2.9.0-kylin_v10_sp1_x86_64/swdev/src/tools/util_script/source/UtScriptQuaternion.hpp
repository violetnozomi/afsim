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

#ifndef UTSCRIPTQUATERNION_HPP
#define UTSCRIPTQUATERNION_HPP

#include "util_script_export.h"

class UtQuaternion;
#include "UtScriptClass.hpp"

//! Script Class for UtQuaternion.
class UTIL_SCRIPT_EXPORT UtScriptQuaternion : public UtScriptClass
{
public:
   UtScriptQuaternion(UtScriptTypes* aScriptTypesPtr);

   static UtQuaternion ConvertToQuaternion(const UtScriptData& aData);

   void*       Create(const UtScriptContext& aInstance) override;
   void        Destroy(void* aObject) override;
   void*       Clone(void* aObject) override;
   std::string ToString(void* aObjectPtr) const override;

   static UtScriptRef* Create(const UtQuaternion& aQuat);

private:
   static UtScriptQuaternion* sClassPtr;
};

#endif
