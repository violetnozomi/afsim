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

#ifndef WSFDISSERVICEREQUEST_HPP
#define WSFDISSERVICEREQUEST_HPP

#include "wsf_export.h"

#include "DisServiceRequest.hpp"
class WsfDisInterface;

class WSF_EXPORT WsfDisServiceRequest : public DisServiceRequest
{
public:
   WsfDisServiceRequest(WsfDisInterface* aInterfacePtr);
   WsfDisServiceRequest(const DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr);
   ~WsfDisServiceRequest() override;

   int Process() override;

private:
   // Disallow assignment
   WsfDisServiceRequest& operator=(const WsfDisServiceRequest& aRhs);

   WsfDisInterface* mInterfacePtr;
};

#endif
