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

#ifndef WSFDISRECEIVER_HPP
#define WSFDISRECEIVER_HPP

#include "wsf_export.h"

#include "DisReceiver.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisReceiver : public DisReceiver
{
public:
   WsfDisReceiver(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisReceiver() override;

   int Process() override;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
