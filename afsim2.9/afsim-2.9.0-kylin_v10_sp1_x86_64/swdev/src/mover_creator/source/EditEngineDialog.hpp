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

#ifndef EDIT_ENGINE_DIALOG_HPP
#define EDIT_ENGINE_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

#include <memory>

namespace Designer
{
   class EngineDesignerWidget;
}

namespace Ui
{
   class EditEngineDialog;
   class EngineDesignerWidget;
}

namespace Designer
{
   class EditEngineDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit EditEngineDialog(QWidget*        aParent,
                                   Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~EditEngineDialog();

         //void SetEngineDesignerWidget(std::shared_ptr<EngineDesignerWidget> aEngineDesignerWidget);

         void showEvent(QShowEvent* aEvent) override;

         void SetEngineTypeAndModel(QString aType, QString aModel);
         void ChangeTheme(QString aThemeName) override;

      private slots:
         void EngineTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;

      private:
         Ui::EditEngineDialog* mUIPtr;
   };
}
#endif // !EDIT_ENGINE_DIALOG_HPP
