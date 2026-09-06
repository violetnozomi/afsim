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

#include "UtInputXML.hpp"

/** Create a UtInput that will read commands from an XML-formatted string.

    @param aString The string from which commands are to be read.
    @throws OpenError if the string stream object cannot be created.
*/
UtInputXML::UtInputXML(const std::string& aString)
   : UtInputDocument()
{
   mDocument->Insert(0, aString.c_str(), (int)aString.length());
   mPos = mDocument->Begin();
}

/** Create a UtInput that will read commands from an XML file.

    @param aPath The name of the file from which commands are to be read.
    @throws OpenError if the file cannot be opened.
*/
UtInputXML::UtInputXML(const UtPath& aPath)
   : UtInputDocument()
{
   FILE* fin = fopen(aPath.GetSystemPath().c_str(), "rb");
   if (fin == nullptr)
   {
      throw OpenError(aPath.GetSystemPath().c_str());
   }
   else
   {
      fclose(fin);
      mDocument->ReadFile(aPath);
      mPos = mDocument->Begin();
   }
}

// protected
// virtual
bool UtInputXML::RealReadString(std::string& aValue)
{
   bool inputOk = false;
   aValue       = "";
   // Skip leading white space
   SkipWhiteSpace();

   // Assemble the string.
   char ch;
   Get(ch);

   // This is an XML tag
   if (ch == '<')
   {
      while (Get(ch))
      {
         if (ch != '>')
         {
            aValue += ch;
            inputOk = true;
         }
         else
         {
            inputOk = true;
            break;
         }
      }
   }
   // This is an XML value
   else
   {
      UnGet();

      while (Get(ch))
      {
         if (ch != '<')
         {
            aValue += ch;
            inputOk = true;
         }
         else
         {
            UnGet();
            inputOk = true;
            break;
         }
      }
   }
   return inputOk;
}
