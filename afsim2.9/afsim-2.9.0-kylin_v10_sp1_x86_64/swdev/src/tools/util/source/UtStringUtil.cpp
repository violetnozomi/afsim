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

#include "UtStringUtil.hpp"

#include <algorithm>
#include <bitset>
#include <cctype>
#include <sstream>

#include "UtStringRef.hpp"

std::string UtStringRef::emptyStr;

namespace UtStringUtil
{

unsigned int ParseWhitespace(const std::string& aInput, std::vector<std::string>& aOutput)
{
   aOutput.clear();
   std::string current;
   for (char c : aInput)
   {
      if (isspace(static_cast<unsigned char>(c)) != 0)
      {
         if (!current.empty())
         {
            aOutput.push_back(current);
            current.clear();
         }
      }
      else
      {
         current += c;
      }
   }

   if (!current.empty())
   {
      aOutput.push_back(current);
      // current = "";
   }
   return static_cast<unsigned int>(aOutput.size());
}

unsigned int Parse(const std::string& aInput, std::vector<std::string>& aOutput, char aDelimiter)
{
   size_t numParms   = 0;
   size_t outputSize = aOutput.size();
   size_t length     = aInput.size();
   size_t parmStart  = 0;

   // Go through input string and look for the delimiter
   if (length != 0u)
   {
      for (size_t j = 0; j <= length; ++j)
      {
         if (aInput[j] == aDelimiter || j == length)
         {
            // Found something to parse
            if (numParms < outputSize)
            {
               aOutput[numParms].assign(aInput, parmStart, j - parmStart);
            }
            else
            {
               aOutput.emplace_back(aInput, parmStart, j - parmStart);
            }
            ++numParms;
            parmStart = j + 1;
         }
      }
   }
   return static_cast<unsigned int>(numParms);
}

// Parse a string into a vector of strings separated by a delimiter word.
unsigned int Parse(const std::string& aInput, std::vector<std::string>& aOutput, const std::string& aDelimiter)
{
   aOutput.clear();
   // Go through input string and look for the delimiter
   size_t start     = 0;
   size_t delimSize = aDelimiter.length();
   while (true)
   {
      size_t end = std::string::npos;
      if (!aDelimiter.empty())
      {
         end = aInput.find(aDelimiter, start);
      }
      if (end != std::string::npos)
      {
         aOutput.push_back(aInput.substr(start, end - start));
         start = end + delimSize;
      }
      else
      {
         aOutput.push_back(aInput.substr(start));
         break;
      }
   }
   return static_cast<unsigned int>(aOutput.size());
}

// Split a string into multiple strings given a delimiter
std::vector<std::string> Split(const std::string& aText, char aDelimiter)
{
   std::vector<std::string> tok;
   Parse(aText, tok, aDelimiter);
   return tok;
}

// Combine a vector of strings into one, separated by aDelimiter
std::string Join(const std::string& aDelimiter, const std::vector<std::string>& aStringList)
{
   std::string output;
   for (size_t i = 0; i < aStringList.size(); ++i)
   {
      if (i != 0)
      {
         output.append(aDelimiter);
      }
      output.append(aStringList[i]);
   }
   return output;
}

// Remove whitespace from either end of a string
void TrimWhiteSpace(std::string& aString)
{
   std::string::iterator iter(aString.begin());

   while (iter != aString.end())
   {
      if (isspace(static_cast<unsigned char>(*iter)) == 0)
      {
         break;
      }
      ++iter;
   }

   if (iter != aString.begin())
   {
      // There were some leading white spaces, so trim them.
      aString.erase(aString.begin(), iter);
   }

   std::string::reverse_iterator rev_iter(aString.rbegin());
   while (rev_iter != aString.rend())
   {
      if (isspace(static_cast<unsigned char>(*rev_iter)) == 0)
      {
         break;
      }
      ++rev_iter;
   }

   if (rev_iter != aString.rbegin())
   {
      // There were some trailing white spaces, so trim them.
      aString.erase(rev_iter.base(), aString.end());
   }
}

// Remove a set of characters from either end of a string.
void Trim(std::string& aString, const std::string& aTrimCharacters)
{
   std::bitset<256> chars;
   for (const char c : aTrimCharacters)
   {
      chars.set(static_cast<unsigned char>(c));
   }
   std::string::iterator start(aString.begin());
   std::string::iterator end(aString.end());

   while ((start != end) && chars.test(static_cast<unsigned char>(*start)))
   {
      ++start;
   }
   if (start != end)
   {
      while (start != --end)
      {
         if (!chars.test(static_cast<unsigned char>(*end)))
         {
            break;
         }
      }
      ++end;
   }
   aString.assign(start, end);
}

// The original code used toupper and tolower in the 'transform' calls, but
// compilation failed on some systems because toupper and tolower are macros.
// Therefore, we define inline methods to invoke the real routine or macro.

namespace
{
inline int ToLowerChar(int c)
{
   return tolower(c);
}
inline int ToUpperChar(int c)
{
   return toupper(c);
}
} // namespace

void ToLower(std::string& aString)
{
   std::transform(aString.begin(), aString.end(), aString.begin(), ToLowerChar);
}

void ToUpper(std::string& aString)
{
   std::transform(aString.begin(), aString.end(), aString.begin(), ToUpperChar);
}

bool CompareNoCase(const std::string& aLhs, const std::string& aRhs)
{
   bool equal = true;

   if (aLhs.length() != aRhs.length())
   {
      equal = false;
   }
   else if (aLhs.empty() && aRhs.empty())
   {
      // equal = true;
   }
   else if (aLhs.empty() || aRhs.empty())
   {
      equal = false;
   }
   else
   {
      size_t i = 0;
      while ((i < aLhs.length()) && (i < aRhs.length()))
      {
         char c1 = aLhs[i];
         char c2 = aRhs[i];

         if (islower(c1) != 0)
         {
            c1 = toupper(c1);
         }
         if (islower(c2) != 0)
         {
            c2 = toupper(c2);
         }

         if (c1 != c2)
         {
            equal = false;
            break;
         }
         ++i;
      }
   }
   return equal;
}

void AddNewLines(std::string& aString, unsigned int aMaxLineLength)
{
   size_t lastSpace = 0;
   size_t maxLine   = static_cast<size_t>(aMaxLineLength);
   for (size_t j = 0; j < aString.size(); ++j)
   {
      if ((lastSpace != 0u) && j > maxLine)
      {
         maxLine            = lastSpace + static_cast<size_t>(aMaxLineLength) + 1;
         aString[lastSpace] = '\n';
         lastSpace          = 0;
      }
      if (aString[j] == ' ' || aString[j] == '\t')
      {
         lastSpace = j;
      }
      else if (aString[j] == '\n')
      {
         lastSpace = 0;
         maxLine   = j + static_cast<size_t>(aMaxLineLength) + 1;
      }
   }
}

size_t HashFun(const std::string& aString)
{
   size_t result = 0;

   std::string::const_iterator iter(aString.begin());
   while (iter != aString.end())
   {
      result = result * 5 + *iter++;
   }

   return result;
}

std::string ToString(long aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(long long aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(int aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(unsigned int aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(unsigned long aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(float aFloat)
{
   std::ostringstream oss;
   oss << aFloat;
   return oss.str();
}

std::string ToString(unsigned long long aInt)
{
   std::ostringstream oss;
   oss << aInt;
   return oss.str();
}

std::string ToString(double aDouble)
{
   std::ostringstream oss;
   oss << aDouble;
   return oss.str();
}

std::string ToString(long double aDouble)
{
   std::ostringstream oss;
   oss << aDouble;
   return oss.str();
}

std::string ToString(bool aBool)
{
   std::ostringstream oss;
   oss << std::boolalpha << aBool;
   return oss.str();
}

double ToDouble(const std::string& aString)
{
   double             value = 0.0;
   std::istringstream iss(aString);
   iss >> value;
   return value;
}

int ToInt(const std::string& aString)
{
   int                value = 0;
   std::istringstream iss(aString);
   iss >> value;
   return value;
}

bool ToBool(const std::string& aString)
{
   bool        value = false;
   std::string lowerCaseString(aString);
   ToLower(lowerCaseString);
   if (lowerCaseString == "true" || lowerCaseString == "1")
   {
      value = true;
   }
   return value;
}

bool ContainsNonNumeric(const std::string& aString)
{
   // Brute force.  Traverse the string and look for a character that
   // could not be part of a float or exponential number.  (Does not
   // guarantee that aString actually contains a valid number.)

   for (unsigned int i = 0; i != aString.size(); ++i)
   {
      if (!(((isdigit(aString[i])) != 0) || (aString[i] == 'E') || (aString[i] == 'e') || (aString[i] == '.') ||
            (aString[i] == '+') || (aString[i] == '-')))
      {
         return true;
      }
   }
   return false;
}

int ToOct(const std::string& aString)
{
   int                value;
   std::istringstream iss(aString);
   iss >> std::oct >> value;
   return value;
}

int ToHex(const std::string& aString)
{
   int                value;
   std::istringstream iss(aString);
   iss >> std::hex >> value;
   return value;
}

bool StartsWith(const std::string& aString, const std::string& aPrefix)
{
   return aString.size() >= aPrefix.size() && (0 == aString.compare(0, aPrefix.size(), aPrefix));
}

bool EndsWith(const std::string& aString, const std::string& aSuffix)
{
   return aString.size() >= aSuffix.size() &&
          (0 == aString.compare(aString.size() - aSuffix.size(), aSuffix.size(), aSuffix));
}

//! Read all lines from an input stream.
std::vector<std::string> ReadLines(std::istream& aInput)
{
   std::vector<std::string> lines;
   std::string              line;
   while (std::getline(aInput, line, '\n'))
   {
      if (!line.empty())
      {
         size_t lastCharIndex = line.size() - 1;
         // getline discards the \n, discard the '\r' if it exists
         if (line[lastCharIndex] == '\r')
         {
            line.erase(line.begin() + lastCharIndex);
         }
      }
      lines.push_back(line);
   }
   return lines;
}

void ReplaceAll(std::string& aString, const std::string& aReplaceThis, const std::string& aReplacement)
{
   size_t match = aString.find(aReplaceThis);
   while (match != std::string::npos)
   {
      aString.replace(match, aReplaceThis.length(), aReplacement);
      match = aString.find(aReplaceThis, match + aReplacement.length());
   }
}

} // namespace UtStringUtil
