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

#ifndef UTSCRIPTVEC3_HPP
#define UTSCRIPTVEC3_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"
#include "UtVec3.hpp"

//! Script Class for UtVec3d
class UTIL_SCRIPT_EXPORT UtScriptVec3 : public UtScriptClass
{
public:
   UtScriptVec3(UtScriptTypes* aScriptTypesPtr);

   static UtVec3d ConvertToVec3(const UtScriptData& aData);

   void*       Create(const UtScriptContext& aContext) override;
   void        Destroy(void* aObject) override;
   void*       Clone(void* aObject) override;
   std::string ToString(void* aObjectPtr) const override;

   static UtScriptRef* Create(const UtVec3d& aVec);

private:
   static UtScriptVec3* sClassPtr;
};

#endif
