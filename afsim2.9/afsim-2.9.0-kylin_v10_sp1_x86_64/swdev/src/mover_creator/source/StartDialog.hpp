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

#ifndef START_DIALOG_HPP
#define START_DIALOG_HPP

#include <memory>
#include <QWidget>

namespace Designer
{
   class CreateAircraftDialog;
   class EditAircraftDialog;
   class DeleteAircraftDialog;
   class ConvertBrawlerAircraftDialog;
   class CreateWeaponDialog;
   class EditWeaponDialog;
   class DeleteWeaponDialog;
   class CreateEngineDialog;
   class EditEngineDialog;
   class DeleteEngineDialog;
   class EngineDesignerWidget;
}

namespace Ui
{
   class StartDialog;
}
namespace Designer
{
   class StartDialog : public QWidget
   {
      public:

         explicit StartDialog(QWidget*        aParent,
                              Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~StartDialog();

         void ChangeTheme(QString aThemeName);

         void Reset() { }

      private slots:

         void CreateAircraftClicked(bool aChecked);
         void EditAircraftClicked(bool aChecked);
         void DeleteAircraftClicked(bool aChecked);
         void ConvertBrawlerAircraftClicked(bool aChecked);
         void CreateWeaponClicked(bool aChecked);
         void EditWeaponClicked(bool aChecked);
         void DeleteWeaponClicked(bool aChecked);
         void CreateEngineClicked(bool aChecked);
         void EditEngineClicked(bool aChecked);
         void DeleteEngineClicked(bool aChecked);
         void CancelClicked(bool aChecked);
         void SettingsClicked();
         void AboutClicked();

      private:

         std::unique_ptr<Ui::StartDialog>      mUIPtr;

         std::unique_ptr<CreateAircraftDialog> mCreateAircraftDialog;
         std::unique_ptr<EditAircraftDialog>   mEditAircraftDialog;
         std::unique_ptr<DeleteAircraftDialog> mDeleteAircraftDialog;

         std::unique_ptr<ConvertBrawlerAircraftDialog> mConvertBrawlerAircraftDialog;

         std::unique_ptr<CreateWeaponDialog>   mCreateWeaponDialog;
         std::unique_ptr<EditWeaponDialog>     mEditWeaponDialog;
         std::unique_ptr<DeleteWeaponDialog>   mDeleteWeaponDialog;

         std::unique_ptr<CreateEngineDialog>   mCreateEngineDialog;
         std::unique_ptr<EditEngineDialog>     mEditEngineDialog;
         std::unique_ptr<DeleteEngineDialog>   mDeleteEngineDialog;
   };
}
#endif // !START_DIALOG_HPP
