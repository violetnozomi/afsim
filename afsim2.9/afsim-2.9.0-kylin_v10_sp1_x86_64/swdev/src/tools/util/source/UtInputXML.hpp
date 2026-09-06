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

#ifndef UTINPUTXML_HPP
#define UTINPUTXML_HPP

#include "UtInputDocument.hpp"


/** A specialization of UtInput that represents XML-formatted input.

   This file defines only the protected methods to perform the low-level input
   operations. UtInput file defines the application methods to read input values.
*/
class UT_EXPORT UtInputXML : public UtInputDocument
{
public:
   UtInputXML(const std::string& aString);
   UtInputXML(const UtPath& aPath);

protected:
   bool RealReadString(std::string& aValue) override;
};

#endif
