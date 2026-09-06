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

#ifndef WSFDISDETONATION_HPP
#define WSFDISDETONATION_HPP

#include "wsf_mil_export.h"

#include "DisDetonation.hpp"
#include "UtCallback.hpp"
class UtInput;

class WsfDisInterface;
class WsfPlatform;
#include "WsfStringId.hpp"
class WsfWeaponEngagement;

class WSF_MIL_EXPORT WsfDisDetonation : public DisDetonation
{
public:
   WsfDisDetonation(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisDetonation() override = default;

   int Process() override;

   static UtCallbackListN<void(WsfDisDetonation&)> ProcessPDU;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
