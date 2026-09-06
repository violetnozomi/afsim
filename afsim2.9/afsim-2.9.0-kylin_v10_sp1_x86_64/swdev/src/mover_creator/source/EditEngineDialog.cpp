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

#include "EditEngineDialog.hpp"

#include <QDir>
#include <QMessageBox>

#include "EngineDesignerWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorTabs.hpp"
#include "MoverCreatorWidget.hpp"
#include "ui_EditEngineDialog.h"

namespace Designer
{

EditEngineDialog::EditEngineDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags)
   : MoverCreatorDialogBase(aParent, aFlags)
   , mUIPtr(new Ui::EditEngineDialog)
{
   mUIPtr->setupUi(this);
   SetBaseDirPath(GetBaseDirPath() + "/Engines/");

   connect(mUIPtr->mOkButton, &QPushButton::clicked, this, &EditEngineDialog::OkClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &EditEngineDialog::CancelClicked);
   connect(mUIPtr->mEngineTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EditEngineDialog::EngineTypeChanged);

   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
}

EditEngineDialog::~EditEngineDialog()
{
   delete mUIPtr;
}

void EditEngineDialog::EngineTypeChanged(int aIndex)
{
   QString dirName = GetBaseDirPath();
   dirName.append(mUIPtr->mEngineTypeComboBox->currentText());
   dirName.append("/");
   PopulateListComboBox(dirName, mUIPtr->mEngineListComboBox, false);
}

void EditEngineDialog::OkClicked(bool aClicked)
{
   QString engineTypeName = mUIPtr->mEngineTypeComboBox->currentText();
   QString engineListName = mUIPtr->mEngineListComboBox->currentText();

   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->ResetAllTabs();
   mcWidget->SetEngineTypeAndModel(engineTypeName, engineListName);
   mcWidget->SetInitialTab(Designer::cENGINE_TAB);

   close();
}

void EditEngineDialog::showEvent(QShowEvent* aEvent)
{
   PopulateTypeComboBox(GetBaseDirPath(), mUIPtr->mEngineTypeComboBox);
   QWidget::showEvent(aEvent);
}

void EditEngineDialog::SetEngineTypeAndModel(QString aType, QString aModel)
{
   mUIPtr->mEngineTypeComboBox->setCurrentText(aType);
   mUIPtr->mEngineListComboBox->setCurrentText(aModel);
}

void EditEngineDialog::ChangeTheme(QString aThemeName)
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
