// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "RvMergeDialog.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include "RvEnvironment.hpp"

RvMerger::MergeDialog::MergeDialog(QDialog* aParent)
   : QDialog(aParent)
{
   this->setWindowTitle("Recording Merger");

   mProgressBar.close();
   mProgressBar.setRange(0, 0);
   mProgressBar.setWindowTitle("Merge in Process");
   mProgressBar.setModal(true);

   mUi.setupUi(this);

   connect(mUi.addButton,
           &QPushButton::clicked,
           [this]()
           {
              QStringList selected =
                 QFileDialog::getOpenFileNames(this, "Recording File", QString(), "Recordings (*.aer)");
              if (!selected.isEmpty())
              {
                 mUi.inputFilesList->addItems(selected);
                 mUi.masterComboBox->addItems(selected);
              }
           });

   connect(mUi.removeButton,
           &QPushButton::clicked,
           [this]()
           {
              QList<QListWidgetItem*> selected = mUi.inputFilesList->selectedItems();
              for (auto* item : selected)
              {
                 for (int i = 0; i < mUi.masterComboBox->count(); i++)
                 {
                    if (item->text() == mUi.masterComboBox->itemText(i))
                    {
                       mUi.masterComboBox->removeItem(i);
                       break;
                    }
                 }

                 delete item;
              }
           });

   connect(mUi.masterHelpButton,
           &QToolButton::clicked,
           [this]()
           {
              QMessageBox::about(
                 nullptr,
                 "Master Recording - Help",
                 "The master recording is the one which will be used for date, time, and classification.");
           });

   connect(mUi.browseButton,
           &QPushButton::clicked,
           [this]()
           {
              QString selected = QFileDialog::getSaveFileName(this, "Target File", QString(), "Recordings (*.aer)");
              if (!selected.isEmpty())
              {
                 mUi.outputLineEdit->setText(selected);
              }
           });

   connect(mUi.buttonBox, &QDialogButtonBox::accepted, this, &MergeDialog::Accept);
   connect(mUi.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::close);

   connect(&mProgressBar, &QProgressDialog::canceled, this, &MergeDialog::MergeCanceled);
}

void RvMerger::MergeDialog::Reset() noexcept
{
   mUi.inputFilesList->clear();
   mUi.outputLineEdit->clear();
   mProgressBar.close();
}

void RvMerger::MergeDialog::Abort() noexcept
{
   mProgressBar.close();
}

void RvMerger::MergeDialog::MergeFinished()
{
   if (mProgressBar.isVisible())
   {
      mProgressBar.close();
      if (QMessageBox::StandardButton::Yes ==
          QMessageBox::question(
             nullptr,
             "Merge Finished",
             QString("The final recording is stored in \"%1\". Open it now?").arg(mUi.outputLineEdit->text())))
      {
         rvEnv.OpenEventRecording(mUi.outputLineEdit->text());
      }
      close();
   }
}

void RvMerger::MergeDialog::Accept()
{
   QStringList files;

   QString masterFile = mUi.masterComboBox->currentText();
   files += masterFile;

   for (int i = 0; mUi.inputFilesList->item(i); i++)
   {
      QString file = mUi.inputFilesList->item(i)->text();
      if (file != masterFile)
      {
         files += file;
      }
   }

   mProgressBar.open();

   emit Merge(files, mUi.outputLineEdit->text());
}
