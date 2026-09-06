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
#ifndef TETHERVIEWDOCKWIDGET_HPP
#define TETHERVIEWDOCKWIDGET_HPP

#include <QString>

#include "VaCallbackHolder.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "misc/WkfImmersiveViewDockWidget.hpp"

namespace vespa
{
class VaAttachmentVectors;
class VaEntity;
class VaScenario;
class VaViewer;
} // namespace vespa

namespace TetherView
{
class DockWidget : public wkf::ImmersiveViewDockWidget
{
   Q_OBJECT

public:
   DockWidget(const QString&  aObjectName,
              const QString&  aPlatformName,
              QMainWindow*    parent = nullptr,
              Qt::WindowFlags flags  = Qt::WindowFlags());
   ~DockWidget() override = default;

protected:
   virtual void BuildContextMenuPrivate(QMenu& aMenu, const QPoint& pos) override;
   virtual void closeEvent(QCloseEvent* event) override;

private:
};
} // namespace TetherView
#endif
