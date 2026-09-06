// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MoverCreatorDialogBase.hpp"

#include "MoverCreatorMainWindow.hpp"
#include <QComboBox>
#include <QDir>

namespace Designer
{

MoverCreatorDialogBase::MoverCreatorDialogBase(QWidget*        aParent,
                                               Qt::WindowFlags aFlags)
   : QDialog(aParent, aFlags)
{

   SetBaseDirPath(MoverCreatorMainWindow::GetDataPath());
   setModal(true);
}

MoverCreatorDialogBase::~MoverCreatorDialogBase()
{

}

void MoverCreatorDialogBase::PopulateTypeComboBox(QString aDirPath, QComboBox* aComboBox)
{
   aComboBox->clear();
   QDir typeDir(aDirPath);
   if (typeDir.exists())
   {
      for (auto& curType : typeDir.entryInfoList())
      {
         if (curType.isDir())
         {
            QString typeName = curType.baseName();
            if (typeName != "")
            {
               aComboBox->addItem(typeName);
            }
         }
      }
   }
}

void MoverCreatorDialogBase::PopulateListComboBox(QString aDirPath, QComboBox* aComboBox, bool aShowTemplates)
{
   aComboBox->clear();
   //QFileInfo standardTemplateFile = GetTemplateFile(aDirPath);

   QDir listDir(aDirPath);
   if (listDir.exists())
   {
      for (auto& curFile : listDir.entryInfoList())
      {
         if (curFile.isFile())
         {
            QString fileName = curFile.fileName();
            if (fileName.endsWith(".amc"))
            {
               if (IsTemplateFile(aDirPath + fileName) && !aShowTemplates)
               {
                  continue;
               }
               else if (fileName.startsWith("~")) //do not show temp files
               {
                  continue;
               }
               else
               {
                  fileName.chop(4);
                  aComboBox->addItem(fileName);
               }
            }
         }
      }
   }
}

void MoverCreatorDialogBase::OkClicked(bool aClicked)
{
   close();
}

void MoverCreatorDialogBase::CancelClicked(bool aClicked)
{
   close();
}

//Scan the directory for an instance of a template file.  NOTE will return the first template it finds, there could be more than one.
QFileInfo MoverCreatorDialogBase::GetTemplateFile(QString aDirPath)
{
   QDir typeDir(aDirPath);
   if (typeDir.exists())
   {
      for (auto& curFile : typeDir.entryInfoList())
      {
         if (curFile.isFile())
         {
            QFile jFile(curFile.absoluteFilePath());
            if (jFile.open(QIODevice::ReadOnly))
            {
               QByteArray fileData = jFile.readAll();
               QJsonDocument curDoc(QJsonDocument::fromJson(fileData));
               QJsonObject docObj = curDoc.object();

               if (docObj["standardTemplate"].toBool())
               {
                  return curFile;
               }
            }
         }
      }
   }
   return QFileInfo();
}

bool MoverCreatorDialogBase::IsTemplateFile(const QString& aFilePath)
{
   //QFile jFile(aFile.absoluteFilePath());
   QFile jFile(aFilePath);
   if (jFile.open(QIODevice::ReadOnly))
   {
      QByteArray fileData = jFile.readAll();
      QJsonDocument curDoc(QJsonDocument::fromJson(fileData));
      QJsonObject docObj = curDoc.object();

      // Some .amc files have null values for inheritedFileName, but false values for
      // standardTemplate.
      if (docObj["inheritedFileName"].toString() == "" || docObj["standardTemplate"].toBool())
      {
         return true;
      }
   }
   return false;
}

void Designer::MoverCreatorDialogBase::LoadSettings(bool& aGenerateGM, bool& aGenerateP6DOF, bool& aGenerateRB6D, bool& aGeneratePM6D) const
{
   // Start with assuming there are no setting entries 
   // for GenerateP6DOFMover or GenerateGuidedMover.
   // When the user runs for the first time, 
   // P6DOF will be checked and GM will be unchecked.

   aGenerateGM    = false;
   aGeneratePM6D  = false;
   aGenerateP6DOF = false;
   aGenerateRB6D  = true;

   MoverCreatorMainWindow& mainWindow = MoverCreatorMainWindow::GetInstance();
   QSettings* settings = mainWindow.GetSettings();
   if (settings)
   {
      if (settings->contains("GenerateGuidedMover"))
      {
         aGenerateGM = settings->value("GenerateGuidedMover").toBool();
      }
      if (settings->contains("GenerateP6DOFMover"))
      {
         aGenerateP6DOF = settings->value("GenerateP6DOFMover").toBool();
      }
      if (settings->contains("GenerateRB6DOFMover"))
      {
         aGenerateRB6D = settings->value("GenerateRB6DOFMover").toBool();
      }
      if (settings->contains("GeneratePM6DOFMover"))
      {
          aGeneratePM6D = settings->value("GeneratePM6DOFMover").toBool();
      }
   }
}

void Designer::MoverCreatorDialogBase::SaveSettings(bool aGenerateGM, bool aGenerateP6DOF, bool aGenerateRB6D, bool aGeneratePM6D) const
{
   QSettings* settings = MoverCreatorMainWindow::GetInstance().GetSettings();
   settings->setValue("GenerateGuidedMover", aGenerateGM);
   settings->setValue("GenerateP6DOFMover",  aGenerateP6DOF);
   settings->setValue("GenerateRB6DOFMover", aGenerateRB6D);
   settings->setValue("GeneratePM6DOFMover", aGeneratePM6D);
}

} // namespace Designer
