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

#ifndef WSFPM_COMM_HPP
#define WSFPM_COMM_HPP

#include "wsf_parser_export.h"

#include "WsfPM_CommLink.hpp"
#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"
#include "WsfPProxyBasicValues.hpp"

class WSF_PARSER_EXPORT WsfPM_Comm : public WsfPM_PlatformPart
{
public:
   WsfPM_Comm() {}
   WsfPM_Comm(const WsfPProxyNode& aNode)
      : WsfPM_PlatformPart(aNode)
   {
   }

   WsfProxy::String  GetNetworkName();
   const std::string GetNetworkNameString() const { return (*this + "networkName").GetValue().ValueToString(); }
   const std::string Address() const { return (*this + "address").GetValue().ValueToString(); }
   const std::string NetworkAddress() const { return (*this + "networkAddress").GetValue().ValueToString(); }
   const std::string Name() const { return GetName(); }
   const std::string Type() const { return GetValue().GetType()->GetTypeName(); }
   const std::string LinkPlatform() const { return (*this + "linkPlatform").GetValue().ValueToString(); }
   const std::string LinkComm() const { return (*this + "linkComm").GetValue().ValueToString(); }
   const std::string LocalLink() const { return (*this + "localLink").GetValue().ValueToString(); }
   const std::string LinkAddress() const { return (*this + "linkAddress").GetValue().ValueToString(); }
   const std::string RouterName() const { return (*this + "routerName").GetValue().ValueToString(); }
   const std::string GatewayPlatform() const { return (*this + "gatewayPlatform").GetValue().ValueToString(); }
   const std::string GatewayComm() const { return (*this + "gatewayComm").GetValue().ValueToString(); }

   size_t         LinksCount() const { return LinksList().size(); }
   WsfPM_CommLink GetLink(size_t aIndex) const;

   WsfPM_CommLinkList LinksList() const { return *this + "commLinks"; }
};

using WsfPM_CommMap = WsfPM_ObjectMapT<WsfPM_Comm>;

#endif
