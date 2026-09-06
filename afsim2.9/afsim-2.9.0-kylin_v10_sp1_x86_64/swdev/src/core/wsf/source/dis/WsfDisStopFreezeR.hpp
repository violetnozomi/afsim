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

#ifndef WSFDISSTOPFREEZER_HPP
#define WSFDISSTOPFREEZER_HPP

#include "wsf_export.h"

#include "DisStopFreezeR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisStopFreezeR : public DisStopFreezeR
{
public:
   WsfDisStopFreezeR(WsfDisInterface* aInterfacePtr);
   WsfDisStopFreezeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisStopFreezeR() override;

   int  Process() override;
   void TimedOut() override;

private:
   // Disallow assignment
   WsfDisStopFreezeR& operator=(const WsfDisStopFreezeR& aRhs);
   DisPdu*            ProduceResponse(bool aIsDone);
   bool               ProcessStopFreezeRequest();
   bool               ActionIsForMe() const;

   WsfDisInterface* mInterfacePtr;
};

#endif
