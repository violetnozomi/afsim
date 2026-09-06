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
#include "WkfTeamVisibilityPrefWidget.hpp"

#include <QCheckBox>
#include <QKeyEvent>
#include <QMessageBox>

#include "UtCast.hpp"
#include "UtLog.hpp"
#include "UtQt.hpp"
#include "UtQtColorButton.hpp"
#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

wkf::TeamVisibilityPrefWidget::TeamVisibilityPrefWidget(QWidget* aParent)
   : PrefWidgetT<wkf::TeamVisibilityPrefObject>(aParent)
{
   mUi.setupUi(this);
   connect(mUi.addTeamPushButton, &QPushButton::clicked, this, &TeamVisibilityPrefWidget::AddNewTeamButtonPressed);
   connect(wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>(),
           &wkf::TeamVisibilityPrefObject::TeamColorChanged,
           this,
           [&] { ReadPreferenceData(mPrefObjectPtr->GetPreferences()); });
   mUi.teamNameLineEdit->installEventFilter(this);
   mUi.teamTableWidget->installEventFilter(this);
   mUi.teamTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
   mUi.teamTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

void wkf::TeamVisibilityPrefWidget::AddNewTeamButtonPressed()
{
   // Check to see if new name is unique
   QString teamName = mUi.teamNameLineEdit->text();
   if (teamName.contains(" "))
   {
      QMessageBox::information(nullptr, "Invalid Team Entry", "Team name can't include space characters.");
   }
   else if (!teamName.isEmpty())
   {
      bool unique = true;
      for (int i = 0; i < mUi.teamTableWidget->rowCount(); ++i)
      {
         if (teamName == mUi.teamTableWidget->item(i, 0)->text())
         {
            unique = false;
            break;
         }
      }

      if (unique)
      {
         // if it is unique add a row to the table.
         int row = mUi.teamTableWidget->rowCount();
         mUi.teamTableWidget->setRowCount(row + 1);

         QTableWidgetItem* item = new QTableWidgetItem();
         item->setText(teamName);
         item->setFlags(item->flags() & ~Qt::ItemIsEditable);
         mUi.teamTableWidget->setItem(row, 0, item);

         UtQtColorButton* colorButton = new UtQtColorButton(mUi.teamTableWidget);
         colorButton->SetColor(GetPreferenceObject()->GetTeamColor(teamName));
         mUi.teamTableWidget->setCellWidget(row, 2, colorButton);

         QCheckBox* checkbox = new QCheckBox();
         checkbox->setChecked(true);
         checkbox->setStyleSheet("margin-left:50%; margin-right:50%;");
         mUi.teamTableWidget->setCellWidget(row, 1, checkbox);

         mUi.teamTableWidget->scrollToBottom();
         mUi.teamTableWidget->resizeRowsToContents();

         // Check if it was deleted and now we are adding it back, if so, we need to remove it from deleted list.
         for (int i = 0; i < mDeletedTeamNames.size(); ++i)
         {
            if (mDeletedTeamNames.at(i) == teamName)
            {
               // Should be a unique list so we can break out of loop once we find it in list once.
               mDeletedTeamNames.removeAt(i);
               break;
            }
         }
      }
      else
      {
         QMessageBox::information(nullptr,
                                  "Duplicate Team Entry",
                                  QString("Team %1 already exists, unable to add team.").arg(teamName));
      }
   }
}

bool wkf::TeamVisibilityPrefWidget::eventFilter(QObject* aObject, QEvent* aEvent)
{
   if (aEvent->type() == QEvent::KeyPress)
   {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(aEvent);
      if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
      {
         // Allow for Enter to be hit in the LineEdit without closing the Preferences dialog.
         if (aObject == mUi.teamNameLineEdit)
         {
            // Can't simply connect to the returnPressed() signal because we want to block
            //  this key stroke from being propagated to the Preferences dialog.
            AddNewTeamButtonPressed();
            return true;
         }
      }
      else if (keyEvent->key() == Qt::Key_Delete)
      {
         // Allows for delete within the TeamTableWidget
         if (mUi.teamTableWidget->hasFocus())
         {
            int     row      = mUi.teamTableWidget->currentRow();
            QString teamName = mUi.teamTableWidget->item(row, 0)->text();
            if (teamName == wkf::TeamVisibilityPrefData::cDEFAULT_TEAM)
            {
               QMessageBox::information(nullptr, "Can't Remove Default Team", "Cannot remove the default team. It is used when an entity's team name does not match any team name in this list.");
            }
            else
            {
               mDeletedTeamNames.push_back(teamName);
               mUi.teamTableWidget->removeRow(row);
            }
            return true;
         }
      }
   }
   return QWidget::eventFilter(aObject, aEvent);
}

void wkf::TeamVisibilityPrefWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::TeamVisibilityPrefWidget::ReadPreferenceData(const wkf::TeamVisibilityPrefData& aPrefData)
{
   mDeletedTeamNames.clear();

   // Handle Team Visibility & Color
   int row = 0;
   mUi.teamTableWidget->clearContents();
   mUi.teamTableWidget->setRowCount(ut::cast_to_int(aPrefData.mTeamInfo.size()));
   for (auto& info : aPrefData.mTeamInfo)
   {
      QTableWidgetItem* item = new QTableWidgetItem();
      item->setText(info.first);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      mUi.teamTableWidget->setItem(row, 0, item);

      UtQtColorButton* colorButton = new UtQtColorButton(mUi.teamTableWidget);
      colorButton->SetColor(info.second.color);
      mUi.teamTableWidget->setCellWidget(row, 2, colorButton);

      QCheckBox* checkbox = new QCheckBox();
      checkbox->setChecked(info.second.visible);
      checkbox->setStyleSheet("margin-left:50%; margin-right:50%;");
      checkbox->setEnabled(!aPrefData.mLockTeamVisibility);
      mUi.teamTableWidget->setCellWidget(row, 1, checkbox);

      row++;
   }
   mUi.teamTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
   mUi.teamTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
   mUi.teamTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
   mUi.teamTableWidget->resizeRowsToContents();
}

void wkf::TeamVisibilityPrefWidget::WritePreferenceData(wkf::TeamVisibilityPrefData& aPrefData)
{
   aPrefData.mTeamInfo.clear();

   // Get Team info from the Table Widget
   for (int i = 0; i < mUi.teamTableWidget->rowCount(); ++i)
   {
      QTableWidgetItem* item        = mUi.teamTableWidget->item(i, 0);
      auto*             colorButton = dynamic_cast<UtQtColorButton*>(mUi.teamTableWidget->cellWidget(i, 2));
      auto*             checkbox    = dynamic_cast<QCheckBox*>(mUi.teamTableWidget->cellWidget(i, 1));
      if (item != nullptr && colorButton != nullptr && checkbox != nullptr)
      {
         wkf::TeamVisibilityPrefData::TeamInfo info;
         info.color   = colorButton->GetColor();
         info.visible = checkbox->isChecked();
         aPrefData.mTeamInfo.emplace(item->text(), info);
      }
   }
}
