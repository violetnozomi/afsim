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

#ifndef WSFXIO_DRAWREQUEST_HPP
#define WSFXIO_DRAWREQUEST_HPP

#include "wsf_export.h"

#include "WsfDraw.hpp"
#include "xio/WsfXIO_Request.hpp"

//! Requests receipt of WsfXIO_Draw packets.  Allows both reliable and non-reliable transmission.
//! Users must manually create a WsfXIO_Draw packet handler using WsfXIO_Interface::Connect().
class WSF_EXPORT WsfXIO_DrawRequest : public WsfXIO_Request
{
public:
   WsfXIO_DrawRequest(WsfXIO_Connection* aConnectionPtr, bool aIsReliable = true);
   void Initialized() override;

private:
};
#endif
