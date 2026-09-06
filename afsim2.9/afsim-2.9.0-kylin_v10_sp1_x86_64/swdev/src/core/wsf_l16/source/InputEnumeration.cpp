// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "InputEnumeration.hpp"

#include <algorithm>
#include <functional>
#include <sstream>

#include "UtInput.hpp"

InputEnumeration::InputEnumeration() {}

InputEnumeration::InputEnumeration(const InputEnumeration& aSrc)
   : mValueList(aSrc.mValueList)
{
}

InputEnumeration::~InputEnumeration() {}

//! Remove all enumeration values.  Free memory.
void InputEnumeration::Clear()
{
   mValueList = std::shared_ptr<EnumList>();
}

//! Add an enumeration to the list.
void InputEnumeration::Add(int aValue, const char* aString)
{
   GetList().push_back(Pair(aString, aValue));
}

//! Add an enumeration to the list.
void InputEnumeration::Add(int aValue, const std::string& aString)
{
   GetList().push_back(Pair(aString, aValue));
}

//! Attempt to read the enumeration from aInput.
//! Throws UtInput::BadValue if there is a problem reading the input
//! @return 'true' if the value was read.
//! @return 'false' if aInput.GetCommand() != aCommand.
bool InputEnumeration::ProcessInput(const std::string& aCommand, UtInput& aInput, int& aEnumValue)
{
   bool myCommand = false;
   if (aInput.GetCommand() == aCommand)
   {
      myCommand = true;
      std::string enumString;
      aInput.ReadValue(enumString);

      EnumList::iterator i = GetList().begin();
      for (; i != GetList().end() && i->first != enumString; ++i)
      {
      }

      if (i != GetList().end())
      {
         aEnumValue = i->second;
      }
      else
      {
         std::ostringstream ss;
         ss << "Invalid value for " + aCommand + ".  Choices are: [ ";
         for (i = GetList().begin(); i != GetList().end(); ++i)
         {
            ss << i->first << " ";
         }
         ss << "]";
         UtInput::BadValue e(aInput, ss.str());
         throw e;
      }
   }
   return myCommand;
}

InputEnumeration::EnumList& InputEnumeration::GetList()
{
   if (!mValueList)
   {
      mValueList = std::make_shared<EnumList>();
   }
   return *mValueList;
}
