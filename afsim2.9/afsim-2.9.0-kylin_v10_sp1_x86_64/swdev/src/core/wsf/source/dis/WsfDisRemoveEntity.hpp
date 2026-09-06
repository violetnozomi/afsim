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

#ifndef WSFDISREMOVEENTITY_HPP
#define WSFDISREMOVEENTITY_HPP

#include "wsf_export.h"

#include "DisRemoveEntity.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisRemoveEntity : public DisRemoveEntity
{
public:
   WsfDisRemoveEntity(WsfDisInterface* aInterfacePtr);
   WsfDisRemoveEntity(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisRemoveEntity() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisRemoveEntity& operator=(const WsfDisRemoveEntity& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
