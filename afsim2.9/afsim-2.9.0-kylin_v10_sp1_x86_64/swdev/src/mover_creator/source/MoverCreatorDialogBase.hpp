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

#ifndef MOVER_CREATOR_DIALOG_BASE_HPP
#define MOVER_CREATOR_DIALOG_BASE_HPP

#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>

class QComboBox;
class QFileInfo;

namespace Designer
{
   class MoverCreatorDialogBase : public QDialog
   {
      public:
         explicit MoverCreatorDialogBase(QWidget*        aParent,
                                         Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~MoverCreatorDialogBase();

         void SetBaseDirPath(QString aPath) { mBaseDirPath = aPath; }
         QString GetBaseDirPath() { return mBaseDirPath; }
         virtual void ChangeTheme(QString aThemeName) = 0;

      protected:

         virtual void PopulateTypeComboBox(QString aDirPath, QComboBox* aComboBox);
         virtual void PopulateListComboBox(QString aDirPath, QComboBox* aComboBox, bool aShowTemplates = true);
         virtual void OkClicked(bool aClicked);
         virtual void CancelClicked(bool aClicked);
         QFileInfo GetTemplateFile(QString aDirPath);
         bool IsTemplateFile(const QString& aFilePath);

         // Reads from the settings file to determine which models to generate by default
         void LoadSettings(bool& aGenerateGM, bool& aGenerateP6DOF, bool& aGenerateRB6D, bool& aGeneratePM6D) const;

         // Write to the settings file to save which models the user generated
         void SaveSettings(bool aGenerateGM, bool aGenerateP6DOF, bool aGenerateRB6D, bool aGeneratePM6D) const;

      private:
         QString mBaseDirPath;
   };
}
#endif // !MOVER_CREATOR_DIALOG_BASE_HPP
