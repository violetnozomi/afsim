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

#ifndef WSFDISREPAIRCOMPLETE_HPP
#define WSFDISREPAIRCOMPLETE_HPP

#include "wsf_export.h"

#include "DisRepairComplete.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisRepairComplete : public DisRepairComplete
{
public:
   WsfDisRepairComplete(WsfDisInterface* aInterfacePtr);
   WsfDisRepairComplete(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRepairComplete() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisRepairComplete& operator=(const WsfDisRepairComplete& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
