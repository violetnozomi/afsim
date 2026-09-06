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

#ifndef WSFDISDESIGNATOR_HPP
#define WSFDISDESIGNATOR_HPP

#include "wsf_export.h"

#include "DisDesignator.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisDesignator : public DisDesignator
{
public:
   WsfDisDesignator(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisDesignator() override;

   int Process() override;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
