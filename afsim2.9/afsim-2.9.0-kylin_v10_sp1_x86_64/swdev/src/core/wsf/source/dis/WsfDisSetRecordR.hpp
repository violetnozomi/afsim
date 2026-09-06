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

#ifndef WSFDISSETRECORDR_HPP
#define WSFDISSETRECORDR_HPP

#include "wsf_export.h"

#include "DisSetRecordR.hpp"
class WsfDisRecordR;
class WsfDisInterface;

class WSF_EXPORT WsfDisSetRecordR : public DisSetRecordR
{
public:
   WsfDisSetRecordR(WsfDisInterface* aInterfacePtr);
   WsfDisSetRecordR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisSetRecordR() override;

   int  Process() override;
   void TimedOut() override;

protected:
   // Disallow assignment
   WsfDisSetRecordR& operator=(const WsfDisSetRecordR& aRhs);
   bool              ProcessSetRecordRequest();
   bool              PopulateReply(WsfDisRecordR* aResponsePtr);
   WsfDisRecordR*    ProduceReply();

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
