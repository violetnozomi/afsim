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
#ifndef RVPLUGINPLATFORMBROWSER_HPP
#define RVPLUGINPLATFORMBROWSER_HPP

#include "RvPlatformBrowserInterface.hpp"
#include "RvPlatformBrowserPrefWidget.hpp"
#include "RvPlugin.hpp"

namespace RvPlatformBrowser
{
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   virtual ~Plugin() = default;

private slots:
   void                    GuiUpdate() override;
   void                    ClearScenario(bool aFullReset) override;
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

private:
   PluginUiPointer<PrefWidget> mPrefWidgetPtr;

   // this will end up belonging to the main window through the base class
   Interface* mPlatformBrowserInterfacePtr;
};
} // namespace RvPlatformBrowser
#endif
