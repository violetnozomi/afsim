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

#ifndef WSFDISREPAIRRESPONSE_HPP
#define WSFDISREPAIRRESPONSE_HPP

#include "wsf_export.h"

#include "DisRepairResponse.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisRepairResponse : public DisRepairResponse
{
public:
   WsfDisRepairResponse(WsfDisInterface* aInterfacePtr);
   WsfDisRepairResponse(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRepairResponse() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisRepairResponse& operator=(const WsfDisRepairResponse& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
