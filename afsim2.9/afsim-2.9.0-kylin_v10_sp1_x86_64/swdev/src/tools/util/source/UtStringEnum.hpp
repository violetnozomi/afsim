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

#ifndef VASTRINGTABLE_HPP
#define VASTRINGTABLE_HPP
#include "ut_export.h"

#include <stdexcept>
#include <string>
#ifdef UT_GENERATED_STRING_LIST
#include "UtStringEnum_gen.hpp"
#else
enum UtStringEnumId
{
}; // no enumeration generated
#endif

#define UTSTR(X) UtStringEnum(cSTR_##X)

/**
Efficiently store strings using hard-coded enumerations.
An application must register each string to be used.

Multiple libraries may each define a list of strings to be available
at compile time.  CMake is used to generate the final enumeration list
and string table.

Each project should do this in CMakeLists.txt:
   include(util/source/UtStringEnum.cmake)
   UT_STRING_ENUM("my_project_strings.txt")

Where my_project_strings.txt is a file containing the line delimited strings
*/
class UT_EXPORT UtStringEnum
{
public:
   using IdType = int;
   //! Registers all strings associated with the UtStringEnumId enumeration
   //! This Function should be called by the application before use of UtStringEnum
   static void               RegisterStrings();
   static void               RegisterString(IdType aStringId, const char* aStringPtr);
   static const std::string& MakeGlobalString(const std::string& aString);
   static void               ClearStringTable();
   //! Create a string enum given a string.  Adds a new string to the table if necessary.
   static UtStringEnum Create(const std::string& aString);

   struct HashFunction
   {
      size_t operator()(const UtStringEnum& aEnum) const { return (size_t)aEnum.GetId(); }
   };

   UtStringEnum() = default;
   UtStringEnum(UtStringEnumId aStringId)
      : mStringId((IdType)aStringId)
   {
   }
   UtStringEnum(IdType aStringId)
      : mStringId(aStringId)
   {
   }
   //! Construct a string enum given the textual representation
   //! The given string must have already been registered via RegisterString()
   UtStringEnum(const std::string& aString);

                      operator const std::string&() const;
                      operator UtStringEnumId() const { return (UtStringEnumId)mStringId; }
   const std::string& GetString() const;
   IdType             GetId() const { return mStringId; }

   bool operator==(const UtStringEnum& aRhs) const { return mStringId == aRhs.mStringId; }

private:
   IdType mStringId = 0;
};

#endif
