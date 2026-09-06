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

#include "StartDialog.hpp"

#include "CreateAircraftDialog.hpp"
#include "CreateEngineDialog.hpp"
#include "CreateWeaponDialog.hpp"
#include "ConvertBrawlerAircraftDialog.hpp"
#include "DeleteAircraftDialog.hpp"
#include "DeleteEngineDialog.hpp"
#include "DeleteWeaponDialog.hpp"
#include "EditAircraftDialog.hpp"
#include "EditEngineDialog.hpp"
#include "EditWeaponDialog.hpp"
#include "EngineDesignerWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "ui_StartDialog.h"

namespace Designer
{

StartDialog::StartDialog(QWidget*        aParent,
                         Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::StartDialog)
   , mCreateAircraftDialog(new CreateAircraftDialog(nullptr))
   , mEditAircraftDialog(new EditAircraftDialog(nullptr))
   , mDeleteAircraftDialog(new DeleteAircraftDialog(nullptr))
   , mConvertBrawlerAircraftDialog(new ConvertBrawlerAircraftDialog(nullptr))
   , mCreateWeaponDialog(new CreateWeaponDialog(nullptr))
   , mEditWeaponDialog(new EditWeaponDialog(nullptr))
   , mDeleteWeaponDialog(new DeleteWeaponDialog(nullptr))
   , mCreateEngineDialog(new CreateEngineDialog(nullptr))
   , mEditEngineDialog(new EditEngineDialog(nullptr))
   , mDeleteEngineDialog(new DeleteEngineDialog(nullptr))
{
   mUIPtr->setupUi(this);

   connect(mUIPtr->mCreateAircraftButton, &QPushButton::clicked, this, &StartDialog::CreateAircraftClicked);
   connect(mUIPtr->mEditAircraftButton, &QPushButton::clicked, this, &StartDialog::EditAircraftClicked);
   connect(mUIPtr->mDeleteAircraftButton, &QPushButton::clicked, this, &StartDialog::DeleteAircraftClicked);

   connect(mUIPtr->mCreateWeaponButton, &QPushButton::clicked, this, &StartDialog::CreateWeaponClicked);
   connect(mUIPtr->mEditWeaponButton, &QPushButton::clicked, this, &StartDialog::EditWeaponClicked);
   connect(mUIPtr->mDeleteWeaponButton, &QPushButton::clicked, this, &StartDialog::DeleteWeaponClicked);

   connect(mUIPtr->mCreateEngineButton, &QPushButton::clicked, this, &StartDialog::CreateEngineClicked);
   connect(mUIPtr->mEditEngineButton, &QPushButton::clicked, this, &StartDialog::EditEngineClicked);
   connect(mUIPtr->mDeleteEngineButton, &QPushButton::clicked, this, &StartDialog::DeleteEngineClicked);

   connect(mUIPtr->mConvertBrawlerAircraftButton, &QPushButton::clicked, this, &StartDialog::ConvertBrawlerAircraftClicked);

   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &StartDialog::CancelClicked);
   connect(mUIPtr->mSettingsButton, &QPushButton::clicked, this, &StartDialog::SettingsClicked);
   connect(mUIPtr->mAboutButton, &QPushButton::clicked, this, &StartDialog::AboutClicked);

   // Note: Consider removing these buttons altogether from the .ui file
   mUIPtr->mDeleteAircraftButton->hide();
   mUIPtr->mDeleteEngineButton->hide();
   mUIPtr->mDeleteWeaponButton->hide();
}

StartDialog::~StartDialog()
{
}

void StartDialog::CreateAircraftClicked(bool aChecked)
{
   mCreateAircraftDialog->show();
}

void StartDialog::EditAircraftClicked(bool aChecked)
{
   mEditAircraftDialog->show();
}

void StartDialog::DeleteAircraftClicked(bool aChecked)
{
   mDeleteAircraftDialog->show();
}

void StartDialog::ConvertBrawlerAircraftClicked(bool aChecked)
{
   mConvertBrawlerAircraftDialog->show();
}

void StartDialog::CreateWeaponClicked(bool aChecked)
{
   mCreateWeaponDialog->show();
}

void StartDialog::EditWeaponClicked(bool aChecked)
{
   mEditWeaponDialog->show();
}

void StartDialog::DeleteWeaponClicked(bool aChecked)
{
   mDeleteWeaponDialog->show();
}

void StartDialog::CreateEngineClicked(bool aChecked)
{
   mCreateEngineDialog->show();
}

void StartDialog::EditEngineClicked(bool aChecked)
{
   // Previously, this would "show" the EngineDesignerWidget.
   // Now, it "shows" the EditEngineWidget instead and
   // after selecting the engine to edit, that widget/dialog
   // will need to create/show the EngineDesignerWidget.
   mEditEngineDialog->show();
}

void StartDialog::DeleteEngineClicked(bool aChecked)
{
   mDeleteEngineDialog->show();
}

void StartDialog::CancelClicked(bool aChecked)
{
   exit(0);
}

void StartDialog::SettingsClicked()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mw.ShowSettingsDialog();
}

void StartDialog::AboutClicked()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   mw.ShowAboutDialog();
}

void StartDialog::ChangeTheme(QString aThemeName)
{
   QString baseIconStr(QString(":/icons/%1/64x64/").arg(aThemeName));
   mUIPtr->AircraftIcon->setPixmap(QPixmap(baseIconStr + "aircraft.png"));
   mUIPtr->WeaponIcon->setPixmap(QPixmap(baseIconStr + "weapon.png"));
   mUIPtr->EngineIcon->setPixmap(QPixmap(baseIconStr + "engine.png"));
   mUIPtr->mSettingsButton->setIcon(QPixmap(baseIconStr + "gears.png"));
   mUIPtr->mAboutButton->setIcon(QPixmap(baseIconStr + "about.png"));

   mCreateAircraftDialog->ChangeTheme(aThemeName);
   mEditAircraftDialog->ChangeTheme(aThemeName);
   mDeleteAircraftDialog->ChangeTheme(aThemeName);
   mCreateWeaponDialog->ChangeTheme(aThemeName);
   mEditWeaponDialog->ChangeTheme(aThemeName);
   mDeleteWeaponDialog->ChangeTheme(aThemeName);
   mCreateEngineDialog->ChangeTheme(aThemeName);
   mEditEngineDialog->ChangeTheme(aThemeName);
   mDeleteEngineDialog->ChangeTheme(aThemeName);
}

} // namespace Designer
