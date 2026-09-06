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

#ifndef DETECTIONTYPE_HPP
#define DETECTIONTYPE_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class DetectionType;
}

namespace PostProcessor
{

// Detection Report Type Widget
class DetectionType : public ReportWidget
{
      Q_OBJECT
   public:
      DetectionType();
      ~DetectionType() override;

      void FDPressedHandler();
      void ADEPressedHandler();
      void TDPressedHandler();
      void DBPressedHandler();

      void AccessPressedHandler();

      void emitReportOptionsPressed() { emit ReportOptionsPressed(mOptionName); }

      void WriteData(std::ostream& aStream) override;

      std::vector<OptionWidget*> GetOptionWidgets() { return mOptionWidgets; }

   signals:
      void ReportOptionsPressed(std::string aOptionName);

   private:
      void RestoreDefaults() override;
      Ui::DetectionType* mUIPtr;
      std::string mOptionName;
      std::vector<OptionWidget*> mOptionWidgets;
};
}
#endif // !DETECTIONTYPE_HPP
