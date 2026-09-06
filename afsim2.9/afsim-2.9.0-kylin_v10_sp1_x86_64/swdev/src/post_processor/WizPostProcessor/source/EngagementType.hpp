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

#ifndef ENGAGEMENTTYPE_HPP
#define ENGAGEMENTTYPE_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

namespace Ui
{
class EngagementType;
}

namespace PostProcessor
{
// Engagement Report Type Widget
class EngagementType : public ReportWidget
{
      Q_OBJECT
   public:
      EngagementType();
      ~EngagementType() override;

      void GAHPressedHandler();
      void DFHPressedHandler();
      void ETPressedHandler();
      void PAPAPressedHandler();
      void TSPressedHandler();
      void TDPressedHandler();
      void ThSmPressedHandler();
      void TMSPressedHandler();
      void EFLPressedHandler();
      void MFPPressedHandler();
      void ISPressedHandler();
      void ActingPressedHandler();
      void ActionPressedHandler();
      void TErHPressedHandler();
      void TEvHPressedHandler();
      void TStatsPressedHandler();
      void FPWHPressedHandler();
      void WEPressedHandler();

      void emitReportOptionsPressed() { emit ReportOptionsPressed(mOptionName); }

      void WriteData(std::ostream& aStream) override;

      std::vector<OptionWidget*> GetOptionWidgets() { return mOptionWidgets; }
   signals:
      void ReportOptionsPressed(std::string aOptionName);

   private:
      void RestoreDefaults() override;

      Ui::EngagementType*        mUIPtr;
      std::string                mOptionName;
      std::vector<OptionWidget*> mOptionWidgets;
};
}
#endif // !ENGAGEMENTTYPE_HPP
