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

#ifndef WSFPM_PLATFORMPART_HPP
#define WSFPM_PLATFORMPART_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_PlatformPart : public WsfPProxyNode
{
public:
   WsfPM_PlatformPart();
   WsfPM_PlatformPart(const WsfPProxyNode& aNode);

   bool on();
   void setOn(bool on);

   bool operational();
   void setOperational(bool operational);

   bool restorable();
   void setRestorable(bool restorable);
};

#endif // WSFPM_PLATFORMPART_HPP
