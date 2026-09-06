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

#ifndef WSFDISACKNOWLEDGER_HPP
#define WSFDISACKNOWLEDGER_HPP

#include "wsf_export.h"

#include "DisAcknowledgeR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisAcknowledgeR : public DisAcknowledgeR
{
public:
   WsfDisAcknowledgeR(WsfDisInterface* aInterfacePtr);
   WsfDisAcknowledgeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisAcknowledgeR() override;

   int  Process() override;
   void TimedOut() override;

private:
   // Disallow assignment
   WsfDisAcknowledgeR& operator=(const WsfDisAcknowledgeR& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
