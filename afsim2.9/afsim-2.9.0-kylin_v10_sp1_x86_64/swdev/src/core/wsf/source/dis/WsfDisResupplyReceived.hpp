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

#ifndef WSFDISRESUPPLYRECEIVED_HPP
#define WSFDISRESUPPLYRECEIVED_HPP

#include "wsf_export.h"

#include "DisResupplyReceived.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisResupplyReceived : public DisResupplyReceived
{
public:
   WsfDisResupplyReceived(WsfDisInterface* aInterfacePtr);
   WsfDisResupplyReceived(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisResupplyReceived() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisResupplyReceived& operator=(const WsfDisResupplyReceived& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
