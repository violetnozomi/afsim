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

#ifndef ENGINE_DESIGNER_WIDGET_HPP
#define ENGINE_DESIGNER_WIDGET_HPP

#include <QWidget>
#include <QMenu>

#include "AeroEngine.hpp"
#include "EngineWidget.hpp"

namespace Ui
{
   class EngineDesignerWidget;
}

namespace Designer
{
   class EngineDesignerWidget : public QWidget
   {
         Q_OBJECT

      public:
         explicit EngineDesignerWidget(QWidget*        aParent,
                                       Qt::WindowFlags aFlags = Qt::WindowFlags());
         ~EngineDesignerWidget() override;

         void showEvent(QShowEvent* aEvent);
         void SetTypeAndModel(QString aType, QString aModel);
         bool CheckNeedsSave();
         void SaveModel();

         QStringList Audit();

         void Reset() { }

      private:
         void UpdatePlot();
         void HandleEngineSpecsChanged();
         void HandleGlobalOffsetClicked();
         void HandleGlobalMultiplierClicked();
         void ToggleModifierLine();
         void ToggleModifierType();
         void LoadModifierInfo();
         void RevertPlot();
         void LoadEngine();

         Ui::EngineDesignerWidget*     mUIPtr;
         std::unique_ptr<EngineWidget> mEngineWidgetPtr;
         std::unique_ptr<AeroEngine>   mAeroEnginePtr;

         std::shared_ptr<QDoubleValidator> mPositiveValidator;
         std::shared_ptr<QDoubleValidator> mRealNumberValidator;

         bool                          mEngineSaved;
   };
}
#endif // !ENGINE_DESIGNER_WIDGET_HPP
