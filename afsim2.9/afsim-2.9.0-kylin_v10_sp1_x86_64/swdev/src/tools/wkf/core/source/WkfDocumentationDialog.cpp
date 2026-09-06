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
#include "WkfDocumentationDialog.hpp"

#include <QDialogButtonBox>
#include <QFileInfo>
#include <QIcon>
#include <QLabel>

#include "WkfEnvironment.hpp"

wkf::DocumentationDialog::DocumentationDialog(MainWindow* parent, Qt::WindowFlags f /* = Qt::WindowFlags() */)
   : Dialog(parent, new QAction(QIcon::fromTheme("help"), "Documentation...", nullptr), "Help", f)
   , mIsPopulated(false)
{
   mUi.setupUi(this);
}

void wkf::DocumentationDialog::PopulateDialog()
{
   QFileInfo fi(wkfEnv.GetDocumentationDir());

   if (fi.isDir())
   {
      QString dir(fi.absoluteFilePath().append("/html/docs/"));
      CreateDocLink(dir + "main_page.html", "AFSIM Main Page", mUi.verticalLayout);
   }

   if (mUi.verticalLayout->count() == 0)
   {
      QLabel* labelPtr = new QLabel("<p style=\"font-size:10pt;\"><i>No documentation found</i></p>", this);
      mUi.verticalLayout->addWidget(labelPtr);
      mUi.verticalLayout->addStretch();
   }

   QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
   buttonBox->setObjectName(QStringLiteral("buttonBox"));
   buttonBox->setOrientation(Qt::Horizontal);
   buttonBox->setStandardButtons(QDialogButtonBox::Ok);
   connect(buttonBox, &QDialogButtonBox::accepted, this, [this] { hide(); });
   mUi.verticalLayout->addWidget(buttonBox);
}

void wkf::DocumentationDialog::CreateDocLink(const QString& link, const QString& text, QLayout* layout)
{
   if (QFileInfo::exists(link))
   {
      QLabel* labelPtr =
         new QLabel(QString(R"(<p style="font-size:10pt;"><a href="%1">%2</a></p>)").arg("file:///" + link, text), this);
      labelPtr->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
      labelPtr->setOpenExternalLinks(true);
      layout->addWidget(labelPtr);
   }
}

QVBoxLayout* wkf::DocumentationDialog::GetLayout() const
{
   return mUi.verticalLayout;
}

void wkf::DocumentationDialog::showEvent(QShowEvent* aEvent)
{
   if (!mIsPopulated)
   {
      PopulateDialog();
      mIsPopulated = true;
   }
}
