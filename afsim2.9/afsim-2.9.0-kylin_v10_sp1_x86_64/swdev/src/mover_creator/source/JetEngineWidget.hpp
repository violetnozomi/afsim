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

#ifndef JET_ENGINE_WIDGET_HPP
#define JET_ENGINE_WIDGET_HPP

#include "EngineWidget.hpp"

namespace Ui
{
   class JetEngineWidget;
}

namespace Designer
{
   class JetEngineWidget : public EngineWidget
   {
         Q_OBJECT
      public:
         JetEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions, bool aIncludeAfterburner);
         ~JetEngineWidget() override;

         QStringList Audit() const override;

      private:
         void HandleEngineSpecEditingFinished() override;

         void PopulateLineEdits() override;

         Ui::JetEngineWidget* mUIPtr;
         bool mIncludeAfterburner;
   };
}
#endif // !JET_ENGINE_WIDGET_HPP
