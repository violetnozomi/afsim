// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef PARSEUTIL_HPP
#define PARSEUTIL_HPP

#include "artificer_export.h"

#include <map>
#include <string>

namespace artificer
{
///@brief Returns true if `aCurrent` and `aNext` indicate an escaped double quote in CSV.
inline bool IsEscapedQuote(char aCurrent, char aNext)
{
   return aCurrent == '"' && aNext == '"';
}

///@brief Returns true if `aCurrent` and `aNext` indicate a line ending in Windows text.
inline bool IsWindowsLineEnding(char aCurrent, char aNext)
{
   return aCurrent == '\r' && aNext == '\n';
}

///@brief Returns true if `aCurrent` and `aNext` indicate a line ending.
inline bool IsLineSeparator(char aCurrent, char aNext)
{
   return aCurrent == '\n' || IsWindowsLineEnding(aCurrent, aNext);
}

///@brief Returns true if `aCurrent` and `aNext` indicate a value separation in CSV.
inline bool IsValueSeparator(char aCurrent, char aNext)
{
   return aCurrent == ',' || IsLineSeparator(aCurrent, aNext);
}

} // namespace artificer

#endif
