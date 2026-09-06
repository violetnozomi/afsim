// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTRECTANGULARFIELDOFVIEWCLASS_HPP
#define WSFSCRIPTRECTANGULARFIELDOFVIEWCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptFieldOfViewClass.hpp"

//! Script methods for WsfRectangularFieldOfView.
class WSF_EXPORT WsfScriptRectangularFieldOfViewClass : public WsfScriptFieldOfViewClass
{
public:
   WsfScriptRectangularFieldOfViewClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   bool EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(AzimuthFieldOfView);
   UT_DECLARE_SCRIPT_METHOD(ElevationFieldOfView);
};

#endif
