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

#ifndef WSFPM_PROCESSOR_HPP
#define WSFPM_PROCESSOR_HPP

#include "wsf_parser_export.h"

#include "WsfPM_ObjectMap.hpp"
#include "WsfPM_PlatformPart.hpp"

class WsfPM_Processor : public WsfPM_PlatformPart
{
public:
   WsfPM_Processor() {}
   WsfPM_Processor(const WsfPProxyNode& aNode)
      : WsfPM_PlatformPart(aNode)
   {
   }
};

using WsfPM_ProcessorMap = WsfPM_ObjectMapT<WsfPM_Processor>;

#endif // WSFPM_PROCESSOR_HPP
