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

#ifndef WSFDISREMOVEENTITYR_HPP
#define WSFDISREMOVEENTITYR_HPP

#include "wsf_export.h"

#include "DisRemoveEntityR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisRemoveEntityR : public DisRemoveEntityR
{
public:
   WsfDisRemoveEntityR(WsfDisInterface* aInterfacePtr);
   WsfDisRemoveEntityR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRemoveEntityR() override;

   int  Process() override;
   void TimedOut() override;

protected:
   // Disallow assignment
   WsfDisRemoveEntityR& operator=(const WsfDisRemoveEntityR& aRhs);
   bool                 ProcessRemoveEntityRequest();
   DisPdu*              ProduceResponse(bool aIsDone);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
