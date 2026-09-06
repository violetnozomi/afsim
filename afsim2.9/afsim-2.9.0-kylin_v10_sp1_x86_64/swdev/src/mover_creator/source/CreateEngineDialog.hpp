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

#ifndef CREATE_ENGINE_DIALOG_HPP
#define CREATE_ENGINE_DIALOG_HPP

#include "MoverCreatorDialogBase.hpp"

#include <memory>

namespace Designer
{
   class EngineDesignerWidget;
}

namespace Ui
{
   class CreateEngineDialog;
}

namespace Designer
{
   class CreateEngineDialog : public MoverCreatorDialogBase
   {
         Q_OBJECT
      public:
         explicit CreateEngineDialog(QWidget*        aParent,
                                     Qt::WindowFlags aFlags = Qt::WindowFlags());

         ~CreateEngineDialog();

         void SetEngineDesignerWidget(const std::shared_ptr<EngineDesignerWidget>& aEngineDesignerWidget);
         void SetEngineTypeAndModel(QString aType, QString aModel);
         void ChangeTheme(QString aThemeName) override;

         void showEvent(QShowEvent* aEventPtr) override;

      signals:
         void EngineCreated(QString aModelName);

      private slots:
         void EngineTypeChanged(int aIndex);
         void OkClicked(bool aClicked) override;

      private:
         Ui::CreateEngineDialog* mUIPtr;

         std::shared_ptr<EngineDesignerWidget> mSharedEngineDesignerWidget = 0;
   };
}
#endif // !CREATE_ENGINE_DIALOG_HPP
