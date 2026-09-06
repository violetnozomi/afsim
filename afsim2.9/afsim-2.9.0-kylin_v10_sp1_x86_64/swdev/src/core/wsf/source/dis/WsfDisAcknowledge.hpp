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

#ifndef WSFDISACKNOWLEDGE_HPP
#define WSFDISACKNOWLEDGE_HPP

#include "wsf_export.h"

#include "DisAcknowledge.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisAcknowledge : public DisAcknowledge
{
public:
   WsfDisAcknowledge(WsfDisInterface* aInterfacePtr);
   WsfDisAcknowledge(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisAcknowledge() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisAcknowledge& operator=(const WsfDisAcknowledge& aRhs);
   WsfDisInterface*   mInterfacePtr;
};

#endif
