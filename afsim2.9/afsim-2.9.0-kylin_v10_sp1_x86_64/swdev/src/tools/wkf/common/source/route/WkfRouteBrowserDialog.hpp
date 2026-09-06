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

#ifndef WKFROUTEBROWSERDIALOG_HPP
#define WKFROUTEBROWSERDIALOG_HPP

#include "wkf_common_export.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QString>
#include <QTreeWidgetItem>

#include "UtQtUnitValueEdit.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfAttachmentRoute.hpp"
#include "WkfDialog.hpp"
#include "WkfRouteBrowserInterface.hpp"
#include "ui_WkfRouteBrowserDialog.h"

namespace wkf
{
class WKF_COMMON_EXPORT RouteBrowserDialog : public QDialog
{
   Q_OBJECT

public:
   RouteBrowserDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~RouteBrowserDialog() override = 0;

   int  GetCurrentWaypoint() const { return mCurrentWaypoint; }
   void SetCurrentWaypoint(int aWaypointIndex) { mCurrentWaypoint = aWaypointIndex; }
   void SelectWaypoint(int aWaypoint);

   const RouteBrowserInterface::RouteInfo& GetRouteInfo() const { return mCurrentRoute; }

   void SetDefaultRoutePrefix(const QString& aDefaultRoutePrefix) { mDefaultRouteNamePrefix = aDefaultRoutePrefix; }
   void SetDefaultWaypointPrefix(const QString& aDefaultWaypointPrefix)
   {
      mWaypointDefaultLabelPrefix = aDefaultWaypointPrefix;
   }

signals:
   void LocalWaypointSelected(const QString& aPlatformName, int aWaypoint);
   void GlobalWaypointSelected(const QString& aCurrRouteName, int aCurrWaypoint);
   void CreateRouteStarted(const QString& aCurrRouteName);
   void CreateRouteTerminated(const QString& aCurRouteName);

   void RouteEdited(const RouteBrowserInterface::RouteInfo& aRouteInfo);
   void GlobalRouteEdited(const RouteBrowserInterface::RouteInfo& aRouteInfo);

   void WaypointAdded(const wkf::RouteBrowserInterface::WaypointInfo& aWaypoint,
                      int                                             aIndex,
                      const QString&                                  aPlatformName,
                      const QString&                                  aRouteName);
   void WaypointRemoved(int aIndex, const QString& aPlatformName, const QString& aRouteName);

   void WaypointLabelEdited(const QString& aPlatformName, const QString& aRouteName, const QString& aLabel, int aIndex);
   void WaypointLatEdited(const QString& aPlatformName, const QString& aRouteName, const UtLatPos& aLat, int aIndex);
   void WaypointLonEdited(const QString& aPlatformName, const QString& aRouteName, const UtLonPos& aLon, int aIndex);
   void WaypointAltEdited(const QString& aPlatformName, const QString& aRouteName, float aAlt, int aIndex);
   void WaypointAglEdited(const QString& aPlatformName, const QString& aRouteName, int aAgl, int aIndex);
   void WaypointGoToEdited(const QString& aPlatformName, const QString& aRouteName, const QString& aGoTo, int aIndex);
   void WaypointSpeedEdited(const QString& aPlatformName, const QString& aRouteName, float aSpeed, int aIndex);
   void WaypointClimbRateEdited(const QString& aPlatformName, const QString& aRouteName, float aClimbRate, int aIndex);
   void WaypointLinAccelEdited(const QString& aPlatformName, const QString& aRouteName, float aLinAccel, int aIndex);
   void WaypointRadAccelEdited(const QString& aPlatformName, const QString& aRouteName, float aRadAccel, int aIndex);
   void WaypointTurnDirEdited(const QString& aPlatformName, const QString& aRouteName, int aTurnDir, int aIndex);
   void WaypointSwitchTypeEdited(const QString& aPlatformName, const QString& aRouteName, int aSwitchType, int aIndex);
   void WaypointEndPathEdited(const QString& aPlatformName, const QString& aRouteName, int aEndOfPath);

protected:
   Ui::WkfRouteBrowserDialog        mUi;
   unsigned int                     mRadIndex;
   RouteBrowserInterface::RouteInfo mCurrentRoute;
   int                              mCurrentWaypoint;
   RouteBrowserInterface::RouteInfo mPreviousRoute;

   QString mDefaultRouteNamePrefix     = "Route-";
   QString mWaypointDefaultLabelPrefix = "Waypoint-";

   virtual bool ApplyChanges() { return false; }
   void         OkClickedHandler();
   virtual void CancelHandler() {}
   virtual void Refresh() {}
   bool         CheckValidWaypointProperties();

   void PopulateTree(const RouteBrowserInterface::RouteInfo& aRoute, int aSelectedIndex = -1);

   QString GetDefaultWaypointLabel(int aIndex);
   void    UpdateDefaultWaypointLabels(RouteBrowserInterface::RouteInfo& aRoute);

   bool IsCurrentRouteGlobal();
   bool IsPreviousRouteGlobal();

   void AppendWaypoint(double aLatitude, double aLongitude, double aAltitude);

   // returns true if non-coincident
   // returns false and removes waypoint 2 if coincident
   //! @retval true if non-coincident
   //! @retval false and removes waypoint 2 if coincident
   bool ResolveCoincidentWaypoints(int aWaypoint1, int aWaypoint2);

   //! Converts the given altitude to the preferred units
   //! @return the converted altitude
   virtual UtLengthValue GetUI_Altitude(const UtLengthValue& aAlt) const;

private:
   bool eventFilter(QObject* aObject, QEvent* aEvent) override;
   void TreeWidgetContextMenu(const QPoint& aPoint);

   void LabelEditingFinishedHandler();
   void LatEditingFinishedHandler();
   void LonEditingFinishedHandler();
   void AltEditingFinishedHandler();
   void AglEditingFinishedHandler();
   void GoToEditingFinishedHandler();
   void SpeedEditingFinishedHandler();
   void ClimbRateEditingFinishedHandler();
   void LinAccelEditingFinishedHandler();
   void ComboBoxChangedHandler(int aIndex);
   void RadAccelEditingFinishedHandler();
   void BankAngleEditingFinishedHandler();
   void MaxGEditingFinishedHandler();
   void AnyRadAccelEditingFinished();
   void TurnDirEditingFinishedHandler();
   void SwitchTypeEditingFinishedHandler();
   void EndPathEditingFinishedHandler();

   void AddWaypointActionHandler();
   void RemoveWaypointActionHandler();
   void RemoveWaypointAtIndex(int aIndex);
   void PopulateWaypointInfo();

   void WaypointModifiedCB(const AttachmentRoute* aRoute, int aWaypointIndex);

   QMessageBox             mDuplicateWarningBox;
   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf

#endif
