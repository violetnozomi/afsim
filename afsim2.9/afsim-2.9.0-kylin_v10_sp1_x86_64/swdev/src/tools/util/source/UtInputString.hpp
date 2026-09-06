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

#ifndef UTINPUTSTRING_HPP
#define UTINPUTSTRING_HPP

#include "ut_export.h"

#include <iosfwd>
#include <string>

#include "UtInputDocument.hpp"

/** A specialization of UtInput that represents input that originates from a string.

   This file defines only the protected methods to perform the low-level input
   operations. UtInput file defines the application methods to read input values.
*/

class UT_EXPORT UtInputString : public UtInputDocument
{
public:
   UtInputString(const std::string& aString);
};

#endif
