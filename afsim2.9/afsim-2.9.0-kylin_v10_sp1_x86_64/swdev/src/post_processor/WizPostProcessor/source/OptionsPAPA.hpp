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

#ifndef OPTIONSPAPA_HPP
#define OPTIONSPAPA_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsPAPA;
}

namespace PostProcessor
{
// PAPA Option Widget for Engagement Report
class OptionsPAPA : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsPAPA();
      ~OptionsPAPA() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsPAPA* mUIPtr;
};
}
#endif // !OPTIONSPAPA_HPP
