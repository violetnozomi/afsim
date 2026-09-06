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

#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

#include <QDialog>

namespace Ui
{
   class SettingsDialog;
}

namespace Designer
{
   class SettingsDialog : public QDialog
   {
         Q_OBJECT
      public:
         explicit SettingsDialog(QWidget*        aParent,
                                 Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~SettingsDialog();
         void showEvent(QShowEvent* aEvent);
         Ui::SettingsDialog* getUIPtr() { return mUIPtr; }

      private:
      signals:
         void ResetSettings();
         void ChangeThemeDark();
         void ChangeThemeLight();
         void SetSiteDirectory();
         void SetOsgConvPath();
         void SetOsgConvFileTypeIVE();
         void SetOsgConvFileTypeOSGB();

      private:
         Ui::SettingsDialog* mUIPtr;
   };
}
#endif // !SETTINGS_DIALOG_HPP


