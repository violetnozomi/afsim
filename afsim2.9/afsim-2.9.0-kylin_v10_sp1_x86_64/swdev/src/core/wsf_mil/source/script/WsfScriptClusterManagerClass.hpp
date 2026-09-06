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

#ifndef WSFSCRIPTCLUSTERMANAGERCLASS_HPP
#define WSFSCRIPTCLUSTERMANAGERCLASS_HPP

#include "wsf_mil_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

//! Define script methods for WsfClusterManager.
class WSF_MIL_EXPORT WsfScriptClusterManagerClass : public UtScriptClass
{
public:
   WsfScriptClusterManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~WsfScriptClusterManagerClass() override = default;

   // virtual void* Create(const UtScriptContext& aInstance);
   // virtual void* Clone(void* aObjectPtr);
   void Destroy(void* aObjectPtr) override;
   UT_DECLARE_SCRIPT_METHOD(GetClusters_3); // GetClusters( WsfPlatformList )
};

#endif
