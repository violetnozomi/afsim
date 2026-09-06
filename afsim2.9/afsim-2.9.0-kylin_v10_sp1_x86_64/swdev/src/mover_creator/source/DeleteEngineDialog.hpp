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

#ifndef DELETE_ENGINE_DIALOG_HPP
#define DELETE_ENGINE_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

namespace Ui
{
   class DeleteEngineDialog;
}

namespace Designer
{
   class DeleteEngineDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit DeleteEngineDialog(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~DeleteEngineDialog();

         void showEvent(QShowEvent* aEvent) override;
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void EngineTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;

      private:
         Ui::DeleteEngineDialog* mUIPtr;
   };
}
#endif // !DELETE_ENGINE_DIALOG_HPP
