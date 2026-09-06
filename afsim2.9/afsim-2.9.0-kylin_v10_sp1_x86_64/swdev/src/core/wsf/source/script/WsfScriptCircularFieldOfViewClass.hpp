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

#ifndef WSFSCRIPTCIRCULARFIELDOFVIEWCLASS_HPP
#define WSFSCRIPTCIRCULARFIELDOFVIEWCLASS_HPP

#include "wsf_export.h"

#include "WsfScriptFieldOfViewClass.hpp"

//! Define script methods for WsfCircularFieldOfView.
class WSF_EXPORT WsfScriptCircularFieldOfViewClass : public WsfScriptFieldOfViewClass
{
public:
   WsfScriptCircularFieldOfViewClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   bool EqualTo(void* aLHS, void* aRHS) override;

   UT_DECLARE_SCRIPT_METHOD(Construct);
   UT_DECLARE_SCRIPT_METHOD(HalfAngle);
};

#endif
