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

#ifndef WSFDISRESUPPLYOFFER_HPP
#define WSFDISRESUPPLYOFFER_HPP

#include "wsf_export.h"

#include "DisResupplyOffer.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisResupplyOffer : public DisResupplyOffer
{
public:
   WsfDisResupplyOffer(WsfDisInterface* aInterfacePtr);
   WsfDisResupplyOffer(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisResupplyOffer() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisResupplyOffer& operator=(const WsfDisResupplyOffer& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
