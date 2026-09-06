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

#ifndef OPTIONSDFH_HPP
#define OPTIONSDFH_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsDFH;
}

namespace PostProcessor
{
// Damage Factor History Option Widget for Engagement Report
class OptionsDFH : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsDFH();
      ~OptionsDFH() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void EnableDEWidgets(bool aEnable);
      void RestoreDefaults() override;
      Ui::OptionsDFH* mUIPtr;
};
}
#endif // !OPTIONSDFH_HPP

