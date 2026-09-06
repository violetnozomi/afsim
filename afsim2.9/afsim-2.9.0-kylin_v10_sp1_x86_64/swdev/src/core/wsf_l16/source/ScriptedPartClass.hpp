// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef L16_SCRIPTEDPARTCLASS_HPP
#define L16_SCRIPTEDPARTCLASS_HPP

#include "script/WsfScriptObjectClass.hpp"

namespace WsfL16
{
//! Define script methods for WsfProcessor.
class ScriptedPartClass : public WsfScriptObjectClass
{
public:
   ScriptedPartClass(UtScriptTypes* aScriptTypesPtr);

   static void RegisterType(UtScriptTypes& aTypes);

   UT_DECLARE_SCRIPT_METHOD(SendJMessage);
};
} // namespace WsfL16

#endif
