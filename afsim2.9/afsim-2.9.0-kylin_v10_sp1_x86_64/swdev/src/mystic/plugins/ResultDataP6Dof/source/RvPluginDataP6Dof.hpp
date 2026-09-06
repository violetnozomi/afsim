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
#ifndef RVPLUGINDATAP6DOF_HPP
#define RVPLUGINDATAP6DOF_HPP

#include "RvPlugin.hpp"

namespace RvDataP6Dof
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;
};
} // namespace RvDataP6Dof
#endif
