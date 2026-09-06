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

#ifndef UTSCRIPTATMOSPHERE_HPP
#define UTSCRIPTATMOSPHERE_HPP

#include "util_script_export.h"

#include "UtAtmosphere.hpp"
#include "UtScriptClass.hpp"

//! Script Class for UtAtmosphere.
class UTIL_SCRIPT_EXPORT UtScriptAtmosphere : public UtScriptClass
{
public:
   UtScriptAtmosphere(UtScriptTypes* aScriptTypesPtr, const UtAtmosphere& aAtmosphere);

   void* Create(const UtScriptContext& aContext) override;
   void  Destroy(void* aObject) override;
   void* Clone(void* aObject) override;

   static UtScriptRef* Create(const UtAtmosphere& aAtmosphere);

   const UtAtmosphere mAtmosphere;
};

#endif
