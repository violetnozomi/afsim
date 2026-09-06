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

#ifndef WSFSCRIPTORBITDESIGNER_HPP
#define WSFSCRIPTORBITDESIGNER_HPP

#include "wsf_space_export.h"

#include "UtScriptClass.hpp"
#include "UtScriptClassDefine.hpp"

namespace wsf
{

namespace space
{

namespace script
{

//! Define a script class for the WsfOrbitDesigner object.
//! The methods in this class return a platform that contains a space mover with the desired orbit
class WSF_SPACE_EXPORT OrbitDesigner : public UtScriptClass
{
public:
   OrbitDesigner(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~OrbitDesigner() override = default;

   //!@name Script methods documented in the AFSIM Reference Guide:
   UT_DECLARE_SCRIPT_METHOD(SunSynchronous);
};

} // namespace script

} // namespace space

} // namespace wsf

#endif
