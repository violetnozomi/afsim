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

#ifndef UTSCRIPTDCM_HPP
#define UTSCRIPTDCM_HPP

#include "util_script_export.h"

class UtDCM;
#include "UtScriptClass.hpp"

//! Script Class for UtDCM.
class UTIL_SCRIPT_EXPORT UtScriptDCM : public UtScriptClass
{
public:
   UtScriptDCM(UtScriptTypes* aScriptTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObject) override;
   void* Clone(void* aObject) override;

   static UtScriptRef* Create(const UtDCM& aDCM);

private:
   static UtScriptDCM* sClassPtr;
};

#endif
