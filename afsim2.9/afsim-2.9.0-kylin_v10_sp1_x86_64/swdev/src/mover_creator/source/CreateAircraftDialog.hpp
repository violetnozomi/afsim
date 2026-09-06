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

#ifndef CREATE_AIRCRAFT_DIALOG_HPP
#define CREATE_AIRCRAFT_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class CreateAircraftDialog;
}

namespace Designer
{
   class CreateAircraftDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit CreateAircraftDialog(QWidget*        aParent,
                                       Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~CreateAircraftDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void VehicleTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;
         void MoverCheckBoxChanged(bool aChecked);

      private:
         Ui::CreateAircraftDialog* mUIPtr;
   };
}
#endif // !CREATE_AIRCRAFT_DIALOG_HPP
