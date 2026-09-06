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

#ifndef WSFDISRESUPPLYCANCEL_HPP
#define WSFDISRESUPPLYCANCEL_HPP

#include "wsf_export.h"

#include "DisResupplyCancel.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisResupplyCancel : public DisResupplyCancel
{
public:
   WsfDisResupplyCancel(WsfDisInterface* aInterfacePtr);
   WsfDisResupplyCancel(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisResupplyCancel() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisResupplyCancel& operator=(const WsfDisResupplyCancel& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
