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

#ifndef MOVER_CREATOR_MAIN_WINDOW_HPP
#define MOVER_CREATOR_MAIN_WINDOW_HPP

#include <memory>
#include <QFileDialog>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>

#include "AboutDialog.hpp"
#include "SettingsDialog.hpp"
#include "VersionInfo.hpp"

namespace Ui
{
   class MoverCreatorMainWindow;
}

namespace Designer
{
   class MoverCreatorWidget;
   class MoverCreatorApplication;

   class MoverCreatorMainWindow : public QMainWindow
   {
         Q_OBJECT

      public:
         MoverCreatorMainWindow();
         ~MoverCreatorMainWindow();

         static MoverCreatorMainWindow& GetInstance();
         void Initialize(MoverCreatorApplication* aAppPtr);
         void ApplySettings();
         MoverCreatorWidget* GetMoverCreatorWidget() { return mMoverCreatorWidget; }

         void SetupThemes();
         void SetTheme(QString aThemeName);

         QString GetTheme() { return mCurrentTheme; }

         QPalette GetPalette() { return mPaletteMap[mCurrentTheme]; }
         static QString GetDataPath();

         void SetCurrentBaseName(QString aBaseName) { mCurrentBaseName = aBaseName; }
         QString GetCurrentBaseName() { return mCurrentBaseName; }

         void AppendToTitle(QString aString) { setWindowTitle(mBaseTitle + aString); }

         QSettings* GetSettings() { return mUserSettings; }
         void ShowSettingsDialog();
         void ShowAboutDialog();

         QString GetSiteDir()                   { return mSiteDirectory; }
         QString GetOsgConvPath()               { return mOsgConvPath; }
         QString GetOsgConvFileType()           { return mOsgConvFileType; }

         void CreateErrorBox(const QString& aErrorText);

      private slots:
         void HandleResetSettings();
         //void HandleOpenAction();
         //void HandleThemeAction();
         void HandleChangeThemeDark();
         void HandleChangeThemeLight();
         void HandleSetSiteDirectory();
         void HandleSetOsgConvPath();
         void HandleSetOsgConvFileTypeIVE();
         void HandleSetOsgConvFileTypeOSGB();
         void closeEvent(QCloseEvent* aEvent) override;

      private:
         MoverCreatorWidget*              mMoverCreatorWidget;
         static MoverCreatorMainWindow*   mInstancePtr;
         MoverCreatorApplication*         mAppPtr;
         QString                          mCurrentTheme;
         QMap<QString, QPalette>          mPaletteMap;
         QSettings*                       mUserSettings;
         QString                          mAppDataDir;        // Path to the OS's Application Data directory
         QString                          mCurrentBaseName;   // Current "base" name for the vehicle or engine
         QString                          mSiteDirectory;
         QString                          mOsgConvPath;
         QString                          mOsgConvFileType;
         QString                          mBaseTitle = QString("MoverCreator -- %1").arg(VersionInfo::GetVersionNumberString());
         std::unique_ptr<SettingsDialog>  mSettingsDialogPtr = nullptr;
         std::unique_ptr<AboutDialog>     mAboutDialogPtr = nullptr;
         Ui::MoverCreatorMainWindow*      mUIPtr;
   };
}
#endif // !MOVER_CREATOR_MAIN_WINDOW_HPP
