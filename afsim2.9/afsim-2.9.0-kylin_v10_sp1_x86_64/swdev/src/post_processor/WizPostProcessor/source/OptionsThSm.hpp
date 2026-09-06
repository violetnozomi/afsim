// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef OPTIONSTHSM_HPP
#define OPTIONSTHSM_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsThSm;
}

namespace PostProcessor
{
// Threat Summary Option Widget for Engagement Report
class OptionsThSm : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsThSm();
      ~OptionsThSm() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsThSm* mUIPtr;
};
}
#endif // !OPTIONSTHSM_HPP


