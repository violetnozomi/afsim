// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfExportHelper.hpp"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

#include "WkfEnvironment.hpp"

const QString cSTARTUP = "__startup";

void wkf::ExportHelper::WriteFinalState() const
{
   emit WriteState(cSTARTUP);
}

void wkf::ExportHelper::ReadInitialState()
{
   if (mListExports().contains(cSTARTUP))
   {
      emit ReadState(cSTARTUP);
   }
}

void wkf::ExportHelper::BuildContextMenu(QMenu& aMenu)
{
   auto* action = new QAction("Export " + mLabel, &aMenu);
   connect(action, &QAction::triggered, this, &ExportHelper::ExportSettingsDialog);
   aMenu.addAction(action);

   action = new QAction("Import " + mLabel, &aMenu);
   connect(action, &QAction::triggered, this, &ExportHelper::ImportSettingsDialog);
   aMenu.addAction(action);
}

void wkf::ExportHelper::ExportSettingsDialog() const
{
   QString text;
   while (true)
   {
      bool ok;
      text = QInputDialog::getText(nullptr,
                                   "Export " + mLabel,
                                   "Name: ",
                                   QLineEdit::Normal,
                                   text,
                                   &ok,
                                   Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

      // Return if user clicked Cancel.
      if (!ok)
      {
         return;
      }
      //! Check for invalid characters in string.
      else if (text.contains('/') || text.contains('\\') || text.contains('=') || text.contains('@'))
      {
         QMessageBox::warning(nullptr, "Invalid Name", "Export names may not contain the following characters:\n/, \\, =, @");
      }
      // If an export with that name does not exist, export it.
      else if (!mListExports().contains(text, Qt::CaseInsensitive))
      {
         emit WriteState(text);
         return;
      }
      // If the name does exist, prompt the user to overwrite it.
      // If yes, export and return.
      // If no, rinse and repeat.
      else if (QMessageBox::question(nullptr, "Name Taken", "An export with that name already exists. Replace it?") ==
               QMessageBox::StandardButton::Yes)
      {
         emit WriteState(text);
         return;
      }
   }
}

void wkf::ExportHelper::ImportSettingsDialog()
{
   QDialog dialog;

   dialog.setWindowTitle("Import " + mLabel);
   dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);

   auto* vLayout   = new QVBoxLayout();
   auto* comboBox  = new QComboBox();
   auto* buttonBox = new QDialogButtonBox();

   // Adjust combo box
   for (const auto& exportedPlot : mListExports())
   {
      if (exportedPlot != cSTARTUP)
      {
         comboBox->addItem(exportedPlot);
      }
   }
   auto sp = comboBox->sizePolicy();
   sp.setHorizontalStretch(1);
   comboBox->setSizePolicy(sp);

   // Set dialog buttons
   auto* okButton     = buttonBox->addButton("&OK", QDialogButtonBox::NoRole);
   auto* deleteButton = buttonBox->addButton("&Delete", QDialogButtonBox::NoRole);
   auto* cancelButton = buttonBox->addButton("&Cancel", QDialogButtonBox::NoRole);

   if (comboBox->count() == 0)
   {
      deleteButton->setDisabled(true);
      okButton->setDisabled(true);
   }

   okButton->connect(okButton,
                     &QPushButton::clicked,
                     [this, &dialog, comboBox]()
                     {
                        emit ReadState(comboBox->currentText());
                        dialog.close();
                     });

   deleteButton->connect(deleteButton,
                         &QPushButton::clicked,
                         [this, &dialog, comboBox, deleteButton, okButton]()
                         {
                            if (QMessageBox::question(nullptr,
                                                      "Delete Export",
                                                      "Are you sure you want to delete " + comboBox->currentText() +
                                                         "?") == QMessageBox::Button::Yes)
                            {
                               mDeleteExport(comboBox->currentText());
                               comboBox->removeItem(comboBox->currentIndex());
                               if (comboBox->count() == 0)
                               {
                                  deleteButton->setDisabled(true);
                                  okButton->setDisabled(true);
                               }
                            }
                         });

   cancelButton->connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::close);

   // Add items to layout
   vLayout->addWidget(new QLabel("Name: "));
   vLayout->addWidget(comboBox);
   vLayout->addWidget(buttonBox);

   dialog.setLayout(vLayout);

   dialog.exec();
}
