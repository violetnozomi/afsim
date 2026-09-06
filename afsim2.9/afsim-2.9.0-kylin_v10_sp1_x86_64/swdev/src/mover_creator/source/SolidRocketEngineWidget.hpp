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

#ifndef SOLID_ROCKET_ENGINE_WIDGET_HPP
#define SOLID_ROCKET_ENGINE_WIDGET_HPP

#include "EngineWidget.hpp"

namespace Ui
{
   class SolidRocketEngineWidget;
}

namespace Designer
{
   class SolidRocketEngineWidget : public EngineWidget
   {
         Q_OBJECT
      public:
         SolidRocketEngineWidget(const QString& aName, size_t aNumBaseSpecs, size_t aNumBaseSpecsOptions);
         ~SolidRocketEngineWidget() override;

         QStringList Audit() const override;

         void PopulateLineEdits() override;

      private:
         void HandleEngineSpecEditingFinished() override;

         void HandleSpecificationsComboBoxChanged(const QString& aSpecifications);
         void HandleMassComboBoxChanged(const QString& aMassSpecification);
         void HandleAltitudeComboBoxChanged(const QString& aAltSpecification);
         void HandlePropellantMassComboBoxChanged(const QString& aPropellantMassSpecification);

         Ui::SolidRocketEngineWidget* mUIPtr;
   };
}
#endif // !SOLID_ROCKET_ENGINE_WIDGET_HPP
