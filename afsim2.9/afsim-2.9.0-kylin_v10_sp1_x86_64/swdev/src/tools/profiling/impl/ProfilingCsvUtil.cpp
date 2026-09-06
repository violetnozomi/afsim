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

#include "ProfilingCsvUtil.hpp"

#include <algorithm>

namespace profiling
{
namespace impl
{
constexpr char cCHARS_NEED_QUOTING[] = "\"\n\r,";
constexpr char cDOUBLE_QUOTE         = '\"';

/// @brief Assuming that <tt>aField.substr(startPos, endPos - startPos)</tt> is all double-quote characters,
///    return <tt>(endPos - startPos)</tt> rounded up to the nearest even number, number of double-quote characters.
std::string DoubleDoubleQuotesIfNeeded(const std::string& aField, const std::size_t startPos, const std::size_t endPos)
{
   const std::size_t numChars = endPos - startPos;
   if (numChars % 2u == 0)
   {
      return aField.substr(startPos, numChars);
   }
   else
   {
      return aField.substr(startPos, numChars) + impl::cDOUBLE_QUOTE;
   }
}

QuotedStringAndNewStartPos::QuotedStringAndNewStartPos(std::string quotedString, std::size_t newStartPos)
   : mQuotedString(std::move(quotedString))
   , mNewStartPos(newStartPos)
{
}

QuotedStringAndNewStartPos QuoteCsvFieldUpToFirstDoubleQuoteSequence(const std::string& aField,
                                                                     const std::size_t  startPos,
                                                                     const std::size_t  endPos)
{
   const std::size_t firstQuotePos = aField.find_first_of(cDOUBLE_QUOTE, startPos);
   if (firstQuotePos == std::string::npos) // no double-quotes in the string at all
   {
      return QuotedStringAndNewStartPos{aField.substr(startPos, endPos - startPos), endPos};
   }
   const std::size_t lastQuoteEnd = aField.find_first_not_of(cDOUBLE_QUOTE, firstQuotePos + 1);
   const std::size_t lastQuotePos = lastQuoteEnd == std::string::npos ? endPos : lastQuoteEnd;
   return QuotedStringAndNewStartPos{aField.substr(startPos, firstQuotePos - startPos) +
                                        DoubleDoubleQuotesIfNeeded(aField, firstQuotePos, lastQuotePos),
                                     lastQuotePos};
}

// Assume that we don't have to worry about enclosing quotes.
std::string QuoteCsvFieldWithoutEnclosingQuotes(const std::string& aField, std::size_t startPos, std::size_t endPos)
{
   std::string quotedString;
   while (startPos < endPos)
   {
      const auto result = QuoteCsvFieldUpToFirstDoubleQuoteSequence(aField, startPos, endPos);
      quotedString += result.mQuotedString;
      startPos = result.mNewStartPos;
   }
   return quotedString;
}

/// @return A string consisting of one double-quote character.
///
/// Fun fact: <tt>std::string(1, '\"')</tt> equals <tt>"\""</tt>,
/// but </tt>std::string{std::size_t(1), '\"'}</tt> (with curly braces) equals <tt>"\x1\""</tt>.
std::string oneDoubleQuote()
{
   return std::string(1, impl::cDOUBLE_QUOTE);
}

} // namespace impl

std::string QuoteCsvFieldIfNeeded(const std::string& aField)
{
   std::size_t quoteworthyCharPos = aField.find_first_of(impl::cCHARS_NEED_QUOTING);
   if (quoteworthyCharPos == std::string::npos)
   {
      // Fast path does not name return value,
      // thus simplifying return value optimization (RVO).
      // This also covers the empty string case.
      return aField;
   }
   else // we need to enclose the string in double-quotes.
   {
      constexpr auto    ONE            = std::size_t(1);
      const std::size_t originalEndPos = aField.size();

      // If the string is already enclosed in double-quotes,
      // then assume that the user meant it that way, and don't re-enclose it.
      // Be sure to consider the case "\"" (a single double-quote character).
      if (aField[0] == impl::cDOUBLE_QUOTE && aField[originalEndPos - ONE] == impl::cDOUBLE_QUOTE && originalEndPos != ONE)
      {
         return impl::oneDoubleQuote() + impl::QuoteCsvFieldWithoutEnclosingQuotes(aField, ONE, originalEndPos - ONE) +
                impl::cDOUBLE_QUOTE;
      }
      else
      {
         return impl::oneDoubleQuote() + impl::QuoteCsvFieldWithoutEnclosingQuotes(aField, 0, originalEndPos) +
                impl::cDOUBLE_QUOTE;
      }
   }
}

} // namespace profiling
