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

#ifndef WSFPARSEUTIL_HPP
#define WSFPARSEUTIL_HPP

#include "wsf_parser_export.h"
// Helper functions for parsing:

namespace WsfParseUtil
{
extern WSF_PARSER_EXPORT bool cIS_WHITESPACE[256];
extern WSF_PARSER_EXPORT bool cIS_ALPHANUMERIC[256];
//! 'true' if parsing optimizations are enabled.  Parsing optimizations
//! add time to startup, but improve speed of the parser.  Optimizations
//! are disabled by default.
extern WSF_PARSER_EXPORT bool cOPTIMIZE_PARSE;
WSF_PARSER_EXPORT void        InitializeParseUtil();

inline bool IsWhitespace(char c)
{
   return cIS_WHITESPACE[(unsigned char)c];
}
inline bool IsDigit(char c)
{
   return c >= '0' && c <= '9';
}
inline bool IsAlphaNumeric(char c)
{
   return cIS_ALPHANUMERIC[(unsigned char)c];
}
template<typename ITER_TYPE>
void TrimIter(ITER_TYPE& aStrBegin, ITER_TYPE& aStrEnd)
{
   while (aStrBegin != aStrEnd && IsWhitespace(*(aStrEnd - 1)))
   {
      --aStrEnd;
   }
   while (aStrBegin != aStrEnd && IsWhitespace(*aStrBegin))
   {
      ++aStrBegin;
   }
}
} // namespace WsfParseUtil
#endif
