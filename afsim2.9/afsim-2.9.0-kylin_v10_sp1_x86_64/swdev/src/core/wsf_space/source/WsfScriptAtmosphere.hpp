// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTATMOSPHEREMODEL_HPP
#define WSFSCRIPTATMOSPHEREMODEL_HPP

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

class WsfScriptAtmosphere : public UtScriptClass
{
public:
   WsfScriptAtmosphere(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~WsfScriptAtmosphere() override = default;

   void Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(ModelType);
   UT_DECLARE_SCRIPT_METHOD(Density);
};

#endif // WSFSCRIPTATMOSPHEREMODEL_HPP
