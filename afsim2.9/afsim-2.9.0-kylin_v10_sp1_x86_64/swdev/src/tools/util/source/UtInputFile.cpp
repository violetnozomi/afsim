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

#include "UtInputFile.hpp"

#include <iostream>

/** Create a UtInput that will read commands from a file.

    @param aFileName The name of the file from which commands are to be read.
    @throws OpenError if the file cannot be opened.
*/
UtInputFile::UtInputFile(const std::string& aFileName)
   : UtInputDocument()
{
   FILE* fin = fopen(aFileName.c_str(), "rb");
   if (fin == nullptr)
   {
      throw OpenError(aFileName);
   }
   else
   {
      fclose(fin);
      mDocument->ReadFile(aFileName);
      mPos = mDocument->Begin();
   }
}
