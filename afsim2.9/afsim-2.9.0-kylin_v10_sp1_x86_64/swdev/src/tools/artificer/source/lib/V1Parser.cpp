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

#include "V1Parser.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

#include "AfperfSummarizerInterface.hpp"
#include "parseAfperf.hpp"
#include "parseUtil.hpp"

namespace
{
void ConsumeIfWindowsLineEnding(char aCurrent, char aNext, std::istream& aData)
{
   if (artificer::IsWindowsLineEnding(aCurrent, aNext))
   {
      // consume Windows line endings as a single line.
      aData.get();
   }
}
} // namespace

namespace artificer
{
bool V1Parser::CanHandle(const std::string& aHeader)
{
   return aHeader == "# AFPerf v1     ";
}

void V1Parser::Parse(std::istream& aData, AfperfSummarizerInterface& aSummarizer)
{
   // The stream should be positioned at the beginning of the content, after the header when this method is called.
   AfperfValue  currentValue;
   AfperfRecord currentRecord;
   bool         insideQuotedValue = false;
   bool         insideComment     = false;
   while (aData.good())
   {
      const char currentChar       = aData.get();
      const char nextChar          = aData.peek();
      const bool firstCharOfValue  = currentValue.empty();
      const bool firstCharOfRecord = currentRecord.empty() && firstCharOfValue;

      if (insideComment)
      {
         if (IsLineSeparator(currentChar, nextChar))
         {
            ConsumeIfWindowsLineEnding(currentChar, nextChar, aData);
            insideComment = false;
         }
      }
      else if (firstCharOfRecord && currentChar == '#')
      {
         insideComment = true;
      }
      else if (insideQuotedValue && IsEscapedQuote(currentChar, nextChar))
      {
         currentValue.push_back(aData.get());
      }
      else if (!insideQuotedValue && IsValueSeparator(currentChar, nextChar))
      {
         currentRecord.emplace_back(std::move(currentValue));
         currentValue.clear();
         if (IsLineSeparator(currentChar, nextChar))
         {
            if (!firstCharOfRecord)
            {
               aSummarizer.Summarize(currentRecord);
            }
            currentRecord.clear();
            ConsumeIfWindowsLineEnding(currentChar, nextChar, aData);
         }
      }
      else if (firstCharOfValue && currentChar == '"')
      {
         insideQuotedValue = true;
      }
      else if (insideQuotedValue && currentChar == '"')
      {
         insideQuotedValue = false;
      }
      else if (currentChar != std::istream::traits_type::eof())
      {
         currentValue.push_back(currentChar);
      }
   }
   // Handle the last record even if it's not closed with a line separator.
   if (!currentValue.empty())
   {
      currentRecord.emplace_back(std::move(currentValue));
   }
   if (!currentRecord.empty())
   {
      aSummarizer.Summarize(currentRecord);
   }
   aSummarizer.EndOfRecords();
}

} // namespace artificer
