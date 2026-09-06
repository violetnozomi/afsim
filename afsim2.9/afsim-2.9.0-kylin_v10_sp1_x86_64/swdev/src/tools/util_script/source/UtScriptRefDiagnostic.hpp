// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

//// Uncomment the following line to enable a diagnostic print of the allocated
//// script object totals.  This will include only non-basic types which are
//// memory-managed by the scripting language.
//#define UT_SCRIPT_REF_DIAGNOSTIC

#ifdef UT_SCRIPT_REF_DIAGNOSTIC
#define UT_SCRIPT_REF_DIAGNOSTIC_INCREMENT()                 \
   if (mCounterPtr->GetStrongCount() == 1)                   \
   {                                                         \
      ++g_ReferenceCounter[mScriptClassPtr->GetClassName()]; \
      CheckForPrint();                                       \
   }
#define UT_SCRIPT_REF_DIAGNOSTIC_DECREMENT()              \
   --g_ReferenceCounter[mScriptClassPtr->GetClassName()]; \
   CheckForPrint();
#include <iomanip>
#include <iostream>

#include "UtLog.hpp"
// By default, wait for 2000 Creates and Deletes before printing total reference counts
static const int cPRINT_SCRIPT_REF_INTERVAL = 2000;
namespace
{
std::map<std::string, int> g_ReferenceCounter;
int                        g_NextPrintEvent = cPRINT_SCRIPT_REF_INTERVAL;
void                       PrintScriptReferenceDiagnostic(UtDictionary& aDict)
{
   int  total = 0;
   auto out   = ut::log::info() << "+ Allocated Script Objects:" << std::left << std::setw(30);
   for (std::map<int, int>::iterator iter = g_ReferenceCounter.begin(); iter != g_ReferenceCounter.end(); ++iter)
   {
      out.AddNote() << aDict.GetString(iter->first) << ": " << iter->second;
      total += iter->second;
   }
   out.AddNote() << "Total: " << total;
}
void CheckForPrint()
{
   if (--g_NextPrintEvent < 0)
   {
      g_NextPrintEvent = cPRINT_SCRIPT_REF_INTERVAL;
      PrintScriptReferenceDiagnostic(UtScriptData::GetDictionary());
   }
}
} // namespace
#else
#define UT_SCRIPT_REF_DIAGNOSTIC_INCREMENT()
#define UT_SCRIPT_REF_DIAGNOSTIC_DECREMENT()
#endif
