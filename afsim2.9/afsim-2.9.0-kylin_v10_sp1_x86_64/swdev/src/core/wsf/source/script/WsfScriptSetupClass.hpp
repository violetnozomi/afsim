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

#ifndef WSFSCRIPTSETUPCLASS_HPP
#define WSFSCRIPTSETUPCLASS_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

//! Define script methods for WsfSimulation.
class WSF_EXPORT WsfScriptSetupClass : public UtScriptClass
{
public:
   WsfScriptSetupClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptSetupClass() override;

   // See the base class for a description of Create, Clone, and Destroy.
   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;
};

#endif
