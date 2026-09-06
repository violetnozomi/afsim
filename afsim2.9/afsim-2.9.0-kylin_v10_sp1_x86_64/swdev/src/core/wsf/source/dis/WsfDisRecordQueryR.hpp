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

#ifndef WSFDISRECORDQUERYR_HPP
#define WSFDISRECORDQUERYR_HPP

#include "wsf_export.h"

#include "DisRecordQueryR.hpp"
class WsfDisDataR;
class WsfDisInterface;

class WSF_EXPORT WsfDisRecordQueryR : public DisRecordQueryR
{
public:
   WsfDisRecordQueryR(WsfDisInterface* aInterfacePtr);
   WsfDisRecordQueryR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRecordQueryR() override;

   int  Process() override;
   void TimedOut() override;

protected:
   // Disallow assignment
   WsfDisRecordQueryR& operator=(const WsfDisRecordQueryR& aRhs);
   bool                PopulateResponse(WsfDisDataR* aReplyPduPtr);
   WsfDisDataR*        ProduceResponse(bool& aIsComplete);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
