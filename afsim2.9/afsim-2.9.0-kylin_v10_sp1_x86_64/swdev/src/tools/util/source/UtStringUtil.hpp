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

#ifndef UTSTRINGUTIL_HPP
#define UTSTRINGUTIL_HPP

#include "ut_export.h"

#include <cstddef>
#include <string>
#include <vector>

namespace UtStringUtil
{
// Function will fill vector with strings as it parses "aInput"
UT_EXPORT unsigned int Parse(const std::string& aInput, std::vector<std::string>& aOutput, char aDelimiter = ':');
UT_EXPORT unsigned int Parse(const std::string& aInput, std::vector<std::string>& aOutput, const std::string& aDelimiter);
UT_EXPORT unsigned int ParseWhitespace(const std::string& aInput, std::vector<std::string>& aOutput);

UT_EXPORT std::vector<std::string> Split(const std::string& aText, char aDelimiter);

UT_EXPORT std::string Join(const std::string& aDelimiter, const std::vector<std::string>& aStringList);

UT_EXPORT std::vector<std::string> ReadLines(std::istream& aInput);

UT_EXPORT bool   StartsWith(const std::string& aString, const std::string& aPrefix);
UT_EXPORT bool   EndsWith(const std::string& aString, const std::string& aSuffix);
UT_EXPORT void   TrimWhiteSpace(std::string& aString);
UT_EXPORT void   Trim(std::string& aString, const std::string& aTrimCharacters);
UT_EXPORT void   ToLower(std::string& aString);
UT_EXPORT void   ToUpper(std::string& aString);
UT_EXPORT bool   CompareNoCase(const std::string& aLhs, const std::string& aRhs);
UT_EXPORT void   AddNewLines(std::string& aString, unsigned int aMaxLineLength);
UT_EXPORT size_t HashFun(const std::string& aString);
UT_EXPORT std::string ToString(int aInt);
UT_EXPORT std::string ToString(long aInt);
UT_EXPORT std::string ToString(long long aInt);
UT_EXPORT std::string ToString(unsigned long long aInt);
UT_EXPORT std::string ToString(unsigned int aInt);
UT_EXPORT std::string ToString(unsigned long aInt);
UT_EXPORT std::string ToString(float aFloat);
UT_EXPORT std::string ToString(double aDouble);
UT_EXPORT std::string ToString(long double aDouble);
UT_EXPORT std::string ToString(bool aBool);
UT_EXPORT double      ToDouble(const std::string& aString);
UT_EXPORT int         ToInt(const std::string& aString);
UT_EXPORT bool        ToBool(const std::string& aString);
UT_EXPORT bool        ContainsNonNumeric(const std::string& aString);
UT_EXPORT int         ToOct(const std::string& aString);
UT_EXPORT int         ToHex(const std::string& aString);
UT_EXPORT void ReplaceAll(std::string& aString, const std::string& aReplaceThis, const std::string& aReplacement);
} // namespace UtStringUtil

#endif
