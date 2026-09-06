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

#ifndef OPTIONSTS_HPP
#define OPTIONSTS_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsTS;
}

namespace PostProcessor
{
// Track States Option Widget for Engagement Report
class OptionsTS : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsTS();
      ~OptionsTS() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsTS* mUIPtr;
};
}
#endif // !OPTIONSTS_HPP

