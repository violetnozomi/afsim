// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WSFPARSEDEBUGMARKERS_HPP
#define WSFPARSEDEBUGMARKERS_HPP

// Uncomment this to use UtMemoryDebug
//#define WSF_PARSE_DEBUG_MEMORY

#include "UtMemoryDebug.hpp"

enum WsfParseDebugMarkers
{
   cMDB_PROXY_STRUCT = 100,
   cMDB_PROXY_MAP,
   cMDB_PROXY_LIST,
   cMDB_PROXY_INDEX,
   cMDB_PROXY_BASIC_VALUE,
   cMDB_PROXY_REGISTRY,
   cMDB_PROXY,
   cMDB_PARSE_DEFINITIONS,
   cMDB_PARSE_INDEX,
   cMDB_PARSE_PARSER,
   cMDB_PARSE_PARSER_SHARED,
   cMDB_PARSE_NODE_POOL,
   cMDB_PARSE_UNDO
};

//#define PROXY_SERIALIZE_TRACE

#endif
