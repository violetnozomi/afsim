// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MODELBROWSERPLUGIN_HPP
#define MODELBROWSERPLUGIN_HPP

#include "WkfDockWidget.hpp"
#include "WkfPlugin.hpp"

namespace ModelBrowser
{
class Plugin : public wkf::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::Tip> GetTips() const override;

private:
   void             Launch();
   void             Reload();
   wkf::DockWidget* mDockWidget;
};
} // namespace ModelBrowser
#endif
