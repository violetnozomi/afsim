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

#ifndef CREATE_WEAPON_DIALOG_HPP
#define CREATE_WEAPON_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class CreateWeaponDialog;
}

namespace Designer
{
   class CreateWeaponDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit CreateWeaponDialog(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~CreateWeaponDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void VehicleTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;
         void MoverCheckBoxChanged(bool aChecked);

      private:
         Ui::CreateWeaponDialog* mUIPtr;
   };
}
#endif // !CREATE_WEAPON_DIALOG_HPP
