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

#ifndef OUTPUTDATAOPTIONS_HPP
#define OUTPUTDATAOPTIONS_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"

namespace Ui
{
class OutputDataOptions;
}

namespace PostProcessor
{
// Output Data Options for Trajectory Report
class OutputDataOptions : public ReportWidget
{
      Q_OBJECT
   public:
      OutputDataOptions();
      ~OutputDataOptions() override;

      void WriteData(std::ostream& aStream) override;

   private:
      void RestoreDefaults() override;
      Ui::OutputDataOptions* mUIPtr;
};
}
#endif // !OUTPUTDATAOPTIONS_HPP