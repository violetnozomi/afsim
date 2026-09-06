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

#ifndef WSFDISEVENTREPORTR_HPP
#define WSFDISEVENTREPORTR_HPP

#include "wsf_export.h"

#include "DisEventReportR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisEventReportR : public DisEventReportR
{
public:
   WsfDisEventReportR(WsfDisInterface* aInterfacePtr);
   WsfDisEventReportR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisEventReportR() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisEventReportR& operator=(const WsfDisEventReportR& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
