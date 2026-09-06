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
#include "RvPluginDataP6Dof.hpp"

#include <memory>

#include "RvP6DofDataExtension.hpp"
#include "UtMemory.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvDataP6Dof::Plugin,
                          "P6Dof Data Extension",
                          "P6DOF Movers and their plugins are deprecated in favor of the SixDOF Movers. The P6DOF Data "
                          "Extension plugin enables the processing of messages from the WSF P6DOF Extension. It has "
                          "been replaced by the SixDOF Data Extension plugin.",
                          "mystic")

RvDataP6Dof::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
   rvEnv.RegisterDataExtension(ut::make_unique<rv::P6DofDataExtension>());
}
