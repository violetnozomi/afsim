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

#ifndef PROFILINGCSVUTIL_HPP
#define PROFILINGCSVUTIL_HPP

#include <string>

namespace profiling
{
namespace impl
{
/// Partial quoted string result and new start position.
struct QuotedStringAndNewStartPos
{
   // This constructor exists only because GCC 4.8
   // does not correctly implement aggregate initialization.
   QuotedStringAndNewStartPos(std::string quotedString, std::size_t newStartPos);

   std::string mQuotedString;
   std::size_t mNewStartPos = 0;
};

/// @return Result of quoting <tt>aField.substr(startPos, endPos)</tt>
///    up to and including the first sequence of consecutive double-quote characters,
///    but not including anything after that.
///
/// This is an implementation detail of @a QuoteCsvFieldWithoutEnclosingQuotes (which see).
///
/// @param aField Input string to quote.
/// @param startPos Inclusive start position in the input string.
/// @param endPos Exclusive end position in the input string.
QuotedStringAndNewStartPos QuoteCsvFieldUpToFirstDoubleQuoteSequence(const std::string& aField,
                                                                     const std::size_t  startPos,
                                                                     const std::size_t  endPos);

/// @returns The substring of @c aField starting at @c startPos
///    and with <tt>endPos - startPos</tt> length,
///    but with any isolated double-quote characters
///    replaced with pairs of double-quote characters.
///
/// This is an implementation detail of @c QuoteCsvFieldIfNeeded
/// and is exposed only for the sake of unit testing.
///
/// This does everything for a CSV field per RFC 4180,
/// except for enclosing the field in double-quotes.
///
/// @param aField Input string to quote.
/// @param startPos Inclusive start position in the input string.
/// @param endPos Exclusive end position in the input string.
std::string QuoteCsvFieldWithoutEnclosingQuotes(const std::string& aField, std::size_t startPos, std::size_t endPos);

} // namespace impl

/// Quote the given string CSV (comma-separated values) field if needed.
///
/// @param aField String to use as a field in CSV output.
///
/// @return The input string, changed if and only if needed
///    to make it a valid CSV field, per RFC 4180.
std::string QuoteCsvFieldIfNeeded(const std::string& aField);

} // namespace profiling

#endif
