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

#ifndef WKFROUTEBROWSEREDITDIALOG_HPP
#define WKFROUTEBROWSEREDITDIALOG_HPP

#include "wkf_common_export.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QTreeWidgetItem>

#include "UtQtUnitValueEdit.hpp"
#include "WkfDialog.hpp"
#include "WkfRouteBrowserDialog.hpp"
#include "WkfRouteBrowserInterface.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT RouteBrowserEditDialog : public RouteBrowserDialog
{
   Q_OBJECT

public:
   RouteBrowserEditDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   virtual ~RouteBrowserEditDialog() override;

   void           PopulateDialog(const RouteBrowserInterface::RouteInfo& aRouteInfo);
   void           EnableButtons();
   void           DisableButtons();
   const QString& GetCurrentPlatform() const { return mCurrentRoute.mPlatformName; }

signals:
   void RequestRouteInfo(const QString& aName);
   void Apply(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo);
   void RoutePopulated(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo,
                       const wkf::RouteBrowserInterface::RouteInfo& aPrevRouteInfo);
   void EditDialogCancelled(const wkf::RouteBrowserInterface::RouteInfo& aRouteInfo);


protected:
   virtual void MoveSelectionAndAttachmentComplete(const wkf::PlatformList&       aPlatformList,
                                                   const wkf::EntityList&         aNonPlatformList,
                                                   const vespa::VaAttachmentList& aAttachmentList);

private:
   bool ApplyChanges() override;
   void Refresh() override;
   void CancelHandler() override;

   void PopulateDescription();

   QLabel* mRouteDescription;
};
} // namespace wkf
#endif
