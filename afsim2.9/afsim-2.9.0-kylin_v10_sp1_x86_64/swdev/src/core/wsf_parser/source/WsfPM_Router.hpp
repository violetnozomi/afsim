// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPM_ROUTER_HPP
#define WSFPM_ROUTER_HPP

#include "wsf_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"
#include "WsfPProxyBasicValues.hpp"

class WSF_PARSER_EXPORT WsfPM_Router : public WsfPM_PlatformPart
{
public:
   WsfPM_Router() {}
   WsfPM_Router(const WsfPProxyNode& aNode)
      : WsfPM_PlatformPart(aNode)
   {
   }

   const std::string Name() const { return GetName(); }
   const std::string Type() const { return GetTypeName(); }

   const std::string GatewayAddress() const { return (*this + "gatewayAddress").GetValue().ValueToString(); }
   const std::string Gateway() const { return (*this + "gateway").GetValue().ValueToString(); }
   const int         HopLimit() const;
   const bool        UseAutomatedInterfaceLinking() const
   {
      return (*this + "automatedInterfaceLinking").GetValue().GetBoolValue();
   }
   const bool UseDefaultProtocol() const { return (*this + "useDefaultProtocol").GetValue().GetBoolValue(); }
   const bool UseMulticastProtocol() const { return (*this + "useMulticastProtocol").GetValue().GetBoolValue(); }
};

using WsfPM_RouterMap = WsfPM_ObjectMapT<WsfPM_Router>;

#endif
