// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommRouterProtocolInterface.hpp"

namespace wsf
{
namespace comm
{
namespace router
{

// ============================================================================
ScriptRouterProtocolClass::ScriptRouterProtocolClass(const std::string& aClassName, UtScriptTypes* aTypesPtr)
   : UtScriptClass(aClassName, aTypesPtr)
{
   SetClassName("WsfCommRouterProtocol");

   mConstructible      = false;
   mCloneable          = false;
   mIsScriptAccessible = true;
}

} // namespace router
} // namespace comm
} // namespace wsf
