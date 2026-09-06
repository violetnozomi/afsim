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

#ifndef WSFDISDATAR_HPP
#define WSFDISDATAR_HPP

#include "wsf_export.h"

#include "DisDataR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisDataR : public DisDataR
{
public:
   WsfDisDataR(WsfDisInterface* aInterfacePtr);
   WsfDisDataR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisDataR() override;

   int Process() override;

   void MakeTimeAdvanceReport(const DisEntityId& aAddressee, double aIssueInterval, double aAppTime, double aAppRate);

protected:
   // Disallow assignment
   WsfDisDataR& operator=(const WsfDisDataR& aRhs);
   bool         ProcessResponseTo(DisPdu* aInitiatorPduPtr);
   bool         ProcessTimeAdvanceReport();

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
