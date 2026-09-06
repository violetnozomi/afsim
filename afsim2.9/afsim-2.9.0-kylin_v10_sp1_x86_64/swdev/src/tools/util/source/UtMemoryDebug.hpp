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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
#ifndef UTMEMORYDEBUG_HPP
#define UTMEMORYDEBUG_HPP

#include "ut_export.h"

#include <cstddef>
#include <set>
#include <vector>

//! Assists in finding leaked memory
class UT_EXPORT UtMemoryDebug
{
public:
   static void AddTrace(int aKind);
   static void RemoveTrace(int aKind);

   static size_t AddDetailTrace(int aKind);
   static void   RemoveDetailTrace(int aKind, int aId);
   static void   ShowDeleteError(int aKind);
   static bool   ShowLeakReport();
};

template<int ID>
class UtMemoryDebug_Tracer_Basic
{
public:
   UtMemoryDebug_Tracer_Basic() { UtMemoryDebug::AddTrace(ID); }
   UtMemoryDebug_Tracer_Basic(const UtMemoryDebug_Tracer_Basic& aRhs) { UtMemoryDebug::AddTrace(ID); }
   ~UtMemoryDebug_Tracer_Basic() { UtMemoryDebug::RemoveTrace(ID); }
};

template<int ID>
class UtMemoryDebug_Tracer_Detailed
{
public:
   UtMemoryDebug_Tracer_Detailed() { m___TraceId = UtMemoryDebug::AddDetailTrace(ID); }
   UtMemoryDebug_Tracer_Detailed(const UtMemoryDebug_Tracer_Detailed& aRhs)
   {
      m___TraceId = UtMemoryDebug::AddDetailTrace(ID);
   }
   ~UtMemoryDebug_Tracer_Detailed() { UtMemoryDebug::RemoveDetailTrace(ID, m___TraceId); }
   size_t m___TraceId;
};

//#ifndef NDEBUG
//#  define UTMEMORYDEBUG_LEVEL 2
//#endif

#ifndef UTMEMORYDEBUG_LEVEL
#define UTMEMORYDEBUG_LEVEL 0
#endif

#if UTMEMORYDEBUG_LEVEL == 0
#define UT_MEMORY_DEBUG_MARKER(KIND)
#elif UTMEMORYDEBUG_LEVEL == 1
// A lighter weight memory check just counts the number of each object kind currently in memory
#define UT_MEMORY_DEBUG_MARKER(KIND) UtMemoryDebug_Tracer_Basic<KIND> m___ut_memory_debug_marker
#elif UTMEMORYDEBUG_LEVEL == 2
// Keep ID of each object in memory
#define UT_MEMORY_DEBUG_MARKER(KIND) UtMemoryDebug_Tracer_Detailed<KIND> m___ut_memory_debug_marker
#endif


// Usage:
// enum DebugMarkers {
//    cDM_MyClass
// }
// ..
// class MyClass {
// ...
//    UT_MEMORY_DEBUG_MARKER(cDM_MyClass);
// }
//
#endif // UtMemoryDebug_h__
