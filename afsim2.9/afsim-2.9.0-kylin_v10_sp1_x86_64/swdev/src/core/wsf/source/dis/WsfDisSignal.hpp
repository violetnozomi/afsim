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

#ifndef WSFDISSIGNAL_HPP
#define WSFDISSIGNAL_HPP

#include "wsf_export.h"

#include "DisSignal.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisSignal : public DisSignal
{
public:
   WsfDisSignal(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisSignal() override;

   int Process() override;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
