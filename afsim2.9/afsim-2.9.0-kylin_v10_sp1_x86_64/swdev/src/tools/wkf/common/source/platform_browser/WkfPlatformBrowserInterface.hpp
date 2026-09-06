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

#ifndef WKFPLATFORMBROWSERINTERFACE_HPP
#define WKFPLATFORMBROWSERINTERFACE_HPP

#include "wkf_common_export.h"

#include <QList>
#include <QMutex>
#include <QObject>
#include <QTreeWidgetItem>

#include "VaCallbackHolder.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformBrowserDockWidget.hpp"

namespace wkf
{
class TeamVisibilityPrefObject;

class WKF_COMMON_EXPORT PlatformBrowserInterface : public QObject
{
   Q_OBJECT
public:
   explicit PlatformBrowserInterface(QWidget* aParent);

   PlatformBrowserDockWidget* GetDockWidget() const { return mDockWidget; }

protected:
   PlatformBrowserDockWidget* mDockWidget;

private:
   void ContextMenuRequestedHandler(const QPoint& aPoint, const QString& aText);
   void SelectedPlatformChangedHandler(const QStringList& aNames);
   void PlatformSelectionChangedHandler(const PlatformList& aSelectedPlatforms, const PlatformList& aUnselectedPlatforms);

   vespa::VaCallbackHolder mCallbacks;

   // When a user selects a platform in the treeWidget, it sets the platform in focus in the WkfEnvironment,
   // When a platform is set in Focus in the WkfEnvironment, it sets the platform selected in the treeWidget,
   // To prevent an infinite loop from occurring, use a mutex to prevent processing when necessary.
   QMutex mEntitySelectionMutex;
};
} // namespace wkf
#endif
