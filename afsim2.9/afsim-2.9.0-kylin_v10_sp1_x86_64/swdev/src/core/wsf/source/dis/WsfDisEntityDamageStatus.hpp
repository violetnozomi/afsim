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

#ifndef WSFDISENTITYDAMAGESTATUS_HPP
#define WSFDISENTITYDAMAGESTATUS_HPP

#include "wsf_export.h"

#include "DisEntityDamageStatus.hpp"
class GenI;
class WsfDisInterface;

class WSF_EXPORT WsfDisEntityDamageStatus : public DisEntityDamageStatus
{
public:
   WsfDisEntityDamageStatus(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisEntityDamageStatus() override = default;

   int Process() override;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
