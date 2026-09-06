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

#ifndef WSFDISSTARTRESUMER_HPP
#define WSFDISSTARTRESUMER_HPP

#include "wsf_export.h"

#include "DisStartResumeR.hpp"
class WsfDisInterface;
class WsfDisAcknowledgeR;

class WSF_EXPORT WsfDisStartResumeR : public DisStartResumeR
{
public:
   WsfDisStartResumeR(WsfDisInterface* aInterfacePtr);
   WsfDisStartResumeR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisStartResumeR() override;

   void TimedOut() override;

protected:
   // Disallow assignment
   WsfDisStartResumeR& operator=(const WsfDisStartResumeR& aRhs) = delete;
   DisPdu*             ProduceResponse(bool aIsDone);
   bool                ActionIsForMe() const;
   bool                ProcessStartCommand();

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
