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

#ifndef MysticPluginZoneDockWidget_HPP
#define MysticPluginZoneDockWidget_HPP

#include "zone_browser/WkfZoneBrowserDockWidget.hpp"

namespace MysticZoneBrowser
{
class DockWidget : public wkf::ZoneBrowserDockWidget
{
   Q_OBJECT
public:
   DockWidget(wkf::ZoneBrowserDataContainer& aDataContainer, QWidget* aParentPtr = nullptr);

private:
   void             RemovePlatform(const std::string& aPlatformName) override;
   vespa::VaEntity* GetActiveAnchor(const std::string& aReferenceName) override;
};
} // namespace MysticZoneBrowser


#endif
