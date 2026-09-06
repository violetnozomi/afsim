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

#ifndef ABOUT_DIALOG_HPP
#define ABOUT_DIALOG_HPP

#include <QApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QTextBrowser>

namespace Ui
{
   class AboutDialog;
}

namespace Designer
{
   class AboutDialog : public QDialog
   {
         Q_OBJECT

      public:
         explicit AboutDialog(QWidget*        aParent,
                              Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~AboutDialog();
         void showEvent(QShowEvent* aEvent);
         Ui::AboutDialog* getUIPtr() { return mUIPtr; }

      private:
         QString GetLicensesDir();

         void HandleArtworkButtonClicked();
         void HandleLicenseButtonClicked();
         void HandleSystemInfoButtonClicked();

         void PopulateArtworkDialog(QTextBrowser& aTextBrowser);
         void PopulateSystemInfoDialog(QTextBrowser& aTextBrowser);

         Ui::AboutDialog* mUIPtr;

         QDialog* mArtworkDialog = nullptr;
         QDialog* mLicenseDialog = nullptr;
         QDialog* mSystemInfoDialog = nullptr;
   };
}

#endif // !ABOUT_DIALOG_HPP