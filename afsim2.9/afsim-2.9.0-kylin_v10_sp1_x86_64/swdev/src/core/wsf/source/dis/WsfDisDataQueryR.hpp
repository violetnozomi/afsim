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

#ifndef WSFDISDATAQUERYR_HPP
#define WSFDISDATAQUERYR_HPP

#include "wsf_export.h"

#include <vector>

#include "DisDataQueryR.hpp"
class WsfDisDataR;
class WsfDisInterface;

class WSF_EXPORT WsfDisDataQueryR : public DisDataQueryR
{
public:
   WsfDisDataQueryR(WsfDisInterface* aInterfacePtr);
   WsfDisDataQueryR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisDataQueryR() override;

   int  Process() override;
   void TimedOut() override;

   // Method tailored to a particular use of the PDU:
   void MakeTimeAdvanceReportRequest(double aTimeInterval = 0.0);

protected:
   // Disallow assignment
   WsfDisDataQueryR& operator=(const WsfDisDataQueryR& aRhs) = delete;
   DisPdu*           ProduceResponse(bool& aIsDone);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
