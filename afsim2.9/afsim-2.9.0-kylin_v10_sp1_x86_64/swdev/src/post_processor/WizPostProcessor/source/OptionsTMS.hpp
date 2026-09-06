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

#ifndef OPTIONSTMS_HPP
#define OPTIONSTMS_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsTMS;
}

namespace PostProcessor
{
// Threat Mode Summary Option Widget for Engagement Report
class OptionsTMS : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsTMS();
      ~OptionsTMS() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsTMS* mUIPtr;
};
}
#endif // !OPTIONSTMS_HPP
