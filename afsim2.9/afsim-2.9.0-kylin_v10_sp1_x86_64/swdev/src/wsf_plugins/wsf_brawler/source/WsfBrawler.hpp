// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFBRAWLER_HPP
#define WSFBRAWLER_HPP

#include "wsf_brawler_export.h"

#include "WsfScenarioExtension.hpp"

//! An implementation of WSf Scenario Extension that
//! adds replicated BRAWLER capability to an application.
//! @see WsfScenarioExtension
class WSF_BRAWLER_EXPORT WsfBrawler : public WsfScenarioExtension
{
public:
   //! Called when the extension has been added to the scenario
   //! to add script types and wsf_brawler specific types
   void AddedToScenario() override;
};

#endif
