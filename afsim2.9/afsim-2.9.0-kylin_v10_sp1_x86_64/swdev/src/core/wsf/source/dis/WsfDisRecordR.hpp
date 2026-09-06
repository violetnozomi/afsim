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

#ifndef WSFDISRECORDR_HPP
#define WSFDISRECORDR_HPP

#include "wsf_export.h"

#include "DisRecordR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisRecordR : public DisRecordR
{
public:
   WsfDisRecordR(WsfDisInterface* aInterfacePtr);
   WsfDisRecordR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRecordR() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisRecordR& operator=(const WsfDisRecordR& aRhs);
   bool           ProcessResponseTo(DisPdu* aInitiatorPtr);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
