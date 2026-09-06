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

#ifndef UTSCRIPTVEC3DX_HPP
#define UTSCRIPTVEC3DX_HPP

#include "util_script_export.h"

#include "UtScriptClass.hpp"
#include "UtVec3dX.hpp"

//! Script Class for UtVec3dX
class UTIL_SCRIPT_EXPORT UtScriptVec3dX : public UtScriptClass
{
public:
   UtScriptVec3dX(UtScriptTypes* aScriptTypesPtr);

   static UtScriptRef* Create(const UtVec3d& aVec);
   void*               Create(const UtScriptContext& aContext) override;
   void                Destroy(void* aObject) override;
   void*               Clone(void* aObject) override;
   std::string         ToString(void* aObjectPtr) const override;

private:
   static UtScriptVec3dX* sClassPtr;
};

#endif
