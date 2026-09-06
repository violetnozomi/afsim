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

#ifndef WSFDISDATA_HPP
#define WSFDISDATA_HPP

#include "wsf_export.h"

#include "DisData.hpp"

class WsfDisInterface;

class WSF_EXPORT WsfDisData : public DisData
{
public:
   WsfDisData(WsfDisInterface* aInterfacePtr);
   WsfDisData(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisData() override;

   int Process() override;

   void Notify(double aSimTime, DisData* aPduPtr);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
