// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptMethodBuilder.hpp"

namespace ut
{
namespace script
{

/// checks for edge-case illegally returned reference tracker.
bool IsIllegallReturnedMemMangement(UtScriptData& aReturnVal, UtScriptMethodArgs aVarArgs)
{
   if (aReturnVal.GetType() != UtScriptData::Type::cPOINTER || aReturnVal.GetPointer()->GetAppObject() == nullptr ||
       aReturnVal.GetPointer()->GetMemManagement() == UtScriptRef::cEXTERNALLY_MANAGED)
   {
      return false;
   }

   UtScriptRef& ref = *aReturnVal.GetPointer();
   for (const auto& arg : aVarArgs)
   {
      if (arg.GetType() != UtScriptData::Type::cPOINTER)
      {
         continue;
      }
      UtScriptRef& argref = *arg.GetPointer();
      if (ref.GetReferenceCounter() != argref.GetReferenceCounter() && ref.GetAppObject() == argref.GetAppObject())
      {
         return true;
      }
   }
   return false;
}

} // end namespace script
} // end namespace ut