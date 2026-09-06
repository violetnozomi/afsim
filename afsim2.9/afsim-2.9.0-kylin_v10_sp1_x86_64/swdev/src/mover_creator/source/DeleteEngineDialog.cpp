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

#include "DeleteEngineDialog.hpp"

#include <QDir>
#include <QMessageBox>

#include "ui_DeleteEngineDialog.h"

namespace Designer
{

DeleteEngineDialog::DeleteEngineDialog(QWidget*        aParent,
                                       Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::DeleteEngineDialog)
{
   mUIPtr->setupUi(this);
   SetBaseDirPath(GetBaseDirPath() + "/Engines/");

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &DeleteEngineDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &DeleteEngineDialog::CancelClicked);
   connect(mUIPtr->mEngineTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeleteEngineDialog::EngineTypeChanged);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
}

DeleteEngineDialog::~DeleteEngineDialog()
{
   delete mUIPtr;
}

void DeleteEngineDialog::EngineTypeChanged(int aIndex)
{
   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mEngineTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mEngineListComboBox, false);
}

void DeleteEngineDialog::OkClicked(bool aClicked)
{
   bool deleteOK = false;
   QString msg = "Are you sure you want to delete " + mUIPtr->mEngineListComboBox->currentText();

   int ret = QMessageBox::question(this, tr("Mover Creator"),
                                   tr(msg.toStdString().c_str()), QMessageBox::Yes | QMessageBox::No);

   if (ret == QMessageBox::Yes)
   {
      deleteOK = true;
   }

   if (deleteOK)
   {
      QString deleteFile = GetBaseDirPath() + mUIPtr->mEngineTypeComboBox->currentText() + "/" + mUIPtr->mEngineListComboBox->currentText();
      QFile::remove(deleteFile);
      close();
   }
}

void DeleteEngineDialog::showEvent(QShowEvent* aEvent)
{
   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
   QWidget::showEvent(aEvent);
}

void DeleteEngineDialog::ChangeTheme(QString aThemeName)
{
   QString baseIconStr(QString(":/icons/%1/64x64/").arg(aThemeName));
   mUIPtr->mDialogGraphic->setPixmap(QPixmap(baseIconStr + "engine.png"));

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
