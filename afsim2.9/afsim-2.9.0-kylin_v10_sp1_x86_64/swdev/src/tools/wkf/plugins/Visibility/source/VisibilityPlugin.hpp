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

#ifndef VISIBILITYPLUGIN_HPP
#define VISIBILITYPLUGIN_HPP

#include "WkfPlugin.hpp"
#include "visibility/WkfTypeVisibilityPrefWidget.hpp"
#include "visibility/WkfVisibilityDockWidget.hpp"

namespace Visibility
{
class Plugin : public wkf::Plugin
{
   Q_OBJECT
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

   ~Plugin() override = default;

   void GuiUpdate() override;

   // Adds Visibility context menu, with buttons to hide selected, hide unselected, and show all
   void BuildViewerContextMenu(QMenu* aMenuPtr, vespa::VaViewer* aViewerPtr) override;

   // Sets any selected platform's invisibility to true
   void HideSelected();

   // Sets any unselected platform's invisibility to true
   void HideUnselected();

   // Sets all platforms' invisibilities to false
   void ShowAll();

   // Returns true if a given platform is NOT invisible
   bool IsVisible(size_t index);

private:
   wkf::VisibilityDockWidget* mDockWidgetPtr{nullptr};
   std::map<size_t, bool>     mIsInvisible;
   vespa::VaCallbackHolder    mCallbacks;
};
} // namespace Visibility
#endif
