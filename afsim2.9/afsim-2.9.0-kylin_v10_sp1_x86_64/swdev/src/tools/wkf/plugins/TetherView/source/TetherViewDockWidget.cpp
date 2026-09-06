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
#include "TetherViewDockWidget.hpp"

#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QVBoxLayout>

#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "VaAttachmentVectors.hpp"
#include "VaEntity.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

TetherView::DockWidget::DockWidget(const QString&  aObjectName,
                                   const QString&  aPlatformName,
                                   QMainWindow*    parent,
                                   Qt::WindowFlags flags)
   : wkf::ImmersiveViewDockWidget(aObjectName, aPlatformName, parent, flags)
{
   CompleteConstruction();
}

void TetherView::DockWidget::BuildContextMenuPrivate(QMenu& aMenu, const QPoint& pos)
{
   wkf::Action* resetCameraPtr = new wkf::Action(QIcon::fromTheme("reset"), "Reset Camera", &aMenu);
   connect(resetCameraPtr, &QAction::triggered, this, &DockWidget::ResetCamera);
   aMenu.addAction(resetCameraPtr);
}

void TetherView::DockWidget::closeEvent(QCloseEvent* event)
{
   // Tether views should be deleted when they are closed, and recreated on demand
   deleteLater();
}
