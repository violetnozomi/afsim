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

#ifndef OPTIONSGAH_HPP
#define OPTIONSGAH_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsGAH;
}

namespace PostProcessor
{
// Geometric Action History Option Widget for Engagement Report
class OptionsGAH : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsGAH();
      ~OptionsGAH() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void EnableGRDAWidgets(bool aEnable);
      void RestoreDefaults() override;
      Ui::OptionsGAH* mUIPtr;
};
}
#endif // !OPTIONSGAH_HPP
