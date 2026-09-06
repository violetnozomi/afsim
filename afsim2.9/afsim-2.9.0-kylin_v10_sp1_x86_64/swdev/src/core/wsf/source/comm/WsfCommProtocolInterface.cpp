// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommProtocolInterface.hpp"

namespace wsf
{
namespace comm
{

// ============================================================================
ScriptProtocolClass::ScriptProtocolClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommProtocol");

   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;
}

} // namespace comm
} // namespace wsf
