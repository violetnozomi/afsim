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

#ifndef EDIT_WEAPON_DIALOG_HPP
#define EDIT_WEAPON_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class EditWeaponDialog;
}

namespace Designer
{
   class EditWeaponDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit EditWeaponDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~EditWeaponDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void VehicleTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;
         void MoverCheckBoxChanged(bool aChecked);

      private:
         Ui::EditWeaponDialog* mUIPtr;
   };
}
#endif // !EDIT_WEAPON_DIALOG_HPP
