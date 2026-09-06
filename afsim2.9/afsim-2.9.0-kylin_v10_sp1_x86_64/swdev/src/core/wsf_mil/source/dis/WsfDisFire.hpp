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

#ifndef WSFDISFIRE_HPP
#define WSFDISFIRE_HPP

#include "wsf_mil_export.h"

#include "DisFire.hpp"
#include "UtCallback.hpp"

class WsfDisInterface;
class WsfMilExtInterface;

class WSF_MIL_EXPORT WsfDisFire : public DisFire
{
public:
   WsfDisFire(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisFire() override = default;

   int Process() override;

   void Notify(double aSimTime, DisFire* aPduPtr);

   static UtCallbackListN<void(WsfDisFire&)> ProcessPDU;

private:
   WsfDisInterface*    mInterfacePtr;
   WsfMilExtInterface* mExtMilPtr;
};

#endif
