// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERAPPLICATIONEXTENSION_HPP
#define WSFCYBERAPPLICATIONEXTENSION_HPP

#include "wsf_cyber_export.h"

#include "WsfApplicationExtension.hpp"

namespace wsf
{

namespace cyber
{

//! The WSF application extension for cyber.
class WSF_CYBER_EXPORT ApplicationExtension : public WsfApplicationExtension
{
public:
   ~ApplicationExtension() override = default;
   void ScenarioCreated(WsfScenario& aScenario) override;
   void AddedToApplication(WsfApplication& aApplication) override;
};

} // namespace cyber

} // namespace wsf

#endif
