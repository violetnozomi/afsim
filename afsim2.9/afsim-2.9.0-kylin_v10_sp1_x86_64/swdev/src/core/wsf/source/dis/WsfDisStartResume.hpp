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

#ifndef WSFDISSTARTRESUME_HPP
#define WSFDISSTARTRESUME_HPP

#include "wsf_export.h"

#include "DisStartResume.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisStartResume : public DisStartResume
{
public:
   WsfDisStartResume(WsfDisInterface* aInterfacePtr);
   WsfDisStartResume(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisStartResume() override;

   int Process() override;

protected:
   // Disallow assignment
   WsfDisStartResume& operator=(const WsfDisStartResume& aRhs);

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
