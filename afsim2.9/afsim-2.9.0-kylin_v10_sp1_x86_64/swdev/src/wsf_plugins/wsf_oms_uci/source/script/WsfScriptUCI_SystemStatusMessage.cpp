// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfScriptUCI_SystemStatusMessage.hpp"

#include "WsfUCI_Messages.hpp"
#include "WsfUCI_Types.hpp"

namespace wsf
{
namespace script
{

UCI_SystemStatusMessageClass::UCI_SystemStatusMessageClass(const std::string& aClassName,
                                                           UtScriptTypes*     aTypesPtr)
   : UCI_MessageClass(aClassName, aTypesPtr)
{
   SetClassName(aClassName);
}

} // script
} // wsf
