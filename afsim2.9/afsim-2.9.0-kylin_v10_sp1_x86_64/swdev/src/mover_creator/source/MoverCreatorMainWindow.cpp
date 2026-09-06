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

#include "MoverCreatorMainWindow.hpp"

#include <QStandardPaths>
#include <QToolTip>

#include "MoverCreatorApplication.hpp"
#include "MoverCreatorWidget.hpp"
#include "SettingsDialog.hpp"
#include "ui_MoverCreatorMainWindow.h"
#include "ui_SettingsDialog.h"
#include "UtLog.hpp"
#include "UtMemory.hpp"
#include "UtPath.hpp"

namespace Designer
{

MoverCreatorMainWindow* MoverCreatorMainWindow::mInstancePtr = nullptr;

MoverCreatorMainWindow::MoverCreatorMainWindow()
   : QMainWindow()
   , mSiteDirectory("")
   , mOsgConvPath("")
   , mUIPtr(new Ui::MoverCreatorMainWindow())
{
   mUIPtr->setupUi(this);
   mMoverCreatorWidget = mUIPtr->mMoverCreatorWidget;
   setWindowTitle(mBaseTitle);

   mAppDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
   mUserSettings = new QSettings(mAppDataDir + "//Settings.ini", QSettings::IniFormat, this);
   mSettingsDialogPtr = ut::make_unique<SettingsDialog>(this);
   mAboutDialogPtr = ut::make_unique<AboutDialog>(this);

   connect(mSettingsDialogPtr.get(), &SettingsDialog::ChangeThemeDark, this, &MoverCreatorMainWindow::HandleChangeThemeDark);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::ChangeThemeLight, this, &MoverCreatorMainWindow::HandleChangeThemeLight);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::ResetSettings, this, &MoverCreatorMainWindow::HandleResetSettings);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::SetSiteDirectory, this, &MoverCreatorMainWindow::HandleSetSiteDirectory);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::SetOsgConvPath, this, &MoverCreatorMainWindow::HandleSetOsgConvPath);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::SetOsgConvFileTypeIVE, this, &MoverCreatorMainWindow::HandleSetOsgConvFileTypeIVE);
   connect(mSettingsDialogPtr.get(), &SettingsDialog::SetOsgConvFileTypeOSGB, this, &MoverCreatorMainWindow::HandleSetOsgConvFileTypeOSGB);
}

MoverCreatorMainWindow::~MoverCreatorMainWindow()
{
   delete mUIPtr;
}

MoverCreatorMainWindow& MoverCreatorMainWindow::GetInstance()
{
   if (mInstancePtr == nullptr)
   {
      mInstancePtr = new MoverCreatorMainWindow();
   }

   return *mInstancePtr;
}

void MoverCreatorMainWindow::Initialize(MoverCreatorApplication* aAppPtr)
{
   mAppPtr = aAppPtr;
   mMoverCreatorWidget->Initialize();
   ApplySettings();
}

void MoverCreatorMainWindow::ApplySettings()
{
   //QIcon::setThemeName("Light");
   mCurrentTheme = mUserSettings->value("theme").toString();
   if (mCurrentTheme == "")
   {
      mCurrentTheme = "Dark";
   }
   SetupThemes();
   SetTheme(mCurrentTheme);
   mMoverCreatorWidget->ChangeTheme(mCurrentTheme);

   // Set the default values based on preferences
   if (mSettingsDialogPtr != nullptr)
   {
      Ui::SettingsDialog* settingsUIPtr = mSettingsDialogPtr->getUIPtr();

      // Restore the path values set by the user
      // If the paths no longer exist, store them as blank values
      mSiteDirectory = mUserSettings->value("siteDir").toString();
      if (!QFile(mSiteDirectory).exists())
      {
         ut::log::info() << "Site directory not found; using resources/site/ instead.";
         mSiteDirectory = "";
         settingsUIPtr->mSiteDir->setText("No directory specified...");
      }
      else
      {
         settingsUIPtr->mSiteDir->setText(mSiteDirectory);
      }
      mOsgConvPath = mUserSettings->value("osgConvPath").toString();
      if (!QFile(mOsgConvPath).exists())
      {
         ut::log::info() << "OsgConv not found.";
         mOsgConvPath = "";
         settingsUIPtr->mOsgConv->setText("No file specified...");
      }
      else
      {
         settingsUIPtr->mOsgConv->setText(mOsgConvPath);
      }

      // Restore the previously selected file type preference
      mOsgConvFileType = mUserSettings->value("osgConvFileType").toString();

      // Theme preference
      if (mCurrentTheme == "Dark")
      {
         settingsUIPtr->mDarkThemeButton->setChecked(true);
      }
      else
      {
         settingsUIPtr->mLightThemeButton->setChecked(true);
      }

      // OsgConv file type preference
      if (mOsgConvFileType == ".ive")
      {
         settingsUIPtr->mIVEButton->setChecked(true);
      }
      else
      {
         settingsUIPtr->mOSGBButton->setChecked(true);
      }
   }
}

void MoverCreatorMainWindow::HandleResetSettings()
{
   // Reset text boxes
   Ui::SettingsDialog* settingsUIPtr = mSettingsDialogPtr->getUIPtr();
   if (settingsUIPtr != nullptr)
   {
      settingsUIPtr->mSiteDir->setText("No directory specified...");
      settingsUIPtr->mOsgConv->setText("No file specified...");
   }

   mUserSettings->setValue("theme", "Dark");
   mUserSettings->setValue("siteDir", "");
   mUserSettings->setValue("osgConvPath", "");
   mUserSettings->setValue("osgConvFileType", ".osgb");
   ApplySettings();
}

void MoverCreatorMainWindow::HandleChangeThemeLight()
{
   mCurrentTheme = "Light";
   SetTheme(mCurrentTheme);
   mMoverCreatorWidget->ChangeTheme(mCurrentTheme);
}

void MoverCreatorMainWindow::HandleChangeThemeDark()
{
   mCurrentTheme = "Dark";
   SetTheme(mCurrentTheme);
   mMoverCreatorWidget->ChangeTheme(mCurrentTheme);
}

void MoverCreatorMainWindow::HandleSetSiteDirectory()
{
   mSiteDirectory = QFileDialog::getExistingDirectory(this, "Open Site Directory");

   if (!mSiteDirectory.endsWith("/site"))
   {
      // Ask the user if they would like to create a site directory here
      QMessageBox addSiteDir;
      addSiteDir.setText("The selected directory isn't a site folder.");
      addSiteDir.setInformativeText("Would you like to create one here?");
      addSiteDir.addButton(QMessageBox::Yes);
      addSiteDir.addButton(QMessageBox::No);

      // Workaround for a Qt bug that prevents resizing a QMessageBox
      QSpacerItem* width = new QSpacerItem(355, 0);
      QSpacerItem* height = new QSpacerItem(0, 25);
      QGridLayout* layout = (QGridLayout*)addSiteDir.layout();
      layout->addItem(width, 0, 1);
      layout->addItem(height, 1, 0);

      int decision = addSiteDir.exec();

      QString newSiteDirPath(mSiteDirectory + "/site");
      QDir newSiteDir(newSiteDirPath);

      switch (decision)
      {
         case QMessageBox::Yes:
            if (!newSiteDir.exists())
            {
               QDir().mkdir(newSiteDirPath);
            }
            mSiteDirectory = newSiteDirPath;
            break;
         case QMessageBox::No:
            break;
         default:
            break;
      }
   }

   Ui::SettingsDialog* settingsUIPtr = mSettingsDialogPtr->getUIPtr();
   if (settingsUIPtr != nullptr)
   {
      settingsUIPtr->mSiteDir->setText(mSiteDirectory);
   }
}

void MoverCreatorMainWindow::HandleSetOsgConvPath()
{
   mOsgConvPath = QFileDialog::getOpenFileName(this, "Open OsgConv", tr("osgconv*"));

   Ui::SettingsDialog* settingsUIPtr = mSettingsDialogPtr->getUIPtr();
   if (settingsUIPtr != nullptr)
   {
      settingsUIPtr->mOsgConv->setText(mOsgConvPath);
   }
}

void MoverCreatorMainWindow::HandleSetOsgConvFileTypeIVE()
{
   mOsgConvFileType = ".ive";
}

void MoverCreatorMainWindow::HandleSetOsgConvFileTypeOSGB()
{
   mOsgConvFileType = ".osgb";
}

void MoverCreatorMainWindow::SetupThemes()
{
   QPalette lightPalette = qApp->style()->standardPalette();
   lightPalette.setColor(QPalette::Link, QColor(0, 99, 177));
   lightPalette.setColor(QPalette::Highlight, QColor(0, 99, 177));
   mPaletteMap.insert("Light", lightPalette);

   QPalette darkPalette(lightPalette);
   darkPalette.setColor(QPalette::Active, QPalette::Base, QColor(30, 30, 32));
   darkPalette.setColor(QPalette::Inactive, QPalette::Base, QColor(30, 30, 32));
   darkPalette.setColor(QPalette::Disabled, QPalette::Base, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Button, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Inactive, QPalette::Button, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::ButtonText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(40, 40, 42));
   darkPalette.setColor(QPalette::Active, QPalette::Light, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::Light, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::Light, QColor(220, 220, 222, 0)); // alpha set to 0 to remove unwanted shadow effect for disabled menu items
   darkPalette.setColor(QPalette::Active, QPalette::Text, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::Text, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Window, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Inactive, QPalette::Window, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Disabled, QPalette::Window, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::WindowText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(45, 45, 47));
   darkPalette.setColor(QPalette::Active, QPalette::Shadow, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Inactive, QPalette::Shadow, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Disabled, QPalette::Shadow, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Dark, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Inactive, QPalette::Dark, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Disabled, QPalette::Dark, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Active, QPalette::Light, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Inactive, QPalette::Light, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::Disabled, QPalette::Light, QColor(78, 78, 80));


   darkPalette.setColor(QPalette::ToolTipText, QColor(78, 78, 80));
   darkPalette.setColor(QPalette::ToolTipBase, QColor(220, 220, 222));
   darkPalette.setColor(QPalette::AlternateBase, QColor(78, 78, 80));
   mPaletteMap.insert("Dark", darkPalette);
}

void MoverCreatorMainWindow::SetTheme(QString aThemeName)
{
   qApp->setPalette(mPaletteMap[aThemeName]);
   emit qApp->paletteChanged(mPaletteMap[aThemeName]);

   qApp->setStyleSheet(QString("                                \
      QAbstractItemView::indicator,                                \
      QCheckBox::indicator,                                        \
      QGroupBox::indicator,                                        \
      QRadioButton::indicator                                      \
      {                                                            \
          height: 12px;                                            \
          width:  12px;                                            \
      }                                                            \
                                                                   \
      QMenu::separator, QToolBar::separator                        \
      {                                                            \
          background: palette(Midlight);                           \
          width:   1px;                                            \
          height:  1px;                                            \
          margin:  4px;                                            \
      }                                                            \
                                                                   \
      QTabBar::tab:hover, QToolBox::tab:hover                      \
      {                                                            \
          color: palette(Highlight);                               \
      }                                                            \
      QTabBar::tab:selected {                                      \
         background: #0063b1;                                      \
         color: white;                                             \
         border-bottom-left-radius: 2px;                           \
         border-bottom-right-radius: 2px;                          \
         min-width: 80px;                                          \
      }                                                            \
      QTabBar::tab:selected:hover{                                 \
         color: white;                                             \
      }                                                            \
      QTabBar::tab{                                                \
         min-width: 80px;                                          \
         padding-left: 16px; padding-right: 16px;                  \
         padding-top: 9px; padding-bottom: 11px;                   \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::checked,                       \
      QCheckBox::indicator:checked,                                \
      QGroupBox::indicator:checked                                 \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_checked.png);       \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::unchecked,                     \
      QCheckBox::indicator:unchecked,                              \
      QGroupBox::indicator:unchecked                               \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_unchecked.png);     \
      }                                                            \
                                                                   \
      QAbstractItemView::indicator::indeterminate,                 \
      QCheckBox::indicator:indeterminate,                          \
      QGroupBox::indicator:indeterminate                           \
      {                                                            \
          image: url(:/icons/%1/64x64/checkbox_indeterminate.png); \
      }                                                            \
                                                                   \
      QRadioButton::indicator:checked                              \
      {                                                            \
          image: url(:/icons/%1/64x64/radiobutton_checked.png);    \
      }                                                            \
                                                                   \
      QRadioButton::indicator:unchecked                            \
      {                                                            \
          image: url(:/icons/%1/64x64/radiobutton_unchecked.png);  \
      }                                                            \
                                                                   \
      QPushButton { padding-left: 14px; padding-right: 14px;       \
                    padding-top: 7px; padding-bottom: 7px; }       \
      ").arg(aThemeName));

   QIcon::setThemeName(aThemeName);
   QToolTip::setPalette(mPaletteMap[aThemeName]);
}

void MoverCreatorMainWindow::closeEvent(QCloseEvent* aEvent)
{
   // Check if the user typed a path manually
   Ui::SettingsDialog* settingsUIPtr = mSettingsDialogPtr->getUIPtr();
   if (settingsUIPtr != nullptr)
   {
      if (settingsUIPtr->mSiteDir->isModified())
      {
         mSiteDirectory = settingsUIPtr->mSiteDir->text();
      }
      if (settingsUIPtr->mOsgConv->isModified())
      {
         mOsgConvPath = settingsUIPtr->mOsgConv->text();
      }
   }

   mUserSettings->setValue("theme", mCurrentTheme);
   mUserSettings->setValue("siteDir", mSiteDirectory);
   mUserSettings->setValue("osgConvPath", mOsgConvPath);
   mUserSettings->setValue("osgConvFileType", mOsgConvFileType);
}

QString MoverCreatorMainWindow::GetDataPath()
{
   //Look up resources directory to determine if this is a development build running in Visual Studio or an installed build
   QString baseDirPath = "";
   UtPath path = UtPath::GetExePath();
   path.Up(); // Strip executable name
   path.Up(); // Strip bin directory
   std::string pathStr = path.GetSystemPath() + "/resources/data/mover_creator";
   UtPath resourcePath = pathStr;
   //This path should exist in all installed loads
   if (resourcePath.Exists())
   {
      baseDirPath = QString::fromStdString(pathStr);
   }
   else
   {
      // Provide a "dev environment message 1 time only"
      static bool msgNotShown = true;
      if (msgNotShown)
      {
         ut::log::info() << "Directory " << pathStr << " not found, assuming development build paths.";
         msgNotShown = false;
      }

      //Use the developer path to the resources directory
      const char* sourceRoot = ::getenv("SOURCE_ROOT");
      if (sourceRoot)
      {
         baseDirPath = QByteArray::fromStdString(sourceRoot) + "/../afsim_shared/mover_creator/data";
      }
      else
      {
         ut::log::info() << "Resource path not found.";
      }
   }
   return baseDirPath;
}

void MoverCreatorMainWindow::ShowSettingsDialog()
{
   if (mSettingsDialogPtr != nullptr)
   {
      mSettingsDialogPtr->show();
   }
}

void MoverCreatorMainWindow::ShowAboutDialog()
{
   if (mAboutDialogPtr != nullptr)
   {
      mAboutDialogPtr->show();
   }
}

void MoverCreatorMainWindow::CreateErrorBox(const QString& aErrorText)
{
   QMessageBox msgBox;
   msgBox.setText(aErrorText);
   msgBox.addButton(QMessageBox::Ok);

   // Workaround for a Qt bug that prevents resizing a QMessageBox
   QSpacerItem* width = new QSpacerItem(355, 0);
   QSpacerItem* height = new QSpacerItem(0, 25);
   QGridLayout* layout = (QGridLayout*)msgBox.layout();
   layout->addItem(width, 0, 1);
   layout->addItem(height, 1, 0);

   msgBox.exec();
}

} // namespace Designer
