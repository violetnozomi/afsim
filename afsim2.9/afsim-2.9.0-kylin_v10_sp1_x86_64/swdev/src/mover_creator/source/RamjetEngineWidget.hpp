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

#ifndef RAMJET_ENGINE_WIDGET_HPP
#define RAMJET_ENGINE_WIDGET_HPP

#include "EngineWidget.hpp"

namespace Ui
{
   class RamjetEngineWidget;
}

namespace Designer
{
   class RamjetEngineWidget : public EngineWidget
   {
         Q_OBJECT
      public:
         RamjetEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions);
         ~RamjetEngineWidget() override;

         QStringList Audit() const override;

      private:
         void HandleEngineSpecEditingFinished() override;

         void PopulateLineEdits() override;

         Ui::RamjetEngineWidget* mUIPtr;
   };
}
#endif // !JET_ENGINE_WIDGET_HPP
