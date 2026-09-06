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

#ifndef OPTIONSC_HPP
#define OPTIONSC_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsC;
}

namespace PostProcessor
{
// Connectivity Option Widget for Communication Report
class OptionsC : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsC();
      ~OptionsC() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsC* mUIPtr;

};
}
#endif // !OPTIONSC_HPP
