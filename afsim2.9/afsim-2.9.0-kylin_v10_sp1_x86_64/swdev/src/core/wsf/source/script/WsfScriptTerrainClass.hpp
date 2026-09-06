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

#ifndef WsfScriptTerrainClass_HPP
#define WsfScriptTerrainClass_HPP

#include "wsf_export.h"

#include "UtScriptBasicTypes.hpp"

//! WsfScriptTerrainClass is a UtScriptClass that defines the methods in
//! WsfTerrain.  This makes the methods available for use in script.

class WSF_EXPORT WsfScriptTerrainClass : public UtScriptClass
{
public:
   WsfScriptTerrainClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~WsfScriptTerrainClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void* Create(const UtScriptContext& aInstance) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(BathymetryElevApprox);
   UT_DECLARE_SCRIPT_METHOD(BathymetryElevInterp);
   UT_DECLARE_SCRIPT_METHOD(TerrainElevApprox);
   UT_DECLARE_SCRIPT_METHOD(TerrainElevInterp);
   UT_DECLARE_SCRIPT_METHOD(VegetationElevApprox);
   UT_DECLARE_SCRIPT_METHOD(VegetationElevInterp);
   UT_DECLARE_SCRIPT_METHOD(MaskedByTerrain);
   UT_DECLARE_SCRIPT_METHOD(MaskedByVegetation);
   UT_DECLARE_SCRIPT_METHOD(MaskedByUnderwaterTerrain);
};

#endif
