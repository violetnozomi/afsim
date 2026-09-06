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

#ifndef WSFDISCREATEENTITY_HPP
#define WSFDISCREATEENTITY_HPP

#include "wsf_export.h"

#include "DisCreateEntity.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisCreateEntity : public DisCreateEntity
{
public:
   WsfDisCreateEntity(WsfDisInterface* aInterfacePtr);
   WsfDisCreateEntity(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisCreateEntity() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisCreateEntity& operator=(const WsfDisCreateEntity& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
