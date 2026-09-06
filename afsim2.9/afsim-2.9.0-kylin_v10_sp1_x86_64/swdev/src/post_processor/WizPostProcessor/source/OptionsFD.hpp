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

#ifndef OPTIONSFD_HPP
#define OPTIONSFD_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class OptionsFD;
}

namespace PostProcessor
{
// First Detections Option Widget for Detection Report
class OptionsFD : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsFD();
      ~OptionsFD() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OptionsFD* mUIPtr;
};
}
#endif // !OPTIONSFD_HPP