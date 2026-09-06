// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISTRANSFEROWNER_HPP
#define WSFDISTRANSFEROWNER_HPP

#include "wsf_export.h"

#include "DisTransferOwnership.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisTransferOwnership : public DisTransferOwnership
{
public:
   WsfDisTransferOwnership(WsfDisInterface* aInterfacePtr);
   WsfDisTransferOwnership(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);

   ~WsfDisTransferOwnership() override = default;

   WsfDisTransferOwnership& operator=(const WsfDisTransferOwnership& aRhs) = delete;

   int Process() override;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
