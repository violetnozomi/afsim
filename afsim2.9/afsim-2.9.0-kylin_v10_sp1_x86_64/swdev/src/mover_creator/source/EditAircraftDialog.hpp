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

#ifndef EDIT_AIRCRAFT_DIALOG_HPP
#define EDIT_AIRCRAFT_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class EditAircraftDialog;
}

namespace Designer
{
   class EditAircraftDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit EditAircraftDialog(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~EditAircraftDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;


      private slots:
         void VehicleTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;
         void MoverCheckBoxChanged(bool aChecked);

      private:
         Ui::EditAircraftDialog* mUIPtr;
   };
}
#endif // !EDIT_AIRCRAFT_DIALOG_HPP
