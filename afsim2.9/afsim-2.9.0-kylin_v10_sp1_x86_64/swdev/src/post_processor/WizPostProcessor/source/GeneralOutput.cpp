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

#include "GeneralOutput.hpp"

#include <QFileDialog>
#include <QSpinBox>
#include <QStandardPaths>

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#include "Project.hpp"
#include "Signals.hpp"

PostProcessor::GeneralOutput::GeneralOutput()
{
   // Need to use ViSignals::ProjectOpen rather than ViProject::ProjectOpened,
   // because project directories are not yet set when the latter gets called.
   mUtCallbacks += wizSignals->ProjectOpen.Connect(&GeneralOutput::ProjectOpened, this);

   mUI.setupUi(this);

   mUI.mReportName->setText(mReportName);
   mUI.mOutputPath->setText(mReportOutputPath);
   mUI.mDataFile->setText(mOutputDataFile);

   connect(mUI.mOutputPath, &QLineEdit::textEdited, this, [this](const QString & aReportOutputPath) { mReportOutputPath = aReportOutputPath; });
   connect(mUI.mDataFile,   &QLineEdit::textEdited, this, [this](const QString & aOutputDataPath)   { mOutputDataFile = aOutputDataPath; });
   connect(mUI.mReportName, &QLineEdit::textEdited, this, [this](const QString & aReportName)       { mReportName = aReportName; });

   connect(mUI.mReportOutputBrowse, &QPushButton::pressed, this, &GeneralOutput::ReportOutputBrowsePressed);
   connect(mUI.mDataFileBrowse, &QPushButton::pressed, this, &GeneralOutput::DataFileBrowsePressed);
}

QString PostProcessor::GeneralOutput::GetConfigName() const
{
   return mReportOutputPath + "/" + mReportName + "_config.txt";
}

int PostProcessor::GeneralOutput::GetMonteCarloStart() const
{
   return mUI.mMonteCarloStart->value();
}

int PostProcessor::GeneralOutput::GetMonteCarloEnd() const
{
   return mUI.mMonteCarloEnd->value();
}

void PostProcessor::GeneralOutput::WriteData(std::ostream& aStream)
{
   static constexpr char newl = '\n';
   aStream << "report_name \"" << mReportName.toStdString();

   if (mUI.mCB_AppendDate->isChecked())
   {
      aStream << "_%D";
   }

   if (mUI.mCB_AppendTime->isChecked())
   {
      aStream << "_%T";
   }
   aStream << '"' << newl;

   aStream << "data_file \"" << mOutputDataFile.toStdString() << '"' << newl
           << "output_directory \"" << mReportOutputPath.toStdString() << '"' << newl
           << "monte_carlo_start " << mUI.mMonteCarloStart->value() << newl
           << "monte_carlo_end " << mUI.mMonteCarloEnd->value() << newl
           << "single_output_file " << std::boolalpha << mUI.mCheckBoxSOF->isChecked() << newl
           << "use_monte_carlo " << mUI.mGB_MonteCarlo->isChecked() << std::noboolalpha << newl;
}

void PostProcessor::GeneralOutput::ReportOutputBrowsePressed()
{
   QFileDialog dlg(nullptr, "Select Directory", mReportOutputPath, tr("Folder"));
   dlg.setAcceptMode(QFileDialog::AcceptOpen);
   dlg.setFileMode(QFileDialog::Directory);

   QStringList selFiles;
   if (dlg.exec())
   {
      selFiles = dlg.selectedFiles();
      if (selFiles.size() == 1)
      {
         mUI.mOutputPath->setText(selFiles[0]);
         mReportOutputPath = selFiles[0];
      }
   }
}

void PostProcessor::GeneralOutput::DataFileBrowsePressed()
{
   QFileDialog dlg(nullptr, "Select File", mOutputPath, tr("CSV files (*.csv);;Text files (*.txt);;All files (*.*)"));
   dlg.setAcceptMode(QFileDialog::AcceptOpen);
   dlg.setFileMode(QFileDialog::ExistingFile);

   QStringList selFiles;
   if (dlg.exec())
   {
      selFiles = dlg.selectedFiles();
      if (selFiles.size() == 1)
      {
         mUI.mDataFile->setText(selFiles[0]);
         mOutputDataFile = selFiles[0];
      }
   }
}

void PostProcessor::GeneralOutput::ProjectOpened(wizard::Project& aProject)
{
   mProjectPath = QString::fromStdString(aProject.ProjectDirectory().GetNormalizedPath());
   QDir projectDir(mProjectPath);

   mReportName = projectDir.dirName();
   mReportOutputPath = mProjectPath;
   mOutputPath = mProjectPath;
   mOutputDataFile = "";

   std::string output = mProjectPath.toStdString() + "/output";
   struct stat info;

   if (stat(output.c_str(), &info) == 0) // see if there is a directory called "output"
   {
      // see if there's a file named the report name + _data inside "output"
      if (info.st_mode & S_IFDIR)
      {
         mOutputPath = QString::fromStdString(output);
         mReportOutputPath = mOutputPath;
         std::string datafile = output + "/" + mReportName.toStdString() + "_data.txt";
         std::ifstream data(datafile);
         if (data.good())
         {
            mOutputDataFile = QString::fromStdString(datafile);
         }
      }
   }
   else // see if there's a file named the report name + _data inside the project directory
   {
      std::string datafile = mOutputPath.toStdString() + "/" + mReportName.toStdString() + "_data.txt";
      std::ifstream data(datafile);
      if (data.good())
      {
         mOutputDataFile = QString::fromStdString(datafile);
      }
   }


   mUI.mReportName->setText(mReportName);
   mUI.mDataFile->setText(mOutputDataFile);
   mUI.mOutputPath->setText(mReportOutputPath);

}

void PostProcessor::GeneralOutput::RestoreDefaults()
{
   // basically the same thing as ProjectOpened, search file tree
   QDir projectDir(mProjectPath);

   mReportName = projectDir.dirName();
   mReportOutputPath = mProjectPath;
   mOutputPath = mProjectPath;
   mOutputDataFile = "";

   std::string output = mProjectPath.toStdString() + "/output";
   struct stat info;

   if (stat(output.c_str(), &info) == 0)
   {
      if (info.st_mode & S_IFDIR)
      {
         mOutputPath = QString::fromStdString(output);
         mReportOutputPath = mOutputPath;
         std::string datafile = output + "/" + mReportName.toStdString() + "_data.txt";
         std::ifstream data(datafile);
         if (data.good())
         {
            mOutputDataFile = QString::fromStdString(datafile);
         }
      }
   }
   else
   {
      std::string datafile = mOutputPath.toStdString() + "/" + mReportName.toStdString() + "_data.txt";
      std::ifstream data(datafile);
      if (data.good())
      {
         mOutputDataFile = QString::fromStdString(datafile);
      }
   }

   mUI.mReportName->setText(mReportName);
   mUI.mDataFile->setText(mOutputDataFile);
   mUI.mOutputPath->setText(mReportOutputPath);

   mUI.mMonteCarloStart->setValue(1);
   mUI.mMonteCarloEnd->setValue(1);
   mUI.mCheckBoxSOF->setChecked(false);
}
