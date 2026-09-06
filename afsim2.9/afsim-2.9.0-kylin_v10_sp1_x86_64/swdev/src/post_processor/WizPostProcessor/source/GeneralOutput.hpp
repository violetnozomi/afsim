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

#ifndef GENERALOUTPUT_HPP
#define GENERALOUTPUT_HPP

// Qt
#include <QWidget>

#include "Project.hpp"
#include "ReportWidget.hpp"

#include "ui_GeneralOutput.h"

namespace PostProcessor
{
// Output Options Widget
class GeneralOutput : public ReportWidget
{
      Q_OBJECT
   public:
      GeneralOutput();

      QString GetConfigName() const;
      QString GetDataFileName() const { return mOutputDataFile; }
      int GetMonteCarloStart() const;
      int GetMonteCarloEnd() const;
      void WriteData(std::ostream& aStream) override;

   private:
      void ReportOutputBrowsePressed();
      void DataFileBrowsePressed();
      void ProjectOpened(wizard::Project& aProjectPtr);
      void RestoreDefaults() override;

      Ui::GeneralOutput mUI;
      QString mProjectPath;
      QString mReportOutputPath;
      QString mOutputPath;

      QString mOutputDataFile;
      QString mReportName;
      int     mMonteCarloStart;
      int     mMonteCarloEnd;
};
}
#endif
