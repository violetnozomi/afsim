// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************


#include <string>

#include <gtest/gtest.h>

#include "ProfilingCsvUtil.hpp"

namespace
{
TEST(ProfilingCsvUtil, ImplQuoteCsvFieldUpToFirstDoubleQuoteSequence)
{
   auto testOne = [](const char        testString[],
                     const std::size_t endPos,
                     const char        expectedUpToResultString[],
                     const std::size_t expectedUpToResultEndPos,
                     const char        expectedFullyQuotedResultString[])
   {
      using profiling::impl::QuoteCsvFieldUpToFirstDoubleQuoteSequence;
      using profiling::impl::QuoteCsvFieldWithoutEnclosingQuotes;
      using profiling::impl::QuotedStringAndNewStartPos;

      const std::size_t                startPos = 0;
      const QuotedStringAndNewStartPos result = QuoteCsvFieldUpToFirstDoubleQuoteSequence(testString, startPos, endPos);
      EXPECT_EQ(result.mQuotedString, expectedUpToResultString);
      EXPECT_EQ(result.mNewStartPos, expectedUpToResultEndPos);

      const std::string fullyQuoted = QuoteCsvFieldWithoutEnclosingQuotes(testString, startPos, endPos);
      EXPECT_EQ(fullyQuoted, expectedFullyQuotedResultString);
   };

   testOne("", 0, "", 0, "");
   testOne("a", 1, "a", 1, "a");
   testOne("\"", 1, "\"\"", 1, "\"\"");
   testOne("\"\"", 2, "\"\"", 2, "\"\"");
   testOne("\"\"\"", 3, "\"\"\"\"", 3, "\"\"\"\"");

   testOne("foo", 3, "foo", 3, "foo");
   testOne("foo,", 4, "foo,", 4, "foo,");
   testOne(",foo", 4, ",foo", 4, ",foo");
   testOne("f,oo", 4, "f,oo", 4, "f,oo");

   testOne("foo\"", 4, "foo\"\"", 4, "foo\"\"");
   testOne("foo\"\"", 5, "foo\"\"", 5, "foo\"\"");
   testOne("foo\"\"\"", 6, "foo\"\"\"\"", 6, "foo\"\"\"\"");

   testOne("\"foo", 4, "\"\"", 1, "\"\"foo");
   testOne("\"\"foo", 5, "\"\"", 2, "\"\"foo");
   testOne("\"\"\"foo", 6, "\"\"\"\"", 3, "\"\"\"\"foo");

   testOne("foo\"barbaz", 10, "foo\"\"", 4, "foo\"\"barbaz");
   testOne("foo\"\"barbaz", 11, "foo\"\"", 5, "foo\"\"barbaz");
   testOne("foo\"\"\"barbaz", 12, "foo\"\"\"\"", 6, "foo\"\"\"\"barbaz");
}

TEST(ProfilingCsvUtil, QuoteCsvFieldIfNeeded)
{
   using profiling::QuoteCsvFieldIfNeeded;

   // An empty field is valid and needs no quoting.
   EXPECT_EQ(QuoteCsvFieldIfNeeded(""), "");

   // Examples that don't need quoting.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo"), "foo");
   // The function preserves case.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("FoO0"), "FoO0");
   // Spaces don't need quoting.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar 123"), "foo Bar 123");
   // Spaces can occur anywhere in the string, and are preserved.
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" foo Bar 123"), " foo Bar 123");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar 123 "), "foo Bar 123 ");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("   foo  Bar 123"), "   foo  Bar 123");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo  Bar 123   "), "foo  Bar 123   ");

   // One double-quote, not enclosed by double-quotes,
   // gets enclosed with double-quotes _and_ doubled.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\""), "\"\"\"\"");
   // Test the single-character, not a quote case.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("a"), "a");
   // Test various numbers of spaces.
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" "), " ");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("  "), "  ");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("   "), "   ");

   // Test that the function is idempotent (doesn't quote if already quoted).
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"\""), "\"\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\" \""), "\" \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"   \""), "\"   \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"foo Bar 123\""), "\"foo Bar 123\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"foo , \nBar\""), "\"foo , \nBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"foo\rBar\""), "\"foo\rBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"  foo Bar 123\""), "\"  foo Bar 123\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"foo , \nBar  \""), "\"foo , \nBar  \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"  foo\rBar\""), "\"  foo\rBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"  foo\"\"Bar\""), "\"  foo\"\"Bar\"");

   // Test that quoting works for a single instance of a needs-quoting character
   // in the middle of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\"Bar"), "\"foo\"\"Bar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\nBar"), "\"foo\nBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\rBar"), "\"foo\rBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo,Bar"), "\"foo,Bar\"");
   // Same, but with spaces in different parts of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" foo\"Bar"), "\" foo\"\"Bar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" foo\nBar"), "\" foo\nBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\rBar "), "\"foo\rBar \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo,Bar "), "\"foo,Bar \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\" Bar"), "\"foo\"\" Bar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\n Bar"), "\"foo\n Bar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo \rBar"), "\"foo \rBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo ,Bar"), "\"foo ,Bar\"");

   // Test that quoting works for a single instance of a needs-quoting character
   // at the beginning of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"fooBar"), "\"\"\"fooBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\nfooBar"), "\"\nfooBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\rfooBar"), "\"\rfooBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded(",fooBar"), "\",fooBar\"");
   // Same, but with spaces in different parts of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\" fooBar"), "\"\"\" fooBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\n fooBar"), "\"\n fooBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\rfooBar "), "\"\rfooBar \"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded(",fooBar "), "\",fooBar \"");

   // Test that quoting works for a single instance of a needs-quoting character
   // at the end of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar\""), "\"fooBar\"\"\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar\n"), "\"fooBar\n\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar\r"), "\"fooBar\r\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar,"), "\"fooBar,\"");
   // Same, but with spaces in different parts of the string.
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" fooBar\""), "\" fooBar\"\"\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded(" fooBar\n"), "\" fooBar\n\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar \r"), "\"fooBar \r\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("fooBar ,"), "\"fooBar ,\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar\""), "\"foo Bar\"\"\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar\n"), "\"foo Bar\n\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar\r"), "\"foo Bar\r\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo Bar,"), "\"foo Bar,\"");

   // Test that quoting works for multiple instances of needs-quoting characters.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo , \nBar"), "\"foo , \nBar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\r\"\"Bar"), "\"foo\r\"\"Bar\"");

   // RFC 4180:
   //
   // "Fields containing line breaks (CRLF), double quotes, and commas
   // should be enclosed in double-quotes."
   //
   // "If double-quotes are used to enclose fields, then a double-quote
   // appearing inside a field must be escaped by preceding it with
   // another double quote."
   //
   // Combining the two rules implies that a single double-quote
   // occurring in the middle of the field calls for the entire field
   // to be enclosed by double-quotes.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\"Bar"), "\"foo\"\"Bar\"");
   // A pair of adjacent double-quotes are already sufficiently escaped.
   // We don't want to escape each of them, because that will make the string
   // longer and longer if we iterate the function.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\"\"Bar"), "\"foo\"\"Bar\"");
   EXPECT_EQ(QuoteCsvFieldIfNeeded("\"foo\"\"Bar\""), "\"foo\"\"Bar\"");

   // Test multiple nonconsecutive double-quotes.
   EXPECT_EQ(QuoteCsvFieldIfNeeded("foo\"Bar\"baz"), "\"foo\"\"Bar\"\"baz\"");
}

} // namespace
