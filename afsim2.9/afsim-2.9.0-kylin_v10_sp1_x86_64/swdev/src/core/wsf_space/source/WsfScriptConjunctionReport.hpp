// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFSCRIPTCONJUNCTION_HPP
#define WSFSCRIPTCONJUNCTION_HPP

#include "wsf_space_export.h"

#include "UtScriptClassDefine.hpp"
#include "script/WsfScriptObjectClass.hpp"

namespace wsf
{

namespace space
{

namespace script
{

//! A Script class to encapsulate the results of an orbital conjunction report.
class WSF_SPACE_EXPORT Conjunction : public WsfScriptObjectClass
{
public:
   Conjunction(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);
   ~Conjunction() override = default;

   UT_DECLARE_SCRIPT_METHOD(MissDistance);
   UT_DECLARE_SCRIPT_METHOD(RelativeVelocity);
   UT_DECLARE_SCRIPT_METHOD(StartTime);
   UT_DECLARE_SCRIPT_METHOD(EndTime);
   UT_DECLARE_SCRIPT_METHOD(MinimumTime);
   UT_DECLARE_SCRIPT_METHOD(MaximumProbability);
   UT_DECLARE_SCRIPT_METHOD(Primary);
   UT_DECLARE_SCRIPT_METHOD(Secondary);
};

} // namespace script

} // namespace space

} // namespace wsf


#endif // WSFSCRIPTCONJUNCTION_HPP