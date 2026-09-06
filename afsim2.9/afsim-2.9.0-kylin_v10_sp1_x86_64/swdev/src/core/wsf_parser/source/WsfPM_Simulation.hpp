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

#ifndef WSFPM_SIMULATION_HPP
#define WSFPM_SIMULATION_HPP

#include "wsf_parser_export.h"

#include "WsfPProxyNode.hpp"

class WSF_PARSER_EXPORT WsfPM_Simulation : public WsfPProxyNode
{
public:
   WsfPM_Simulation();
   WsfPM_Simulation(const WsfPProxyNode& aNode);

   WsfProxy::Time GetEndTime() const;
   WsfProxy::Time GetMinimumMoverTimeStep() const;
};
#endif
