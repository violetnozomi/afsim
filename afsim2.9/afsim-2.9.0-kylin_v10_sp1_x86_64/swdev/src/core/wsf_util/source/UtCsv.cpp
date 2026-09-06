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

#include "UtCsv.hpp"

#include <cassert>
#include <istream>
#include <sstream>

#include "UtStringUtil.hpp"

// Aggregates calls to ReadToken() to read a row at a time
// returns cCSV_ERROR -- no row was read, formatting error
//         cCSV_EOF   -- no row was read, end of file
//         cCSV_NEW_ROW -- the row was read
//          (never returns cCSV_FIELD)
// note: no checking is done to verify column count is constant
UtCsv::ParseResult UtCsv::Parser::ReadRow(std::istream& aStream, StringList& aRowValues)
{
   aRowValues.clear();
   if (mAtEof)
   {
      return cCSV_EOF;
   }
   while (1)
   {
      std::string        f;
      UtCsv::ParseResult result = ReadToken(aStream, f);
      if (result == cCSV_FIELD)
      {
         aRowValues.push_back(f);
      }
      else if (result == cCSV_NEW_ROW || result == cCSV_EOF)
      {
         return cCSV_NEW_ROW;
      }
      else if (result == cCSV_ERROR)
      {
         return cCSV_ERROR;
      }
   }
}

UtCsv::ParseResult UtCsv::Parser::ReadToken(std::istream& aStream, std::string& aFieldValue)
{
   enum State
   {
      cNORMAL,
      cNEW_ROW,
      cFIELD,
      cFIELD_IN_QUOTE_1,
      cFIELD_IN_QUOTE_2,
   };
   std::string fieldValue;
   int         state = 0;
   for (;;)
   {
      char c = 0;
      aStream.get(c);
      if (state == cNORMAL)
      {
         if (c == '\n' || c == '\r')
         {
            state = cNEW_ROW;
            continue;
         }
         if (!c)
         {
            mAtEof = true;
            return cCSV_EOF;
         }
         if (isspace(c))
         {
            continue;
         }
         if (c == ',')
         {
            goto gotField;
         }
         state = cFIELD;
      }
      if (state == cNEW_ROW)
      {
         if (isspace(c) || c == '\n' || c == '\r')
         {
            continue;
         }
         if (!c)
         {
            mAtEof = true;
            return cCSV_EOF;
         }
         aStream.putback(c);
         return cCSV_NEW_ROW;
      }
      if (state == cFIELD)
      {
         if (c == '"')
         {
            state = cFIELD_IN_QUOTE_1;
            continue;
         }
         else if (!c || c == ',')
         {
            goto gotField;
         }
         else if (c == '\n' || c == '\r')
         {
            aStream.putback(c);
            goto gotField;
         }
         else
         {
            fieldValue += c;
         }
      }
      if (state == cFIELD_IN_QUOTE_1)
      {
         if (!c)
         {
            return cCSV_ERROR; // no end quote
         }
         if (c == '"')
         {
            fieldValue += '"'; // double "", insert a "
            state = cFIELD;
            continue;
         }
         else
         {
            state = cFIELD_IN_QUOTE_2;
            fieldValue += c;
            continue;
         }
      }
      if (state == cFIELD_IN_QUOTE_2)
      {
         if (!c)
         {
            return cCSV_ERROR; // no end quote
         }
         if (c == '"')
         {
            state = cFIELD;
            continue;
         }
         else
         {
            fieldValue += c;
            continue;
         }
      }
   }
   assert(false); // no loop breaks
gotField:
   while (!aFieldValue.empty() && isspace(aFieldValue[aFieldValue.size() - 1]))
   {
      aFieldValue.erase(aFieldValue.end() - 1);
   }
   aFieldValue = fieldValue;
   return cCSV_FIELD;
}

//! Escape some text so that it can be written to a CSV field
//! If the text contains a comma, quote, or newline, the string is quoted.
void UtCsv::Escape(std::string& aText)
{
   bool mustQuote = aText.find_first_of("\",\r\n") != std::string::npos;
   if (!mustQuote)
   {
      return;
   }
   std::stringstream ss;
   ss << '"';
   std::vector<std::string> tokens;
   UtStringUtil::Parse(aText, tokens, '"');
   for (size_t i = 0; i < tokens.size(); ++i)
   {
      ss << "\"\"";
      ss << tokens[i];
   }
   ss << '"';
   aText = ss.str();
}

//! Parse a CSV text stream into an array of array of strings representing the rows of cells
std::vector<UtCsv::StringList> UtCsv::ToStrings(std::istream& aCsvStream)
{
   std::vector<StringList> rows;
   Parser                  p;
   StringList*             currentRow = nullptr;
   for (;;)
   {
      std::string v;
      ParseResult r = p.ReadToken(aCsvStream, v);
      if (r == cCSV_EOF)
      {
         break;
      }
      if (r == cCSV_ERROR)
      {
         break;
      }
      if (r == cCSV_FIELD)
      {
         if (!currentRow)
         {
            rows.push_back(StringList());
            currentRow = &rows.back();
         }
         currentRow->push_back(v);
      }
      if (r == cCSV_NEW_ROW)
      {
         currentRow = nullptr;
      }
   }
   return rows;
}

//! Create CSV text given an array of array of strings.
std::string UtCsv::FromStrings(const std::vector<StringList>& aRows)
{
   std::stringstream ss;

   for (size_t i = 0; i < aRows.size(); ++i)
   {
      if (i != 0)
      {
         ss << '\n';
      }
      const StringList& strings = aRows[i];
      for (size_t j = 0; j < strings.size(); ++j)
      {
         if (j != 0)
         {
            ss << ',';
         }
         std::string cell = strings[j];
         Escape(cell);
         ss << cell;
      }
   }
   return ss.str();
}

//! Given a list of strings a row's field values, return a single string
//! with conformant .csv row text
//! Note: the field values should not be escaped yet, or they could receive additional quotes
std::string UtCsv::RowFromStrings(const StringList& strings)
{
   std::stringstream ss;

   for (size_t j = 0; j < strings.size(); ++j)
   {
      if (j != 0)
      {
         ss << ',';
      }
      std::string cell = strings[j];
      Escape(cell);
      ss << cell;
   }
   return ss.str();
}
