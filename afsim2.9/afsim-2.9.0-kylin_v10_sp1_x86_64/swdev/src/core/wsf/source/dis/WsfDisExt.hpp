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

#ifndef WSFDISEXT_HPP
#define WSFDISEXT_HPP

#include "wsf_export.h"

#include "DisEntityId.hpp"
#include "DisEntityType.hpp"
#include "ext/WsfExtEntityId.hpp"
#include "ext/WsfExtEntityType.hpp"
#include "ext/WsfExtInterface.hpp"

//! WsfDisExt provides methods assisting DIS in interacting with WsfExt files
class WSF_EXPORT WsfDisExt
{
public:
   static DisEntityType    ToDis(const WsfExtEntityType& aEntityType);
   static WsfExtEntityType ToExt(const DisEntityType& aEntityType);
   static DisEntityId      ToDis(const WsfExtEntityId& aEntityId);
   static WsfExtEntityId   ToExt(const DisEntityId& aEntityId);
};

#endif
