// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAboutDialog.hpp"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileInfo>
#include <QIcon>
#include <QPushButton>
#include <QTextStream>

#include "VaEnvironment.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlugin.hpp"

wkf::AboutDialog::AboutDialog(MainWindow* parent, Qt::WindowFlags f /* = Qt::WindowFlags() */)
   : Dialog(parent, new QAction(QIcon::fromTheme("about"), "About...", nullptr), "Help")
   , mArtworkDialog(nullptr)
   , mLicenseDialog(nullptr)
   , mSystemInfoDialog(nullptr)
{
   mUi.setupUi(this);
   mUi.additionalLabel->setText(qApp->applicationName() + " " + qApp->applicationVersion());
   mUi.pluginVersionLabel->setText(qApp->applicationName() + " Plugin API Version " +
                                   QString::number(WKF_PLUGIN_API_VERSION));
   mUi.buildLabel->setText(QString("Built on ") + __DATE__ + " " + __TIME__);

   connect(mUi.buttonBox, &QDialogButtonBox::accepted, this, [this] { close(); });
   connect(mUi.artworkPushButton, &QPushButton::clicked, this, &AboutDialog::ArtworkButtonHandler);
   connect(mUi.licensePushButton, &QPushButton::clicked, this, &AboutDialog::LicenseButtonHandler);
   connect(mUi.systemInfoPushButton, &QPushButton::clicked, this, &AboutDialog::SystemInfoButtonHandler);
}

wkf::AboutDialog::~AboutDialog()
{
   if (mArtworkDialog)
   {
      delete mArtworkDialog;
   }
   if (mLicenseDialog)
   {
      delete mLicenseDialog;
   }
}

QLabel* wkf::AboutDialog::GetInfoLabel()
{
   return mUi.additionalLabel;
}

void wkf::AboutDialog::ArtworkButtonHandler()
{
   if (!mArtworkDialog)
   {
      mArtworkDialog        = new QDialog(this);
      QVBoxLayout*  layout  = new QVBoxLayout();
      QTextBrowser* browser = new QTextBrowser(mArtworkDialog);
      layout->addWidget(browser);
      mArtworkDialog->setLayout(layout);
      mArtworkDialog->resize(300, 400);
      mArtworkDialog->setWindowTitle("Artwork Attributions");

      QDialogButtonBox* buttonBox = new QDialogButtonBox(mArtworkDialog);
      buttonBox->setObjectName(QStringLiteral("buttonBox"));
      buttonBox->setOrientation(Qt::Horizontal);
      buttonBox->setStandardButtons(QDialogButtonBox::Ok);
      connect(buttonBox, &QDialogButtonBox::accepted, this, [this] { mArtworkDialog->hide(); });

      layout->addWidget(buttonBox);

      PopulateArtworkDialog(*browser);
   }
   mArtworkDialog->show();
}

void wkf::AboutDialog::LicenseButtonHandler()
{
   QUrl url = "file:///" + wkfEnv.GetResourcesDir() + "/data/3rd_party_licenses";
   QDesktopServices::openUrl(url);
}

void wkf::AboutDialog::SystemInfoButtonHandler()
{
   if (!mSystemInfoDialog)
   {
      mSystemInfoDialog     = new QDialog(this);
      QVBoxLayout*  layout  = new QVBoxLayout();
      QTextBrowser* browser = new QTextBrowser(mSystemInfoDialog);
      layout->addWidget(browser);
      mSystemInfoDialog->setLayout(layout);
      mSystemInfoDialog->resize(300, 400);
      mSystemInfoDialog->setWindowTitle("System Information");

      QDialogButtonBox* buttonBox = new QDialogButtonBox(mSystemInfoDialog);
      buttonBox->setObjectName(QStringLiteral("buttonBox"));
      buttonBox->setOrientation(Qt::Horizontal);
      buttonBox->setStandardButtons(QDialogButtonBox::Ok);
      QPushButton* copy = buttonBox->addButton("Copy to clipboard", QDialogButtonBox::ActionRole);
      connect(buttonBox, &QDialogButtonBox::accepted, mSystemInfoDialog, &QDialog::accept);
      connect(copy, &QPushButton::clicked, [browser] { QApplication::clipboard()->setText(browser->toPlainText()); });

      layout->addWidget(buttonBox);

      PopulateSystemInfoDialog(*browser);
      QTextCursor cursor;
      cursor.setPosition(0);
      browser->setTextCursor(cursor);
   }
   mSystemInfoDialog->show();
}

void wkf::AboutDialog::PopulateArtworkDialog(QTextBrowser& aTextBrowser)
{
   QFile file(":/attributions.txt");
   file.open(QFile::ReadOnly | QFile::Text);
   QTextStream stream(&file);
   aTextBrowser.setText(stream.readAll());
   file.close();
}

void wkf::AboutDialog::PopulateSystemInfoDialog(QTextBrowser& aTextBrowser)
{
   aTextBrowser.append(qApp->applicationName() + " " + qApp->applicationVersion() + "\n");
   aTextBrowser.append(qApp->applicationName() + " Plugin API Version " + QString::number(WKF_PLUGIN_API_VERSION) + "\n");
   aTextBrowser.append(QString("Built on ") + __DATE__ + " " + __TIME__ + "\n");

   aTextBrowser.append("Operating System: " + QString(ut::system::platform));
   aTextBrowser.append("Compiler: " + QString(UT_PLUGIN_API_COMPILER_STRING));

   PopulateSystemInfoDialogApplication(aTextBrowser);

   aTextBrowser.append("\nOpenGL Version: ");
   aTextBrowser.append(vespa::VaEnvironment::Instance().GetGL_Version().c_str());
   aTextBrowser.append("\nOpenGL Shading Language Version: ");
   aTextBrowser.append(vespa::VaEnvironment::Instance().GetGLSL_Version().c_str());
   aTextBrowser.append("\nOpenGL Extensions: ");
   aTextBrowser.append(vespa::VaEnvironment::Instance().GetGL_Extensions().c_str());
}
