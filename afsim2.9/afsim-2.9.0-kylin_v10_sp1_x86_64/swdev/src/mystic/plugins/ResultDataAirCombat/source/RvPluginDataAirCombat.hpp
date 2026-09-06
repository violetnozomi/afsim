// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVPLUGINDATAAIRCOMBAT_HPP
#define RVPLUGINDATAAIRCOMBAT_HPP

#include "RvPlugin.hpp"

namespace RvDataAirCombat
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;
};
} // namespace RvDataAirCombat

#endif // !RVPLUGINDATAAIRCOMBAT_HPP
