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

#include "CreateEngineDialog.hpp"

#include <QDir>
#include <QMessageBox>

#include "EngineDesignerWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "ui_CreateEngineDialog.h"

namespace Designer
{

CreateEngineDialog::CreateEngineDialog(QWidget*        aParent,
                                       Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::CreateEngineDialog)
{
   mUIPtr->setupUi(this);

   SetBaseDirPath(GetBaseDirPath() + "/Engines/");

   connect(mUIPtr->mEngineTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CreateEngineDialog::EngineTypeChanged);
   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &CreateEngineDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &CreateEngineDialog::CancelClicked);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
   if (mUIPtr->mEngineTypeComboBox->count() == 0)
   {
      mUIPtr->mEngineTypeComboBox->addItem(" ");
   }
}

CreateEngineDialog::~CreateEngineDialog()
{
   delete mUIPtr;
}

void CreateEngineDialog::SetEngineDesignerWidget(const std::shared_ptr<EngineDesignerWidget>& aEngineDesignerWidget)
{
   mSharedEngineDesignerWidget = aEngineDesignerWidget;
}

void CreateEngineDialog::EngineTypeChanged(int aIndex)
{
   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mEngineTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mEngineListComboBox);
}

void CreateEngineDialog::OkClicked(bool aClicked)
{
   if (mUIPtr->mInstanceNameLineEdit->text() == "")
   {
      QMessageBox::warning(this, tr("Mover Creator"),
                           tr("You must give this engine a name before proceeding."),
                           QMessageBox::Ok);
      return;
   }

   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mEngineTypeComboBox->currentText());
   dirName.append("/");
   QString fileName = mUIPtr->mInstanceNameLineEdit->text();
   if (!fileName.endsWith(".amc"))
   {
      fileName.append(".amc");
   }

   if (IsTemplateFile(dirName + fileName))
   {
      QMessageBox::critical(this, tr("Mover Creator"),
                            tr("You are attempting to overwrite a file that has been marked as being a template for this engine.\n"
                               "Please enter a different name for this file."),
                            QMessageBox::Ok);
      return;
   }
   QString newFilePath = dirName;
   newFilePath.append(fileName);
   if (QFile::exists(newFilePath))
   {
      QMessageBox::critical(this, tr("Mover Creator"),
                            tr("You are attempting to overwrite a file that already exists.\n"
                               "Please enter a different name for this file."),
                            QMessageBox::Ok);
      return;
   }
   QFile myFile(newFilePath);

   QJsonObject jsonObj;
   if (mUIPtr->mEngineListComboBox->currentText() == "")
   {
      jsonObj["standardTemplate"] = false;
      jsonObj["engine_type"] = mUIPtr->mEngineTypeComboBox->currentText();;
      jsonObj["model_name"] = "";
      jsonObj["inheritedFileName"] = mUIPtr->mEngineListComboBox->currentText();
      jsonObj["fileName"] = fileName;
      QJsonDocument myDoc(jsonObj);
      myFile.open(QIODevice::WriteOnly | QIODevice::Text);
      myFile.write(myDoc.toJson());
      myFile.close();
   }
   else
   {
      // Copy the information from the selected file in the list combo box
      QString copyFilePath = dirName + mUIPtr->mEngineListComboBox->currentText();
      copyFilePath.append(".amc");
      QFile jDoc(copyFilePath);
      if (jDoc.open(QIODevice::ReadOnly))
      {
         QByteArray fileData = jDoc.readAll();
         QJsonDocument copyDoc(QJsonDocument::fromJson(fileData));
         QJsonObject copyObj(copyDoc.object());
         if (copyDoc.isNull() || copyObj.isEmpty())
         {
            QMessageBox::critical(this, tr("Mover Creator"), tr("Error reading template file: malformed data"));
            return;
         }
         copyObj["standardTemplate"] = false;
         copyObj["inheritedFileName"] = mUIPtr->mEngineListComboBox->currentText();
         copyObj["engine_type"] = mUIPtr->mEngineTypeComboBox->currentText();;
         copyObj["fileName"] = fileName;
         fileName.truncate(fileName.size() - 4);
         copyObj["model_name"] = fileName;
         QJsonDocument myDoc(copyObj);
         myFile.open(QIODevice::WriteOnly | QIODevice::Text);
         myFile.write(myDoc.toJson());
         myFile.close();
      }
   }

   QString engineTypeName = mUIPtr->mEngineTypeComboBox->currentText();
   QString instanceName = mUIPtr->mInstanceNameLineEdit->text();

   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->ResetAllTabs();
   mcWidget->SetEngineTypeAndModel(engineTypeName, instanceName);
   mcWidget->SetInitialTab(Designer::cENGINE_TAB);

   EngineCreated(instanceName);
   close();
}

void CreateEngineDialog::SetEngineTypeAndModel(QString aType, QString aModel)
{
   mUIPtr->mEngineTypeComboBox->setCurrentText(aType);
   mUIPtr->mEngineListComboBox->setCurrentText(aModel);
}

void CreateEngineDialog::ChangeTheme(QString aThemeName)
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

void CreateEngineDialog::showEvent(QShowEvent* aEventPtr)
{
   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
   if (mUIPtr->mEngineTypeComboBox->count() == 0)
   {
      mUIPtr->mEngineTypeComboBox->addItem(" ");
   }
   QWidget::showEvent(aEventPtr);
}

} // namespace Designer
