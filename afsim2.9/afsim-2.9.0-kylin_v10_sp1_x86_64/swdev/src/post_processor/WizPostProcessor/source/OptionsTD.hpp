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

#ifndef OPTIONSTD_HPP
#define OPTIONSTD_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsTD;
}

namespace PostProcessor
{
// Threat Summary Option Widget for Engagement Report
class OptionsTD : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsTD();
      ~OptionsTD() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsTD* mUIPtr;
};
}
#endif // !OPTIONSTD_HPP