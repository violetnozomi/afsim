// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef LIQUID_ROCKET_ENGINE_WIDGET_HPP
#define LIQUID_ROCKET_ENGINE_WIDGET_HPP

#include "EngineWidget.hpp"

namespace Ui
{
   class LiquidRocketEngineWidget;
}

namespace Designer
{
   class LiquidRocketEngineWidget : public EngineWidget
   {
         Q_OBJECT
      public:
         LiquidRocketEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions);
         ~LiquidRocketEngineWidget() override;

         QStringList Audit() const override;

      private:
         void HandleEngineSpecEditingFinished() override;

         void PopulateLineEdits() override;

         Ui::LiquidRocketEngineWidget* mUIPtr;
   };
}

#endif // !LIQUID_ROCKET_ENGINE_WIDGET_HPP
