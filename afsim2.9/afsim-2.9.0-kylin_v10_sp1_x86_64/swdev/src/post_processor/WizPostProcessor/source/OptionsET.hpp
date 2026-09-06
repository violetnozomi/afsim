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

#ifndef OPTIONSET_HPP
#define OPTIONSET_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsET;
}

namespace PostProcessor
{
// Event Timing Option Widget for Engagement Report
class OptionsET : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsET();
      ~OptionsET() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsET* mUIPtr;
};
}
#endif // !OPTIONSET_HPP
