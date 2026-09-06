// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPPROXYSTRUCTHEADER_HPP
#define WSFPPROXYSTRUCTHEADER_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyPath.hpp"
class WsfPProxyStructType;

struct WsfPProxyStructHeader
{
public:
   enum StructFlags
   {
      cSF_POINTER_FMT      = 1, // if set, the data format consists of a single pointer to the rest of the struct data
      cSF_EXPANDED_FMT     = 2,
      cSF_HEADER_TYPE_MASK = 3,
      cSF_UNSET            = 4, // if set, the struct is in an 'unset' state
      cSF_INHERITED        = 8, // if set, the struct's value is inherited
      cSF_CONSTRUCTED      = 0x10
   };

   const static unsigned int cSF_INSTANTIATION_MASK = cSF_UNSET;

   // Struct flags
   int mStructFlags;

#ifdef WSF_PARSE_DEBUG_MEMORY
   size_t mInstanceMemoryTraceId;
#endif
   // This points to the start of the struct's data
   void* mStructDataPtr;
   // Both inline and non-inline formats always have these two fields:
   const WsfPProxyStructType* mStructPtr;
   // Path to the base struct, indicates inheritance when set.
   WsfPProxyPath mBasePath;

   // add a path to 'this'

   void* GetStart() { return mStructDataPtr; }

#ifndef NDEBUG
   void Constructing()
   {
      assert(0 == (mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED));
      mStructFlags |= WsfPProxyStructHeader::cSF_CONSTRUCTED;
#ifdef WSF_PARSE_DEBUG_MEMORY
      mInstanceMemoryTraceId = UtMemoryDebug::AddDetailTrace(cMDB_PROXY_STRUCT);
#endif
   }
   void Destroying()
   {
      assert(mStructFlags & WsfPProxyStructHeader::cSF_CONSTRUCTED);
      mStructFlags &= ~WsfPProxyStructHeader::cSF_CONSTRUCTED;
#ifdef WSF_PARSE_DEBUG_MEMORY
      UtMemoryDebug::RemoveDetailTrace(cMDB_PROXY_STRUCT, mInstanceMemoryTraceId);
      mInstanceMemoryTraceId = 0;
#endif
   }
#else
   void Constructing() {}
   void Destroying() {}
#endif

   WsfPProxyPath& GetBasePath() { return mBasePath; }
};

#endif
