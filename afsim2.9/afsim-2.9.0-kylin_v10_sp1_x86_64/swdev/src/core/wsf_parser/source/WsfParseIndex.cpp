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

#include "WsfParseIndex.hpp"

#include <string>

#include "WsfParseType.hpp"
#include "WsfParser.hpp"

WsfParseIndex::WsfParseIndex(WsfParseDefinitions* aDefinitionsPtr)
   : mUserTypes(nullptr)
{
   mDefinitions = aDefinitionsPtr;
   mCurrentTypeStack.push_back(nullptr);
   mGeneratedNameCount = 0;
}

WsfParseIndex::~WsfParseIndex()
{
   // mUserTypeTable->DecRef();
}

void WsfParseIndex::Clear()
{
   mGeneratedNameCount = 0;
   mUserTypes.Clear();
   mIncludeCount.clear();
   mPathVariables.clear();
   ResetFilePath();
}

void WsfParseIndex::AddFilePath(const std::string& aPath)
{
   std::string path = SubstitutePathVariables(aPath);
   if (std::find(mIncludePath.begin(), mIncludePath.end(), path) == mIncludePath.end())
   {
      mIncludePath.push_back(path);
   }
}

void WsfParseIndex::DefinePathVariable(const std::string& aVariableName, const std::string& aValue)
{
   mPathVariables[aVariableName] = aValue;
}

void WsfParseIndex::UndefinePathVariable(const std::string& aVariableName)
{
   mPathVariables.erase(aVariableName);
}

void WsfParseIndex::ResetFilePath()
{
   mIncludePath.clear();
}

std::string WsfParseIndex::SubstitutePathVariables(const std::string& aString) const
{
   std::string            result(aString);
   std::string::size_type startPos = 0;

   while (startPos < result.size())
   {
      // Attempt to locate the next '$' that follows the end of the last substitution.

      std::string::size_type varBeg = result.find('$', startPos);
      if (varBeg == std::string::npos)
      {
         break; // '$' not found
      }
      if ((varBeg + 1) >= result.size())
      {
         break; // '$' at end of string.
      }

      // A '$' has been found. See if it is the start of a substitution request.

      char nextChar = result[varBeg + 1];
      if ((nextChar == '(') || (nextChar == '{'))
      {
         // '$(' or '${' - possible start of a macro
         if ((varBeg + 3) >= result.size())
         {
            break; // Not enough for '$(x)'
         }

         char                   endChar = (nextChar == '(') ? ')' : '}';
         std::string::size_type varEnd  = result.find(endChar, varBeg + 2);
         if (varEnd == std::string::npos)
         {
            break; // end of macro not found - leave the string as-is.
         }

         if (varEnd > (varBeg + 2))
         {
            // Extract the macro name and get the value. of the environment variable with the same name.

            std::string::size_type                             nameSize = varEnd - varBeg - 2;
            std::string                                        name     = result.substr(varBeg + 2, nameSize);
            std::string                                        value;
            std::map<UtStringRef, UtStringRef>::const_iterator varIter = mPathVariables.find(name);
            if (varIter != mPathVariables.end())
            {
               // Variable has been defined internally, so use its value.
               value = (*varIter).second;
            }
            else
            {
               // Variable has not been defined internally. Attempt to use the environment.
               char* valuePtr = ::getenv(name.c_str());
               if (valuePtr != nullptr)
               {
                  value = valuePtr;
               }
            }
            result.replace(varBeg, nameSize + 3, value);
            startPos = varBeg + value.size(); // Continue the scan after the replaced text.
         }
         else
         {
            startPos = varEnd + 1; // Null macro - '$()'. Just leave it.
         }
      }
      else if (nextChar == '$')
      {
         // '$$' - keep one '$' and continue with the character after the retained '$'
         result.erase(varBeg, 1);
         startPos = varBeg + 1;
      }
      else
      {
         // '$' followed by something other than '(', '{' or '$'.
         // Retain the '$' and continue with the character after the '$'.
         startPos = varBeg + 1;
      }
   }

   return result;
}
