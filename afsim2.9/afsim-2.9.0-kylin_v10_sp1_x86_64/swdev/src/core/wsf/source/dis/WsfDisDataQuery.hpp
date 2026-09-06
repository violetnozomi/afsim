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

#ifndef WSFDISDATAQUERY_HPP
#define WSFDISDATAQUERY_HPP

#include "wsf_export.h"

#include "DisDataQuery.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisDataQuery : public DisDataQuery
{
public:
   WsfDisDataQuery(WsfDisInterface* aInterfacePtr);
   WsfDisDataQuery(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisDataQuery() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisDataQuery& operator=(const WsfDisDataQuery& aRhs);
   bool             TakeAction();

   WsfDisInterface* mInterfacePtr;
};

#endif
