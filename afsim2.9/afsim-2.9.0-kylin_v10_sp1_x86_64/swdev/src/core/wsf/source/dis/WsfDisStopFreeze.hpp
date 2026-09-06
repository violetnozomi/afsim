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

#ifndef WSFDISSTOPFREEZE_HPP
#define WSFDISSTOPFREEZE_HPP

#include "wsf_export.h"

#include "DisStopFreeze.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisStopFreeze : public DisStopFreeze
{
public:
   WsfDisStopFreeze(WsfDisInterface* aInterfacePtr);
   WsfDisStopFreeze(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisStopFreeze() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisStopFreeze& operator=(const WsfDisStopFreeze& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
