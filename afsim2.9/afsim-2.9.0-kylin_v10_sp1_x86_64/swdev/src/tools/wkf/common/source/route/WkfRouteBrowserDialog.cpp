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

#include "WkfRouteBrowserDialog.hpp"

#include <unordered_set>

#include <QKeyEvent>
#include <QMessageBox>

#include "UtCast.hpp"
#include "UtEarth.hpp"
#include "WkfAttachmentRoute.hpp"
#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfUnitsObject.hpp"


wkf::RouteBrowserDialog::RouteBrowserDialog(QWidget* aParent /*= 0*/, Qt::WindowFlags aFlags /*= 0*/)
   : QDialog(aParent, aFlags)
   , mRadIndex(0)
   , mCurrentWaypoint(-1)
   , mDuplicateWarningBox(
        QMessageBox::Warning,
        "Coincident Waypoints",
        "This waypoint is coincident with an adjacent waypoint. One of the waypoints will be removed.",
        QMessageBox::Ok,
        this)
{
   mUi.setupUi(this);

   int count = mUi.waypointPropertiesSheet->count();
   for (int i = 0; i < count; ++i)
   {
      QWidget* widget = mUi.waypointPropertiesSheet->itemAt(i)->widget();
      if (widget)
      {
         widget->setEnabled(false);
      }
   }

   // Hide the apply button in the base class, require the derived classes to make it visible.
   mUi.buttonBox->button(QDialogButtonBox::Apply)->setVisible(false);

   mUi.mChangesLabel->setVisible(false);

   // Whenever a button is selected cancel the location selector if it hasn't been canceled already
   connect(mUi.buttonBox->button(QDialogButtonBox::Ok),
           &QPushButton::clicked,
           this,
           [this]
           {
              mUi.mLocationSelector->CancelSelection();
              OkClickedHandler();
           });
   connect(mUi.buttonBox->button(QDialogButtonBox::Apply),
           &QPushButton::clicked,
           this,
           [this]
           {
              mUi.mLocationSelector->CancelSelection();
              if (ApplyChanges())
              {
                 Refresh();
              }
           });
   connect(mUi.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, [this] { close(); });
   connect(this,
           &QDialog::rejected,
           this,
           [this]
           {
              mUi.mLocationSelector->CancelSelection();
              CancelHandler();
           });

   connect(mUi.treeWidget, &QTreeWidget::itemSelectionChanged, this, &RouteBrowserDialog::PopulateWaypointInfo);
   connect(mUi.treeWidget,
           &QTreeWidget::itemClicked,
           this,
           [this]()
           {
              // Notify subscribers that a different waypoint was selected
              if (IsCurrentRouteGlobal())
              {
                 emit GlobalWaypointSelected(mCurrentRoute.mName, mCurrentWaypoint);
              }
              else
              {
                 emit LocalWaypointSelected(mCurrentRoute.mPlatformName, mCurrentWaypoint);
              }
           });

   connect(mUi.mLabelEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::LabelEditingFinishedHandler);
   connect(mUi.mLatEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::LatEditingFinishedHandler);
   connect(mUi.mLonEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::LonEditingFinishedHandler);
   connect(mUi.mAltEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::AltEditingFinishedHandler);
   connect(mUi.mAltRefEdit,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::AglEditingFinishedHandler);

   connect(mUi.mGoToEdit,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::GoToEditingFinishedHandler);

   connect(mUi.mSpeedEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::SpeedEditingFinishedHandler);
   connect(mUi.mClimbRateEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::ClimbRateEditingFinishedHandler);
   connect(mUi.mLinAccelEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::LinAccelEditingFinishedHandler);
   connect(mUi.mRadAccelEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::RadAccelEditingFinishedHandler);
   connect(mUi.mBankAngleEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::BankAngleEditingFinishedHandler);
   connect(mUi.mMaxGEdit, &QLineEdit::editingFinished, this, &RouteBrowserDialog::MaxGEditingFinishedHandler);
   connect(mUi.mRadAccelComboBox,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::ComboBoxChangedHandler);

   connect(mUi.mTurnDirEdit,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::TurnDirEditingFinishedHandler);

   connect(mUi.mSwitchTypeEdit,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::SwitchTypeEditingFinishedHandler);

   connect(mUi.mEndPathEdit,
           static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
           this,
           &RouteBrowserDialog::EndPathEditingFinishedHandler);

   mUi.mLocationSelector->SetMultiSelectionEnabled(true);
   connect(mUi.mLocationSelector, &LocationSelector::Selected, this, &RouteBrowserDialog::AppendWaypoint);
   connect(mUi.mLocationSelector,
           &LocationSelector::SelectionStarted,
           this,
           [this]() { emit CreateRouteStarted(mCurrentRoute.mName); });

   mUi.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
   mUi.treeWidget->installEventFilter(this);
   connect(mUi.treeWidget, &QTreeWidget::customContextMenuRequested, this, &RouteBrowserDialog::TreeWidgetContextMenu);

   mCallbacks.Add(Observer::WaypointMoved.Connect(&RouteBrowserDialog::WaypointModifiedCB, this));

   connect(wkfEnv.GetPreferenceObject<wkf::UnitsObject>(),
           &UnitsObject::UnitsChanged,
           this,
           [this]() { PopulateWaypointInfo(); });
}

wkf::RouteBrowserDialog::~RouteBrowserDialog() = default;

void wkf::RouteBrowserDialog::SelectWaypoint(int aWaypoint)
{
   QTreeWidgetItem* root = mUi.treeWidget->invisibleRootItem();
   if (aWaypoint >= 0 && aWaypoint < root->childCount())
   {
      mUi.treeWidget->setCurrentItem(root->child(aWaypoint));
   }
}

void wkf::RouteBrowserDialog::OkClickedHandler()
{
   if (ApplyChanges())
   {
      setResult(QDialog::Accepted);
      close();
   }
}

bool wkf::RouteBrowserDialog::CheckValidWaypointProperties()
{
   bool radAccelValid = false;
   if (mRadIndex == 0)
   {
      radAccelValid = (mUi.mRadAccelEdit->IsValid() || mUi.mRadAccelEdit->text().isEmpty());
   }
   else if (mRadIndex == 1)
   {
      radAccelValid = (mUi.mBankAngleEdit->IsValid() || mUi.mBankAngleEdit->text().isEmpty());
   }
   else if (mRadIndex == 2)
   {
      radAccelValid = (mUi.mMaxGEdit->IsValid() || mUi.mMaxGEdit->text().isEmpty());
   }

   if ((mUi.mLatEdit->IsValid() || mUi.mLatEdit->text().isEmpty()) &&
       (mUi.mLonEdit->IsValid() || mUi.mLonEdit->text().isEmpty()) &&
       (mUi.mAltEdit->IsValid() || mUi.mAltEdit->text().isEmpty()) &&
       (mUi.mSpeedEdit->IsValid() || mUi.mSpeedEdit->text().isEmpty()) &&
       (mUi.mClimbRateEdit->IsValid() || mUi.mClimbRateEdit->text().isEmpty()) &&
       (mUi.mLinAccelEdit->IsValid() || mUi.mLinAccelEdit->text().isEmpty()) && radAccelValid)
   {
      return true;
   }

   QMessageBox::warning(
      this,
      "Invalid Route",
      "One or more waypoint properties are invalid. Make sure all fields are valid and have the correct units.");
   return false;
}

void wkf::RouteBrowserDialog::PopulateTree(const RouteBrowserInterface::RouteInfo& aRoute, int aSelectedIndex)
{
   mUi.treeWidget->clear();
   for (int i = 0; i < aRoute.mWaypoints.size(); ++i)
   {
      QString        wyptName = "Waypoint " + QString::number(i);
      const QString& label    = aRoute.mWaypoints.at(i).mLabel;
      if (label != "")
      {
         wyptName = wyptName + " (" + label + ")";
      }

      QTreeWidgetItem* wyptItem = new QTreeWidgetItem(QStringList() << wyptName);
      mUi.treeWidget->addTopLevelItem(wyptItem);

      if (i == aSelectedIndex)
      {
         mUi.treeWidget->setCurrentItem(wyptItem);
         wyptItem->setSelected(true);
      }
   }
}

QString wkf::RouteBrowserDialog::GetDefaultWaypointLabel(int aIndex)
{
   if (!mWaypointDefaultLabelPrefix.isEmpty())
   {
      return mWaypointDefaultLabelPrefix + QString::number(aIndex);
   }
   return "";
}

void wkf::RouteBrowserDialog::UpdateDefaultWaypointLabels(RouteBrowserInterface::RouteInfo& aRoute)
{
   // Store the waypoint that have had their mGoto label updated already,
   // We don't want a waypoints label to be updated multiple times because it may lead to incorrect behavior
   // For example: A waypoints goto label may change from Label1 to Label2 and then when Label2 gets updated
   //              it may change to Label3, which is incorrect.
   std::unordered_set<int> modifiedWaypoints;

   int index1 = 0;
   for (auto& wypt1 : aRoute.mWaypoints)
   {
      // Update all waypoint label that are the default label.
      // default labels are any label that starts with "Waypoint-"
      if (wypt1.mLabel.startsWith(mWaypointDefaultLabelPrefix))
      {
         QString newLabel = GetDefaultWaypointLabel(index1);
         // If the label is about to be changed, we need to go update any other waypoints that may be referencing this
         // waypoint in a goto statement
         if (wypt1.mLabel != newLabel)
         {
            int index2 = 0;
            // Check to see if any waypoints have a goto value to the current waypoint's label
            for (auto& wypt2 : aRoute.mWaypoints)
            {
               // Check to see if this waypoint has been updated already, and that the goto label matching the waypoint
               // that is changing
               if (modifiedWaypoints.count(index2) == 0 && wypt2.mGoTo == wypt1.mLabel)
               {
                  wypt2.mGoTo = newLabel;
                  modifiedWaypoints.insert(index2);
               }
               index2++;
            }
         }
         wypt1.mLabel = newLabel;
      }
      index1++;
   }
}

bool wkf::RouteBrowserDialog::IsCurrentRouteGlobal()
{
   // If the platform name is empty, it is a global route.
   return mCurrentRoute.mPlatformName.isEmpty();
}

bool wkf::RouteBrowserDialog::IsPreviousRouteGlobal()
{
   // If the platform name is empty, it is a global route.
   return mPreviousRoute.mPlatformName.isEmpty();
}

bool wkf::RouteBrowserDialog::eventFilter(QObject* aObject, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::KeyPress)
   {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(aEvent);
      if (keyEvent->key() == Qt::Key_Delete)
      {
         // Don't delete item from the tree id there is only 1 item left
         if (mUi.treeWidget->topLevelItemCount() > 1)
         {
            QTreeWidgetItem* item = mUi.treeWidget->currentItem();
            if (item)
            {
               RemoveWaypointAtIndex(mUi.treeWidget->indexOfTopLevelItem(item));
               return true;
            }
         }
         else
         {
            QMessageBox::information(this, "Empty Route", "Unable to delete last waypoint because routes can not be empty.");
         }
      }
   }
   return false;
}

void wkf::RouteBrowserDialog::TreeWidgetContextMenu(const QPoint& aPoint)
{
   QTreeWidgetItem* item = mUi.treeWidget->itemAt(aPoint);
   if (item)
   {
      int currentIndex = mUi.treeWidget->indexOfTopLevelItem(item);

      QMenu* menu = new QMenu();
      // Build the context menu
      QAction* insertBefore = new QAction("Insert Waypoint Prior", menu);
      insertBefore->setData(currentIndex);
      connect(insertBefore, &QAction::triggered, this, &RouteBrowserDialog::AddWaypointActionHandler);
      menu->addAction(insertBefore);

      QAction* insertAfter = new QAction("Insert Waypoint After", menu);
      insertAfter->setData(currentIndex + 1);
      connect(insertAfter, &QAction::triggered, this, &RouteBrowserDialog::AddWaypointActionHandler);
      menu->addAction(insertAfter);

      // Do not allow users to delete the last waypoint, which would make an invalid route.
      if (mCurrentRoute.mWaypoints.size() > 1)
      {
         QAction* deleteAction = new QAction("Remove Waypoint", menu);
         deleteAction->setData(currentIndex);
         connect(deleteAction, &QAction::triggered, this, &RouteBrowserDialog::RemoveWaypointActionHandler);
         menu->addAction(deleteAction);
      }

      menu->exec(mapToGlobal(aPoint));

      delete menu;
   }
}

void wkf::RouteBrowserDialog::LabelEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      mCurrentRoute.mWaypoints[mCurrentWaypoint].mLabel = mUi.mLabelEdit->text();
      PopulateTree(mCurrentRoute, mCurrentWaypoint);
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::LatEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      if (mUi.mLatEdit->IsValid() || mUi.mLatEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mLat = mUi.mLatEdit->GetValue();

         if (mCurrentWaypoint - 1 >= 0)
         {
            ResolveCoincidentWaypoints(mCurrentWaypoint, mCurrentWaypoint - 1);
         }
         if ((mCurrentWaypoint + 1) < mCurrentRoute.mWaypoints.size())
         {
            ResolveCoincidentWaypoints(mCurrentWaypoint, mCurrentWaypoint + 1);
         }
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::LonEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      if (mUi.mLonEdit->IsValid() || mUi.mLonEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mLon = mUi.mLonEdit->GetValue();

         if (mCurrentWaypoint - 1 >= 0)
         {
            ResolveCoincidentWaypoints(mCurrentWaypoint, mCurrentWaypoint - 1);
         }
         if ((mCurrentWaypoint + 1) < mCurrentRoute.mWaypoints.size())
         {
            ResolveCoincidentWaypoints(mCurrentWaypoint, mCurrentWaypoint + 1);
         }

         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::AltEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      UtLengthValue alt = WaypointValueType::cUSE_PREVIOUS;
      if (!mUi.mAltEdit->text().isEmpty())
      {
         alt = mUi.mAltEdit->GetValue();
      }
      if (mUi.mAltEdit->IsValid() || mUi.mAltEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mAlt = alt;
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::AglEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      mCurrentRoute.mWaypoints[mCurrentWaypoint].mAltRef = mUi.mAltRefEdit->currentIndex();
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::GoToEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      QString goTo = "";
      if (mUi.mGoToEdit->currentIndex() != 0)
      {
         goTo = mUi.mGoToEdit->currentText();
      }
      mCurrentRoute.mWaypoints[mCurrentWaypoint].mGoTo = goTo;
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::SpeedEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      UtSpeedValue speed = WaypointValueType::cUSE_PREVIOUS;
      if (!mUi.mSpeedEdit->text().isEmpty())
      {
         speed = mUi.mSpeedEdit->GetValue();
      }
      if (mUi.mSpeedEdit->IsValid() || mUi.mSpeedEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mSpeed = speed;
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::ClimbRateEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      UtSpeedValue climbRate = WaypointValueType::cUSE_PREVIOUS;
      // climb rate
      if (mUi.mClimbRateEdit->text().isEmpty())
      {
         QString placeholder = mUi.mClimbRateEdit->placeholderText();
         if (placeholder.compare("unset") == 0)
         {
            climbRate = WaypointValueType::cNOT_SET;
         }
         else if (placeholder.compare("mover default") == 0)
         {
            climbRate = WaypointValueType::cUSE_DEFAULT;
         }
      }
      else
      {
         climbRate = mUi.mClimbRateEdit->GetValue();
      }
      if (mUi.mClimbRateEdit->IsValid() || mUi.mClimbRateEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mClimbRate = climbRate;
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::LinAccelEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      UtAccelerationValue linAccel = WaypointValueType::cUSE_PREVIOUS;
      if (mUi.mLinAccelEdit->text().isEmpty())
      {
         QString placeholder = mUi.mLinAccelEdit->placeholderText();
         if (placeholder.compare("unset") == 0)
         {
            linAccel = WaypointValueType::cNOT_SET;
         }
         else if (placeholder.compare("mover default") == 0)
         {
            linAccel = WaypointValueType::cUSE_DEFAULT;
         }
      }
      else
      {
         linAccel = mUi.mLinAccelEdit->GetValue();
      }
      if (mUi.mLinAccelEdit->IsValid() || mUi.mLinAccelEdit->text().isEmpty())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mLinAccel = linAccel;
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::ComboBoxChangedHandler(int aIndex)
{
   auto safeIndex = ut::safe_cast<unsigned int, int>(aIndex);

   if (safeIndex == 0)
   {
      // set appropriate line edit
      mUi.mRadAccelStackedWidget->setCurrentWidget(mUi.mRadAccelPage);
      mRadIndex = safeIndex;
   }
   else if (safeIndex == 1)
   {
      // set appropriate line edit
      mUi.mRadAccelStackedWidget->setCurrentWidget(mUi.mBankAnglePage);
      mRadIndex = safeIndex;
   }
   else if (safeIndex == 2)
   {
      // set appropriate line edit
      mUi.mRadAccelStackedWidget->setCurrentWidget(mUi.mMaxGPage);
      mRadIndex = safeIndex;
   }
}

void wkf::RouteBrowserDialog::AnyRadAccelEditingFinished()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      UtAccelerationValue radAccel = WaypointValueType::cUSE_PREVIOUS;

      bool radIsValid    = false;
      int  radAccelIndex = mUi.mRadAccelStackedWidget->currentIndex();
      if (radAccelIndex == 0)
      {
         if (mUi.mRadAccelEdit->text().isEmpty())
         {
            QString placeholder = mUi.mRadAccelEdit->placeholderText();
            if (placeholder.compare("unset") == 0)
            {
               radAccel = WaypointValueType::cNOT_SET;
            }
            else if (placeholder.compare("mover default") == 0)
            {
               radAccel = WaypointValueType::cUSE_DEFAULT;
            }
            radIsValid = true;
         }
         else
         {
            radAccel   = mUi.mRadAccelEdit->GetValue();
            radIsValid = mUi.mRadAccelEdit->IsValid();
         }
      }
      else if (radAccelIndex == 1)
      {
         if (mUi.mBankAngleEdit->text().isEmpty())
         {
            QString placeholder = mUi.mBankAngleEdit->placeholderText();
            if (placeholder.compare("unset") == 0)
            {
               radAccel = WaypointValueType::cNOT_SET;
            }
            else if (placeholder.compare("mover default") == 0)
            {
               radAccel = WaypointValueType::cUSE_DEFAULT;
            }
            radIsValid = true;
         }
         else
         {
            UtAngleValue bankAngle = mUi.mBankAngleEdit->GetValue();
            radAccel.Set(tan(bankAngle) * UtEarth::cACCEL_OF_GRAVITY, UtUnitAcceleration::cMETERS_PER_SECOND2);
            radIsValid = mUi.mBankAngleEdit->IsValid();
         }
      }
      else if (radAccelIndex == 2)
      {
         if (mUi.mMaxGEdit->text().isEmpty())
         {
            QString placeholder = mUi.mMaxGEdit->placeholderText();
            if (placeholder.compare("unset") == 0)
            {
               radAccel = WaypointValueType::cNOT_SET;
            }
            else if (placeholder.compare("mover default") == 0)
            {
               radAccel = WaypointValueType::cUSE_DEFAULT;
            }
            radIsValid = true;
         }
         else
         {
            UtAccelerationValue maxGs = mUi.mMaxGEdit->GetValue();
            radAccel.Set(sqrt(maxGs * maxGs - UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY),
                         UtUnitAcceleration::cMETERS_PER_SECOND2);
            radIsValid = mUi.mMaxGEdit->IsValid();
         }
      }
      if (radIsValid && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
      {
         mCurrentRoute.mWaypoints[mCurrentWaypoint].mRadAccel = radAccel;
         IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
      }
   }
}

void wkf::RouteBrowserDialog::RadAccelEditingFinishedHandler()
{
   UtAccelerationValue radAccel = mUi.mRadAccelEdit->GetValue();

   UtAngleValue bankAngle;
   bankAngle.Set(atan2(radAccel, UtEarth::cACCEL_OF_GRAVITY) * UtMath::cDEG_PER_RAD, UtUnitAngle::cDEGREES);
   mUi.mBankAngleEdit->SetValue(bankAngle);

   UtAccelerationValue maxG;
   maxG.Set(sqrt(radAccel * radAccel + UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY) / UtEarth::cACCEL_OF_GRAVITY,
            UtUnitAcceleration::cG);
   mUi.mMaxGEdit->SetValue(maxG);

   AnyRadAccelEditingFinished();
}

void wkf::RouteBrowserDialog::BankAngleEditingFinishedHandler()
{
   UtAngleValue bankAngle = mUi.mBankAngleEdit->GetValue();

   UtAccelerationValue radAccel;
   radAccel.Set(tan(bankAngle) * UtEarth::cACCEL_OF_GRAVITY, UtUnitAcceleration::cMETERS_PER_SECOND2);
   mUi.mRadAccelEdit->SetValue(radAccel);

   UtAccelerationValue maxG;
   maxG.Set(sqrt(radAccel * radAccel + UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY) / UtEarth::cACCEL_OF_GRAVITY,
            UtUnitAcceleration::cG);
   mUi.mMaxGEdit->SetValue(maxG);

   AnyRadAccelEditingFinished();
}

void wkf::RouteBrowserDialog::MaxGEditingFinishedHandler()
{
   UtAccelerationValue maxG = mUi.mMaxGEdit->GetValue();

   UtAccelerationValue radAccel;
   radAccel.Set(sqrt(maxG * maxG - UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY),
                UtUnitAcceleration::cMETERS_PER_SECOND2);
   mUi.mRadAccelEdit->SetValue(radAccel);

   UtAngleValue bankAngle;
   bankAngle.Set(atan2(radAccel, UtEarth::cACCEL_OF_GRAVITY) * UtMath::cDEG_PER_RAD, UtUnitAngle::cDEGREES);
   mUi.mBankAngleEdit->SetValue(bankAngle);

   AnyRadAccelEditingFinished();
}

void wkf::RouteBrowserDialog::TurnDirEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      mCurrentRoute.mWaypoints[mCurrentWaypoint].mTurnDir = mUi.mTurnDirEdit->currentIndex() - 1;
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::SwitchTypeEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      mCurrentRoute.mWaypoints[mCurrentWaypoint].mSwitchType = mUi.mSwitchTypeEdit->currentIndex();
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::EndPathEditingFinishedHandler()
{
   if (mCurrentWaypoint >= 0 && mCurrentWaypoint < mCurrentRoute.mWaypoints.size())
   {
      mCurrentRoute.mEndPathType = mUi.mEndPathEdit->currentIndex();
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

void wkf::RouteBrowserDialog::AppendWaypoint(double aLatitude, double aLongitude, double aAltitude)
{
   int                                 insertIndex = mCurrentRoute.mWaypoints.size();
   RouteBrowserInterface::WaypointInfo insertWaypoint(insertIndex != 0);

   insertWaypoint.mLat = aLatitude;
   insertWaypoint.mLon = aLongitude;

   if (mCurrentRoute.mWaypoints.empty())
   {
      // This is to catch garbage values due to uninitialized altitude variable.
      if (UtMath::NearlyZero(aAltitude, 1e-6))
      {
         insertWaypoint.mAlt.Set(0.0, "meters");
      }
      else
      {
         insertWaypoint.mAlt.Set(aAltitude, "meters");
      }
   }
   else
   {
      RouteBrowserInterface::WaypointInfo previousWaypoint = mCurrentRoute.mWaypoints.last();
      insertWaypoint.mAlt                                  = previousWaypoint.mAlt;
      insertWaypoint.mAltRef                               = previousWaypoint.mAltRef;
   }
   insertWaypoint.mLabel = GetDefaultWaypointLabel(insertIndex);

   mCurrentRoute.mWaypoints.push_back(insertWaypoint);
   mCurrentWaypoint = insertIndex;

   bool successfullyAdded = true;

   // Check if the previous waypoint and this new waypoint have the same latitude/longitude
   if (mCurrentWaypoint > 0)
   {
      successfullyAdded = ResolveCoincidentWaypoints(mCurrentWaypoint, mCurrentWaypoint - 1);
   }

   // Update displays
   PopulateTree(mCurrentRoute, mCurrentWaypoint);
   if (successfullyAdded)
   {
      emit WaypointAdded(insertWaypoint, insertIndex, mCurrentRoute.mPlatformName, mCurrentRoute.mName);
   }

   IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
}

void wkf::RouteBrowserDialog::AddWaypointActionHandler()
{
   QAction* action = dynamic_cast<QAction*>(sender());
   if (action)
   {
      int insertIndex = action->data().toInt();

      RouteBrowserInterface::WaypointInfo insertWaypoint(insertIndex != 0);
      if (mCurrentRoute.mWaypoints.empty())
      {
         insertWaypoint.mLat = 0.0;
         insertWaypoint.mLon = 0.0;
         insertWaypoint.mAlt.Set(25000.0, "feet");
      }
      else
      {
         RouteBrowserInterface::WaypointInfo previousWaypoint;
         RouteBrowserInterface::WaypointInfo nextWaypoint;
         bool                                prevValid     = false;
         bool                                nextValid     = false;
         int                                 previousIndex = insertIndex - 1;
         if (previousIndex >= 0 && mCurrentRoute.mWaypoints.size() > previousIndex)
         {
            previousWaypoint = mCurrentRoute.mWaypoints[previousIndex];
            prevValid        = true;
         }
         if (insertIndex >= 0 && mCurrentRoute.mWaypoints.size() > insertIndex)
         {
            nextWaypoint = mCurrentRoute.mWaypoints[insertIndex];
            nextValid    = true;
         }
         if (prevValid && nextValid)
         {
            insertWaypoint.mLat = (previousWaypoint.mLat + nextWaypoint.mLat) * 0.5f;
            insertWaypoint.mLon = (previousWaypoint.mLon + nextWaypoint.mLon) * 0.5f;
            insertWaypoint.mAlt = (previousWaypoint.mAlt + nextWaypoint.mAlt) * 0.5f;
            // interpolate
         }
         else if (prevValid)
         {
            if (previousIndex >= 1)
            {
               RouteBrowserInterface::WaypointInfo prevprevWaypoint = mCurrentRoute.mWaypoints[previousIndex - 1];
               insertWaypoint.mLat = previousWaypoint.mLat + (previousWaypoint.mLat - prevprevWaypoint.mLat);
               insertWaypoint.mLon = previousWaypoint.mLon + (previousWaypoint.mLon - prevprevWaypoint.mLon);
               insertWaypoint.mAlt = previousWaypoint.mAlt + (previousWaypoint.mAlt - prevprevWaypoint.mAlt);
               // extrapolate forward
            }
            else
            {
               insertWaypoint.mLat = previousWaypoint.mLat + 1.0f;
               insertWaypoint.mLon = previousWaypoint.mLon;
               insertWaypoint.mAlt = previousWaypoint.mAlt;
               // do something else
            }
         }
         else if (nextValid)
         {
            if (mCurrentRoute.mWaypoints.size() > insertIndex + 1)
            {
               RouteBrowserInterface::WaypointInfo nextnextWaypoint = mCurrentRoute.mWaypoints[insertIndex + 1];
               insertWaypoint.mLat = nextWaypoint.mLat - (nextnextWaypoint.mLat - nextWaypoint.mLat);
               insertWaypoint.mLon = nextWaypoint.mLon - (nextnextWaypoint.mLon - nextWaypoint.mLon);
               insertWaypoint.mAlt = nextWaypoint.mAlt - (nextnextWaypoint.mAlt - nextWaypoint.mAlt);
               // extrapolate back
            }
            else
            {
               insertWaypoint.mLat = nextWaypoint.mLat + 1.0f;
               insertWaypoint.mLon = nextWaypoint.mLon;
               insertWaypoint.mAlt = nextWaypoint.mAlt;
               // do something else
            }
         }
      }
      insertWaypoint.mLabel = GetDefaultWaypointLabel(insertIndex);

      mCurrentRoute.mWaypoints.insert(insertIndex, insertWaypoint);
      mCurrentWaypoint = insertIndex;

      // Update Default Route Labels
      UpdateDefaultWaypointLabels(mCurrentRoute);

      // Update displays
      PopulateTree(mCurrentRoute, mCurrentWaypoint);
      emit WaypointAdded(insertWaypoint, insertIndex, mCurrentRoute.mPlatformName, mCurrentRoute.mName);
   }
}

void wkf::RouteBrowserDialog::RemoveWaypointActionHandler()
{
   QAction* action = dynamic_cast<QAction*>(sender());
   if (action)
   {
      RemoveWaypointAtIndex(action->data().toInt());
   }
}

void wkf::RouteBrowserDialog::RemoveWaypointAtIndex(int aIndex)
{
   // Check to make sure aIndex is within boundaries of mWaypoints
   if (aIndex < mCurrentRoute.mWaypoints.size())
   {
      // Make sure no other waypoints refer to the waypoint that is about to be removed
      // If the goto label is to this waypoint, clear it
      const QString& label = mCurrentRoute.mWaypoints.at(aIndex).mLabel;
      for (auto& wypt : mCurrentRoute.mWaypoints)
      {
         if (wypt.mGoTo == label)
         {
            wypt.mGoTo = "";
         }
      }

      // Removes the selected waypoint, no need to update displays here as the PopulateTree call will handle that
      mCurrentRoute.mWaypoints.removeAt(aIndex);

      // Decrement current waypoints index, if the waypoint that was was the last waypoint
      if (mCurrentWaypoint >= mCurrentRoute.mWaypoints.size())
      {
         mCurrentWaypoint--;
      }

      // Update Default Route Labels
      UpdateDefaultWaypointLabels(mCurrentRoute);

      // Update displays
      PopulateTree(mCurrentRoute, mCurrentWaypoint);
      emit WaypointRemoved(aIndex, mCurrentRoute.mPlatformName, mCurrentRoute.mName);
      IsCurrentRouteGlobal() ? emit GlobalRouteEdited(mCurrentRoute) : emit RouteEdited(mCurrentRoute);
   }
}

// Populates the dialog with the waypoint of index given by mCurrentWaypoint
void wkf::RouteBrowserDialog::PopulateWaypointInfo()
{
   QList<QTreeWidgetItem*> selection     = mUi.treeWidget->selectedItems();
   bool                    enableWidgets = false;
   if (selection.size() == 1)
   {
      QTreeWidgetItem* currentItem = selection[0];

      if (currentItem != nullptr)
      {
         mCurrentWaypoint = mUi.treeWidget->indexOfTopLevelItem(currentItem);

         if ((mCurrentWaypoint != -1) && (mCurrentWaypoint < mCurrentRoute.mWaypoints.size()))
         {
            RouteBrowserInterface::WaypointInfo waypointInfo = mCurrentRoute.mWaypoints[mCurrentWaypoint];

            // label
            mUi.mLabelEdit->clear();
            mUi.mLabelEdit->setPlaceholderText("unset");
            mUi.mLabelEdit->setText(waypointInfo.mLabel);

            if (waypointInfo.mPointType == RouteBrowserInterface::WaypointInfo::cABSOLUTE)
            {
               mUi.mLatEdit->show();
               mUi.mLonEdit->show();
               UtLatPos latitude(waypointInfo.mLat, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
               UtLonPos longitude(waypointInfo.mLon, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
               mUi.mLatEdit->SetValue(latitude);
               mUi.mLonEdit->SetValue(longitude);
            }
            else
            {
               mUi.mLatEdit->hide();
               mUi.mLonEdit->hide();
               QMessageBox::warning(this, "Unsupported Waypoint Type", "Cannot edit the position of this waypoint.");
            }

            // altitude
            mUi.mAltEdit->clear();
            if (waypointInfo.mAlt == WaypointValueType::cNOT_SET)
            {
               mUi.mAltEdit->setPlaceholderText("unset");
               mUi.mAltEdit->UpdateValidityColor(); // this is needed because "unset" will be shown as invalid otherwise
            }
            else if (waypointInfo.mAlt == WaypointValueType::cUSE_PREVIOUS)
            {
               mUi.mAltEdit->setPlaceholderText("previous");
               mUi.mAltEdit->UpdateValidityColor(); // this is needed because "previous" will be shown as invalid otherwise
            }
            else
            {
               mUi.mAltEdit->SetValue(GetUI_Altitude(waypointInfo.mAlt));
            }

            // altitude ref
            switch (static_cast<RouteBrowserInterface::WaypointInfo::AltitudeReference>(waypointInfo.mAltRef))
            {
            case RouteBrowserInterface::WaypointInfo::AltitudeReference::UsePrevious:
            case RouteBrowserInterface::WaypointInfo::AltitudeReference::Unset:
            case RouteBrowserInterface::WaypointInfo::AltitudeReference::MoverDefault:
               mUi.mAltRefEdit->setCurrentIndex(
                  static_cast<int>(RouteBrowserInterface::WaypointInfo::AltitudeReference::MoverDefault));
               break;
            case RouteBrowserInterface::WaypointInfo::AltitudeReference::MSL:
               mUi.mAltRefEdit->setCurrentIndex(
                  static_cast<int>(RouteBrowserInterface::WaypointInfo::AltitudeReference::MSL));
               break;
            case RouteBrowserInterface::WaypointInfo::AltitudeReference::AGL:
               mUi.mAltRefEdit->setCurrentIndex(
                  static_cast<int>(RouteBrowserInterface::WaypointInfo::AltitudeReference::AGL));
               break;
            default:
               break;
            }

            // goTo
            mUi.mGoToEdit->clear();
            mUi.mGoToEdit->addItem("none");
            int count = 0;
            for (auto& w : mCurrentRoute.mWaypoints)
            {
               // Do not add the label for the current waypoint to the list of waypoint available for the goto command.
               // Goto does not support looping to itself.
               if (!w.mLabel.isEmpty() && (count != mCurrentWaypoint))
               {
                  mUi.mGoToEdit->addItem(w.mLabel);
                  if (waypointInfo.mGoTo == w.mLabel)
                  {
                     mUi.mGoToEdit->setCurrentText(waypointInfo.mGoTo);
                  }
               }
               count++;
            }

            // speed
            mUi.mSpeedEdit->clear();
            if (waypointInfo.mSpeed == WaypointValueType::cNOT_SET)
            {
               mUi.mSpeedEdit->setPlaceholderText("unset");
               mUi.mSpeedEdit->UpdateValidityColor(); // this is needed because "unset" will be shown as invalid otherwise
            }
            else if (waypointInfo.mSpeed == WaypointValueType::cUSE_PREVIOUS)
            {
               mUi.mSpeedEdit->setPlaceholderText("previous");
               mUi.mSpeedEdit->UpdateValidityColor(); // this is needed because "previous" will be shown as invalid otherwise
            }
            else
            {
               UtSpeedValue speed = waypointInfo.mSpeed;
               speed.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit());
               mUi.mSpeedEdit->SetValue(speed);
            }

            // climb rate
            mUi.mClimbRateEdit->clear();
            if (waypointInfo.mClimbRate == WaypointValueType::cNOT_SET)
            {
               mUi.mClimbRateEdit->setPlaceholderText("unset");
               mUi.mClimbRateEdit->UpdateValidityColor(); // this is needed because "unset" will be shown as invalid otherwise
            }
            else if (waypointInfo.mClimbRate == WaypointValueType::cUSE_PREVIOUS)
            {
               mUi.mClimbRateEdit->setPlaceholderText("previous");
               mUi.mClimbRateEdit->UpdateValidityColor(); // this is needed because "previous" will be shown as invalid otherwise
            }
            else if (waypointInfo.mClimbRate == WaypointValueType::cUSE_DEFAULT)
            {
               mUi.mClimbRateEdit->setPlaceholderText("mover default");
               mUi.mClimbRateEdit->UpdateValidityColor(); // this is needed because "mover default" will be shown as
                                                          // invalid otherwise
            }
            else
            {
               UtSpeedValue climbRate = waypointInfo.mClimbRate;
               climbRate.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetSpeedUnit());
               mUi.mClimbRateEdit->SetValue(climbRate);
            }

            // linear acceleration
            mUi.mLinAccelEdit->clear();
            if (waypointInfo.mLinAccel == WaypointValueType::cNOT_SET)
            {
               mUi.mLinAccelEdit->setPlaceholderText("unset");
               mUi.mLinAccelEdit->UpdateValidityColor(); // this is needed because "unset" will be shown as invalid otherwise
            }
            else if (waypointInfo.mLinAccel == WaypointValueType::cUSE_PREVIOUS)
            {
               mUi.mLinAccelEdit->setPlaceholderText("previous");
               mUi.mLinAccelEdit->UpdateValidityColor(); // this is needed because "previous" will be shown as invalid otherwise
            }
            else if (waypointInfo.mLinAccel == WaypointValueType::cUSE_DEFAULT)
            {
               mUi.mLinAccelEdit->setPlaceholderText("mover default");
               mUi.mLinAccelEdit->UpdateValidityColor(); // this is needed because "mover default" will be shown as
                                                         // invalid otherwise
            }
            else
            {
               UtAccelerationValue linAccel = waypointInfo.mLinAccel;
               linAccel.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit());
               mUi.mLinAccelEdit->SetValue(linAccel);
            }

            // radial acceleration
            mUi.mRadAccelComboBox->clear();
            mUi.mRadAccelComboBox->addItem("Radial Acceleration");
            mUi.mRadAccelComboBox->addItem("Bank Angle");
            mUi.mRadAccelComboBox->addItem("Maximum G's");
            mUi.mRadAccelComboBox->setCurrentIndex(0);
            mUi.mRadAccelComboBox->setCurrentIndex(mRadIndex);

            mUi.mRadAccelEdit->clear();
            mUi.mBankAngleEdit->clear();
            mUi.mMaxGEdit->clear();

            if (waypointInfo.mRadAccel == WaypointValueType::cNOT_SET)
            {
               mUi.mRadAccelEdit->setPlaceholderText("unset");
               mUi.mBankAngleEdit->setPlaceholderText("unset");
               mUi.mMaxGEdit->setPlaceholderText("unset");

               // this is needed because "unset" will be shown as invalid otherwise
               mUi.mRadAccelEdit->UpdateValidityColor();
               mUi.mBankAngleEdit->UpdateValidityColor();
               mUi.mMaxGEdit->UpdateValidityColor();
            }
            else if (waypointInfo.mRadAccel == WaypointValueType::cUSE_PREVIOUS)
            {
               mUi.mRadAccelEdit->setPlaceholderText("previous");
               mUi.mBankAngleEdit->setPlaceholderText("previous");
               mUi.mMaxGEdit->setPlaceholderText("previous");

               // this is needed because "previous" will be shown as invalid otherwise
               mUi.mRadAccelEdit->UpdateValidityColor();
               mUi.mBankAngleEdit->UpdateValidityColor();
               mUi.mMaxGEdit->UpdateValidityColor();
            }
            else if (waypointInfo.mRadAccel == WaypointValueType::cUSE_DEFAULT)
            {
               mUi.mRadAccelEdit->setPlaceholderText("mover default");
               mUi.mBankAngleEdit->setPlaceholderText("mover default");
               mUi.mMaxGEdit->setPlaceholderText("mover default");

               // this is needed because "mover default" will be shown as invalid otherwise
               mUi.mRadAccelEdit->UpdateValidityColor();
               mUi.mBankAngleEdit->UpdateValidityColor();
               mUi.mMaxGEdit->UpdateValidityColor();
            }
            else
            {
               UtAccelerationValue radAccel = waypointInfo.mRadAccel;
               radAccel.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAccelerationUnit());
               mUi.mRadAccelEdit->SetValue(radAccel);

               UtAngleValue bankAngle;
               bankAngle.Set(atan2(waypointInfo.mRadAccel, UtEarth::cACCEL_OF_GRAVITY) * UtMath::cDEG_PER_RAD,
                             UtUnitAngle::cDEGREES);
               bankAngle.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAngleUnit());
               mUi.mBankAngleEdit->SetValue(bankAngle);

               UtAccelerationValue maxG;
               maxG.Set(sqrt(waypointInfo.mRadAccel * waypointInfo.mRadAccel +
                             UtEarth::cACCEL_OF_GRAVITY * UtEarth::cACCEL_OF_GRAVITY) /
                           UtEarth::cACCEL_OF_GRAVITY,
                        UtUnitAcceleration::cG);
               mUi.mMaxGEdit->SetValue(maxG);
            }

            // turn direction
            mUi.mTurnDirEdit->setCurrentIndex(waypointInfo.mTurnDir + 1);

            // switch type
            mUi.mSwitchTypeEdit->setCurrentIndex(waypointInfo.mSwitchType);

            // end path
            mUi.mEndPathEdit->setCurrentIndex(mCurrentRoute.mEndPathType);
         }
      }
      enableWidgets = true;
   }
   int count = mUi.waypointPropertiesSheet->count();
   for (int i = 0; i < count; ++i)
   {
      QWidget* widget = mUi.waypointPropertiesSheet->itemAt(i)->widget();
      if (widget)
      {
         widget->setEnabled(enableWidgets);
      }
   }
}

void wkf::RouteBrowserDialog::WaypointModifiedCB(const AttachmentRoute* aRoute, int aWaypointIndex)
{
   if (aWaypointIndex >= 0 && aWaypointIndex < mCurrentRoute.mWaypoints.size())
   {
      Waypoint* waypoint = aRoute->GetWaypoint(aWaypointIndex);
      if (waypoint)
      {
         mCurrentRoute.mWaypoints[aWaypointIndex].mLat = waypoint->GetLat();
         mCurrentRoute.mWaypoints[aWaypointIndex].mLon = waypoint->GetLon();

         if (aWaypointIndex == mCurrentWaypoint)
         {
            UtLatPos latitude(waypoint->GetLat(), wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
            UtLonPos longitude(waypoint->GetLon(), wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
            mUi.mLatEdit->SetValue(latitude);
            mUi.mLonEdit->SetValue(longitude);

            emit WaypointLatEdited(mCurrentRoute.mPlatformName,
                                   mCurrentRoute.mName,
                                   mCurrentRoute.mWaypoints[mCurrentWaypoint].mLat,
                                   mCurrentWaypoint);

            emit WaypointLonEdited(mCurrentRoute.mPlatformName,
                                   mCurrentRoute.mName,
                                   mCurrentRoute.mWaypoints[mCurrentWaypoint].mLon,
                                   mCurrentWaypoint);
         }
      }
   }
}

bool wkf::RouteBrowserDialog::ResolveCoincidentWaypoints(int aWaypoint1, int aWaypoint2)
{
   if (mCurrentRoute.mWaypoints[aWaypoint1].mLat == mCurrentRoute.mWaypoints[aWaypoint2].mLat &&
       mCurrentRoute.mWaypoints[aWaypoint1].mLon == mCurrentRoute.mWaypoints[aWaypoint2].mLon)
   {
      if (mDuplicateWarningBox.isHidden())
      {
         mDuplicateWarningBox.exec();
         mUi.treeWidget->takeTopLevelItem(aWaypoint2);
         mCurrentRoute.mWaypoints.removeAt(aWaypoint2);
         return false;
      }
   }
   return true;
}


UtLengthValue wkf::RouteBrowserDialog::GetUI_Altitude(const UtLengthValue& aAlt) const
{
   UtLengthValue altitude = aAlt;
   altitude.ConvertToUnit(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetAltitudeUnit(aAlt));
   return altitude;
}
