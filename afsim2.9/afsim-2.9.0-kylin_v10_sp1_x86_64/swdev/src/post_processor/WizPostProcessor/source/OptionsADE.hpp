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

#ifndef OPTIONSADE_HPP
#define OPTIONSADE_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsADE;
}

namespace PostProcessor
{
// All Detection Events Option Widget for Detection Report
class OptionsADE : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsADE();
      ~OptionsADE() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsADE* mUIPtr;
};
}
#endif // !OPTIONSADE_HPP