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

#ifndef WSFPM_TRANSMITTER_HPP
#define WSFPM_TRANSMITTER_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyNode.hpp"

class UtAzElTable;
class WsfPProxyIndex;

class WSF_PARSER_EXPORT WsfPM_Transmitter : public WsfPProxyNode
{
public:
   WsfPM_Transmitter() {}
   WsfPM_Transmitter(const WsfPProxyNode& aNode)
      : WsfPProxyNode(aNode)
   {
   }

   WsfProxy::Power*     Power() const;
   WsfProxy::Length*    Wavelength() const;
   WsfProxy::Frequency* Frequency() const;
   WsfProxy::Frequency* Bandwidth() const;
   std::string          Polarization() const;
   WsfProxy::Ratio*     InternalLoss() const;
   WsfProxy::Ratio*     PulseCompressionRatio() const;
   WsfProxy::Time*      PulseWidth() const;
};

#endif
