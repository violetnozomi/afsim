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

#include "AboutDialog.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "VersionInfo.hpp"
#include "WsfVersion.hpp"
#include "UtRunEnvManager.hpp"
#include "ui_AboutDialog.h"

namespace Designer
{

AboutDialog::AboutDialog(QWidget* aParent,
                         Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
   , mUIPtr(new Ui::AboutDialog)
{
   // About Section
   mUIPtr->setupUi(this);
   mUIPtr->mApplicationLabel->setText("Application:  AFSIM Mover Creator (MC)");
   mUIPtr->mVersionLabel->setText(QString("Version:  %1").arg(VersionInfo::GetVersionNumberString()));
   mUIPtr->mBuildLabel->setText(QString("Build Date:  ") + __DATE__ + " " + __TIME__);

   connect(mUIPtr->mButtonBox, &QDialogButtonBox::accepted, this, [this] { close(); });
   connect(mUIPtr->mArtworkPushButton, &QPushButton::clicked, this, &AboutDialog::HandleArtworkButtonClicked);
   connect(mUIPtr->mLicensePushButton, &QPushButton::clicked, this, &AboutDialog::HandleLicenseButtonClicked);
   connect(mUIPtr->mSystemInfoPushButton, &QPushButton::clicked, this, &AboutDialog::HandleSystemInfoButtonClicked);
}

AboutDialog::~AboutDialog()
{
   if (mArtworkDialog != nullptr)
   {
      delete mArtworkDialog;
   }
   if (mLicenseDialog != nullptr)
   {
      delete mLicenseDialog;
   }
   delete mUIPtr;
}

void AboutDialog::HandleArtworkButtonClicked()
{
   if (mArtworkDialog == nullptr)
   {
      mArtworkDialog = new QDialog(this);
      QVBoxLayout* layout = new QVBoxLayout();
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

void AboutDialog::PopulateArtworkDialog(QTextBrowser& aTextBrowser)
{
   QFile file(":/attributions.txt");
   file.open(QFile::ReadOnly | QFile::Text);
   QTextStream stream(&file);
   aTextBrowser.setText(stream.readAll());
   file.close();
}

QString AboutDialog::GetLicensesDir()
{
   QDir licenses(qApp->applicationDirPath());
   licenses.cdUp();

   // If the resources folder exists here, we're running from an installed build.
   // This is where the licenses are stored.
   if (licenses.exists("resources"))
   {
      return licenses.absoluteFilePath("resources/data/3rd_party_licenses/");
   }
   else
   {
      return licenses.absoluteFilePath("wsf_install/resources/data/3rd_party_licenses/");
   }
}

void AboutDialog::HandleLicenseButtonClicked()
{
   if (QDir(GetLicensesDir()).exists())
   {
      QUrl url = "file:///" + GetLicensesDir();
      QDesktopServices::openUrl(url);
   }
   else
   {
      MoverCreatorMainWindow().CreateErrorBox("Unable to find licenses directory!");
   }
}

void AboutDialog::HandleSystemInfoButtonClicked()
{
   if (!mSystemInfoDialog)
   {
      mSystemInfoDialog = new QDialog(this);
      QVBoxLayout* layout = new QVBoxLayout();
      QTextBrowser* browser = new QTextBrowser(mSystemInfoDialog);
      layout->addWidget(browser);
      mSystemInfoDialog->setLayout(layout);
      mSystemInfoDialog->resize(300, 400);
      mSystemInfoDialog->setWindowTitle("System Information");

      QDialogButtonBox* buttonBox = new QDialogButtonBox(mSystemInfoDialog);
      buttonBox->setObjectName(QStringLiteral("buttonBox"));
      buttonBox->setOrientation(Qt::Horizontal);
      buttonBox->setStandardButtons(QDialogButtonBox::Ok);
      layout->addWidget(buttonBox);
      connect(buttonBox, &QDialogButtonBox::accepted, this, [this] { mSystemInfoDialog->hide(); });

      PopulateSystemInfoDialog(*browser);
      QTextCursor cursor;
      cursor.setPosition(0);
      browser->setTextCursor(cursor);
   }
   mSystemInfoDialog->show();
}

void AboutDialog::PopulateSystemInfoDialog(QTextBrowser& aTextBrowser)
{
   aTextBrowser.append(QString("Mover Creator %1").arg(VersionInfo::GetVersionNumberString()));
   aTextBrowser.append(QString("Built on ") + __DATE__ + " " + __TIME__ + "\n");

#ifdef WIN32
   aTextBrowser.append("\nOS: Windows\n");
#else
   aTextBrowser.append("\nOS: Linux\n");
#endif

   // Create an offscreen OpenGL window so the glGet* functions don't return null.
   // There has to be a better solution, but for now, this works.
   QOpenGLContext tempContext;
   tempContext.create();
   QOffscreenSurface tempSurface;
   tempSurface.create();
   tempContext.makeCurrent(&tempSurface);

   QString glVersion((char*)glGetString(GL_VERSION));
   QString glSLVersion((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
   QString glExtensions((char*)glGetString(GL_EXTENSIONS));

   aTextBrowser.append("\nOpenGL Version: ");
   aTextBrowser.append(glVersion);
   aTextBrowser.append("\nOpenGL Shading Language Version: ");
   aTextBrowser.append(glSLVersion);
   aTextBrowser.append("\nOpenGL Extensions: ");
   aTextBrowser.append(glExtensions);
}

void AboutDialog::showEvent(QShowEvent* aEvent)
{
   // Populate the documentation links

   QString docDir = QString::fromStdString(UtRunEnvManager::GetRunPath() + "/../documentation/html/docs/");
   QColor col = MoverCreatorMainWindow::GetInstance().GetPalette().color(QPalette::Link);

   QString mainPageDir = docDir + "mover_creator.html";
   QString usersGuideDir = docDir + "mc_users_guide.html";
   QString refGuideDir = docDir + "mc_reference_guide.html";
   QString changelogDir = docDir + "mc_change_log.html";

   if (QFileInfo::exists(mainPageDir))
   {
      mUIPtr->mMainPageLabel->setText(QString(R"(<a href="%1" style="text-decoration:none; color:%2">%3</a>)").arg("file:///" + mainPageDir, col.name(), "Main Page"));
      mUIPtr->mMainPageLabel->setOpenExternalLinks(true);
      mUIPtr->mMainPageLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
   }
   if (QFileInfo::exists(usersGuideDir))
   {
      mUIPtr->mUsersGuideLabel->setText(QString(R"(<a href="%1" style="text-decoration:none; color:%2">%3</a>)").arg("file:///" + usersGuideDir, col.name(), "User's Guide"));
      mUIPtr->mUsersGuideLabel->setOpenExternalLinks(true);
      mUIPtr->mUsersGuideLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
   }
   if (QFileInfo::exists(refGuideDir))
   {
      mUIPtr->mReferenceGuideLabel->setText(QString(R"(<a href="%1" style="text-decoration:none; color:%2">%3</a>)").arg("file:///" + refGuideDir, col.name(), "Reference Guide"));
      mUIPtr->mReferenceGuideLabel->setOpenExternalLinks(true);
      mUIPtr->mReferenceGuideLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
   }
   if (QFileInfo::exists(changelogDir))
   {
      mUIPtr->mChangeLogLabel->setText(QString(R"(<a href="%1" style="text-decoration:none; color:%2">%3</a>)").arg("file:///" + changelogDir, col.name(), "Change Log"));
      mUIPtr->mChangeLogLabel->setOpenExternalLinks(true);
      mUIPtr->mChangeLogLabel->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
   }

}

} // namespace Designer
