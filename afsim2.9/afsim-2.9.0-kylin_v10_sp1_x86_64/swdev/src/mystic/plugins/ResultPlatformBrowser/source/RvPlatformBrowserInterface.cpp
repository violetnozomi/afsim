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

#include "RvPlatformBrowserInterface.hpp"

#include <QMenu>
#include <QMessageBox>

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"
#include "platform_browser/WkfPlatformBrowserDockWidget.hpp"
#include "platform_browser/WkfPlatformBrowserInterface.hpp"

RvPlatformBrowser::Interface::Interface()
   : wkf::PlatformBrowserInterface(wkfEnv.GetMainWindow())
{
}

void RvPlatformBrowser::Interface::Update()
{
   mDockWidget->UpdateInfo();
}

void RvPlatformBrowser::Interface::ClearScenario()
{
   mDockWidget->Reset();
}

void RvPlatformBrowser::Interface::ShowInactiveChanged(bool aState)
{
   mDockWidget->ShowInactive(aState);
}
