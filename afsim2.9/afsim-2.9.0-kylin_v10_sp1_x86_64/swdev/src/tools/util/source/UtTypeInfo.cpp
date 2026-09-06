// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtTypeInfo.hpp"

#include <cctype>
#include <cstring>

#include "UtStringUtil.hpp"

#ifdef _WIN32

static bool IsIdent(char aCharacter) noexcept
{
   return std::isalnum(aCharacter) || aCharacter == '_';
}

//! Removes aQualifier from aName.
//! Does not remove aQualifier if it is a part of a larger identifier.
//! E.g. "struct foo" becomes "foo", but "foo_struct " remains "foo_struct ".
static void RemoveTypeQualifier(std::string& aName, const char* aQualifier)
{
   const std::size_t qualifierLength = std::strlen(aQualifier);
   std::size_t       position        = aName.find(aQualifier);

   while (position != std::string::npos)
   {
      if (position == 0 || !IsIdent(aName[position - 1]))
      {
         aName.erase(position, qualifierLength);
         position = aName.find(aQualifier, position);
      }
      else
      {
         position = aName.find(aQualifier, position + qualifierLength);
      }
   }
}

std::string ut::GetTypeName(const std::type_index& aType)
{
   std::string name = aType.name();

   RemoveTypeQualifier(name, "struct ");
   RemoveTypeQualifier(name, "class ");
   RemoveTypeQualifier(name, "union ");
   RemoveTypeQualifier(name, "enum ");

   UtStringUtil::ReplaceAll(name, " __ptr64", "");
   UtStringUtil::ReplaceAll(name, "__cdecl", "");

   UtStringUtil::ReplaceAll(name, " *", "*");
   UtStringUtil::ReplaceAll(name, " &", "&");
   UtStringUtil::ReplaceAll(name, "( ", "(");

   UtStringUtil::ReplaceAll(name, "__int64", "long long");
   UtStringUtil::ReplaceAll(name, "`anonymous namespace\'", "`anonymous`");
   UtStringUtil::ReplaceAll(name, "(void)", "()");
   UtStringUtil::ReplaceAll(name, ")const", ") const");

   return name;
}

#else

#include <cstdlib>

#include <cxxabi.h>

static std::string UnmangleTypeName(const char* aName)
{
   char* unmangledName = abi::__cxa_demangle(aName, nullptr, nullptr, nullptr);
   if (unmangledName)
   {
      std::string retval = unmangledName;
      std::free(unmangledName);
      return retval;
   }
   return aName;
}

std::string ut::GetTypeName(const std::type_index& aType)
{
   std::string name = UnmangleTypeName(aType.name());

   UtStringUtil::ReplaceAll(name, "(anonymous namespace)", "`anonymous`");
   UtStringUtil::ReplaceAll(name, ", ", ",");
   UtStringUtil::ReplaceAll(name, "[]", "[0]");

   return name;
}

#endif
