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

#include "UtInputString.hpp"

#include <cctype>
#include <iostream>
#include <sstream>

/** Create a UtInput that will read commands from a string.

    @param aString The string from which commands are to be read.
    @throws OpenError if the string stream object cannot be created.
*/
UtInputString::UtInputString(const std::string& aString)
   : UtInputDocument()
{
   mDocument->Insert(0, aString.c_str(), (int)aString.length());
   mPos = mDocument->Begin();
}
