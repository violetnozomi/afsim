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

#include "WsfPM_MilRoot.hpp"

class WsfApplication;
#include "wsf_mil_parser_export.h"

#include "WsfPProxy.hpp"

//
// An extension which parses the grammar in WSF.
//
void WSF_MIL_PARSER_EXPORT Register_wsf_mil_parser(WsfApplication& aApplication)
{
   // aApplication.RegisterExtension("wsf_parser", new WsfDefaultApplicationExtension<WsfParserExtension>());
}

WsfPM_MilRoot::WsfPM_MilRoot() {}

WsfPM_MilRoot::WsfPM_MilRoot(WsfPProxyNode aRootNode)
   : WsfPM_Root(aRootNode)
{
}

WsfPM_MilRoot::WsfPM_MilRoot(WsfPProxy* aProxy)
   : WsfPM_Root(aProxy)
{
}
