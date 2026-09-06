// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPM_COMMLINK_HPP
#define WSFPM_COMMLINK_HPP

#include "WsfPM_ObjectList.hpp"
#include "WsfPProxyBasicValues.hpp"
#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_CommLink : public WsfPProxyNode
{
public:
   WsfPM_CommLink() = default;
   WsfPM_CommLink(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode){};

   std::string LinkDestinationPlatformName() const { return GetValue().GetAttr("linkPlatform").ValueToString(); }
   std::string LinkDestinationCommName() const { return GetValue().GetAttr("linkComm").ValueToString(); }
};

using WsfPM_CommLinkList = WsfPM_ObjectList<WsfPM_CommLink>;
#endif
