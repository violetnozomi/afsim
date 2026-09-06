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

#ifndef UTCSV_HPP
#define UTCSV_HPP
#include <iosfwd>
#include <string>
#include <vector>

#include "WsfUtExport.hpp"

// Comma Separated Value file utilities
namespace UtCsv
{
typedef std::vector<std::string> StringList;
enum ParseResult
{
   cCSV_FIELD,
   cCSV_NEW_ROW,
   cCSV_EOF,
   cCSV_ERROR
};

//! Tokenizes a .csv text stream
class WSF_UT_EXPORT Parser
{
public:
   Parser()
      : mAtEof(false)
   {
   }
   ParseResult ReadToken(std::istream& aStream, std::string& aFieldValue);

   ParseResult ReadRow(std::istream& aStream, StringList& aRowValues);

private:
   bool mAtEof;
};

WSF_UT_EXPORT void Escape(std::string& aText);
WSF_UT_EXPORT std::vector<StringList> ToStrings(std::istream& aCsvStream);
WSF_UT_EXPORT std::string FromStrings(const std::vector<StringList>& aRows);
WSF_UT_EXPORT std::string RowFromStrings(const StringList& aRows);

} // namespace UtCsv

#endif
