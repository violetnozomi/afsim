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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "OutputItem.hpp"

#include <cstdio>
#include <iostream>

#include "Simulation.hpp"
#include "UtInput.hpp"
#include "Variables.hpp"

namespace engage
{

// TODO - NEED to decide if we are going to use C++ or C style user formatting directives.

// =================================================================================================
OutputItem::OutputItem()
   : mItemType(cNEW_LINE)
   , mVariableIndex(0)
   , mFormat()
   , mLiteral()
   , mScaleFactor(1.0)
{
}

// =================================================================================================
bool OutputItem::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "variable")
   {
      std::string name;
      aInput.ReadValue(name);

      // Sequentially try each of the variable types.
      mItemType = cREAL_VARIABLE;
      // mFormat   = " %f";
      mVariableIndex = Variables::FindRealVariable(name);
      if (mVariableIndex == 0)
      {
         mItemType = cINTEGER_VARIABLE;
         // mFormat   = " %d";
         mVariableIndex = Variables::FindIntegerVariable(name);
      }
      if (mVariableIndex == 0)
      {
         mItemType = cSTRING_VARIABLE;
         // mFormat   = " %s";
         mVariableIndex = Variables::FindStringVariable(name);
      }
      if (mVariableIndex == 0)
      {
         throw UtInput::BadValue(aInput, "Unknown variable: " + name);
      }

      // Process any format specifiers.
      while (aInput.ReadCommand(command))
      {
         if (command == "format")
         {
            aInput.ReadValueQuoted(mFormat);
         }
         else if (command == "units")
         {
            if (mItemType != cREAL_VARIABLE)
            {
               throw UtInput::BadValue(aInput, "units can only be specified for real variables");
            }
            std::string units;
            aInput.ReadValue(units);
            mScaleFactor = Variables::GetScaleFactor(mVariableIndex, units);
         }
         else
         {
            aInput.PushBack(command);
            break;
         }
      }
   }
   else if (command == "literal")
   {
      mItemType = cLITERAL;
      aInput.ReadValueQuoted(mLiteral);
   }
   else if (command == "new_line")
   {
      mItemType = cNEW_LINE;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
void OutputItem::DoOutput(std::ostream& aStream, Simulation& aSimulation) const
{
   switch (mItemType)
   {
   case cREAL_VARIABLE:
   {
      double r = Variables::GetRealValue(mVariableIndex, aSimulation, 0) * mScaleFactor;
      if (mFormat.empty())
      {
         aStream << ' ' << r;
      }
      else
      {
         char obuf[32];
         sprintf(obuf, mFormat.c_str(), r);
         aStream << obuf;
      }
   }
   break;

   case cINTEGER_VARIABLE:
   {
      int i = Variables::GetIntegerValue(mVariableIndex, aSimulation, 0);
      if (mFormat.empty())
      {
         aStream << ' ' << i;
      }
      else
      {
         char obuf[32];
         sprintf(obuf, mFormat.c_str(), i);
         aStream << obuf;
      }
   }
   break;

   case cSTRING_VARIABLE:
   {
      std::string s(Variables::GetStringValue(mVariableIndex, aSimulation, 0));
      if (mFormat.empty())
      {
         aStream << ' ' << s;
      }
      else
      {
         char obuf[128];
         sprintf(obuf, mFormat.c_str(), s.c_str());
         aStream << obuf;
      }
   }
   break;

   case cLITERAL:
      aStream << mLiteral;
      break;

   case cNEW_LINE:
      aStream << '\n';
      break;

   default:
      break;
   }
}

} // namespace engage
