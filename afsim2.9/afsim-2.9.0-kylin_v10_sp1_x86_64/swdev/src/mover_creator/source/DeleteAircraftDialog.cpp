// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "DeleteAircraftDialog.hpp"

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

#include "ui_DeleteAircraftDialog.h"

namespace Designer
{

DeleteAircraftDialog::DeleteAircraftDialog(QWidget*        aParent,
                                           Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::DeleteAircraftDialog)
{
   mUIPtr->setupUi(this);
   SetBaseDirPath(GetBaseDirPath() + "/Vehicles/Aircraft/");

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &DeleteAircraftDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &DeleteAircraftDialog::CancelClicked);
   connect(mUIPtr->mVehicleTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeleteAircraftDialog::VehicleTypeChanged);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);
}

DeleteAircraftDialog::~DeleteAircraftDialog()
{
   delete mUIPtr;
}

void DeleteAircraftDialog::VehicleTypeChanged(int aIndex)
{
   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mVehicleTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mVehicleListComboBox, false);
}

void DeleteAircraftDialog::OkClicked(bool aClicked)
{
   bool deleteOK = false;
   QString msg = "Are you sure you want to delete " + mUIPtr->mVehicleListComboBox->currentText();

   int ret = QMessageBox::question(this, tr("Mover Creator"),
                                   tr(msg.toStdString().c_str()), QMessageBox::Yes | QMessageBox::No);

   if (ret == QMessageBox::Yes)
   {
      deleteOK = true;
   }

   if (deleteOK)
   {
      QString deleteFile = GetBaseDirPath() + mUIPtr->mVehicleTypeComboBox->currentText() + "/" + mUIPtr->mVehicleListComboBox->currentText();
      QFile::remove(deleteFile);
      close();
   }
}

void DeleteAircraftDialog::showEvent(QShowEvent* aEvent)
{
   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mVehicleTypeComboBox);
   QWidget::showEvent(aEvent);
}

void DeleteAircraftDialog::ChangeTheme(QString aThemeName)
{
   QString baseIconStr(QString(":/icons/%1/64x64/").arg(aThemeName));
   mUIPtr->mDialogGraphic->setPixmap(QPixmap(baseIconStr + "aircraft.png"));

   // This fixes the QT bug where combo boxes do not correctly change when the palette is changed
   QList<QComboBox*> comboBoxList = this->findChildren<QComboBox*>();

   for (auto& curCB : comboBoxList)
   {
      if (aThemeName == "Dark")

      {
         curCB->setStyleSheet("color: white; background - color: gray");
      }
      else
      {
         curCB->setStyleSheet("color: black; background - color: white");
      }
   }
}

} // namespace Designer
