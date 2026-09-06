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

#ifndef WSFDISEVENTREPORT_HPP
#define WSFDISEVENTREPORT_HPP

#include "wsf_export.h"

#include "DisEventReport.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisEventReport : public DisEventReport
{
public:
   WsfDisEventReport(WsfDisInterface* aInterfacePtr);
   WsfDisEventReport(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisEventReport() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisEventReport& operator=(const WsfDisEventReport& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
