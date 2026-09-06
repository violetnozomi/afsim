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

#ifndef OPTIONSTC_HPP
#define OPTIONSTC_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsTC;
}

namespace PostProcessor
{
// Traffic Count Option Widget for Communication Report
class OptionsTC : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsTC();
      ~OptionsTC() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsTC* mUIPtr;
};
}
#endif // !OPTIONSTC_HPP
