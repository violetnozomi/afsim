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

#ifndef WSFDISCREATEENTITYR_HPP
#define WSFDISCREATEENTITYR_HPP

#include "wsf_export.h"

#include "DisCreateEntityR.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisCreateEntityR : public DisCreateEntityR
{
public:
   WsfDisCreateEntityR(WsfDisInterface* aInterfacePtr);
   WsfDisCreateEntityR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisCreateEntityR() override;

   void TimedOut() override;

protected:
   // Disallow assignment
   WsfDisCreateEntityR& operator=(const WsfDisCreateEntityR& aRhs) = delete;
   bool                 ProcessCreateEntityRequest();
   DisPdu*              ProduceResponse(bool aIsDone) const;

private:
   WsfDisInterface* mInterfacePtr;
};

#endif
