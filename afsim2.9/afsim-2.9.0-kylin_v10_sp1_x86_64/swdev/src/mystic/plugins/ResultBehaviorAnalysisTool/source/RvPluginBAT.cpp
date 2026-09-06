// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvPluginBAT.hpp"

#include <QMenu>

#include "RvBAT_DockWindow.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvBAT::Plugin,
                          "Behavior Analysis Tool",
                          "Displays Advanced Behavior Trees and Finite State Machines for platforms.",
                          "mystic")

RvBAT::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   if (nullptr != mainWindowPtr)
   {
      mDockWindowPtr = new DockWindow("BAT_DockWidget", mainWindowPtr->centralWidget(), Qt::WindowFlags());
      mDockWindowPtr->hide();
      mDockWindowPtr->SetNeedsUpdate(true);

      wkfEnv.GetMainWindow()->AddDockWidgetToViewMenu(mDockWindowPtr);
   }
}

void RvBAT::Plugin::ClearScenario(bool aFullReset)
{
   mDockWindowPtr->Reset();
}

void RvBAT::Plugin::GuiUpdate()
{
   if (mDockWindowPtr->isVisible())
   {
      auto data = rvEnv.GetData();
      if (data != nullptr)
      {
         mDockWindowPtr->Update(*data);
      }
   }
}
