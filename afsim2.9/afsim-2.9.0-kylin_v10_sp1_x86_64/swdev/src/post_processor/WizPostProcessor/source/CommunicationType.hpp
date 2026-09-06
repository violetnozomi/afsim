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

#ifndef COMMTYPE_HPP
#define COMMTYPE_HPP

// Qt
#include <QWidget>

#include "OptionWidget.hpp"

#include "ui_CommunicationType.h"

#include "UtSpan.hpp"

namespace PostProcessor
{
// Communication Report Type Widget
class CommunicationType : public ReportWidget
{
      Q_OBJECT
   public:
      CommunicationType();
      ~CommunicationType() override = default;

      void TCPressedHandler();
      void CPressedHandler();
      void emitReportOptionsPressed() { emit ReportOptionsPressed(mOptionName); }

      void WriteData(std::ostream& aStream) override;

      ut::span<OptionWidget*, 2> GetOptionWidgets() { return mOptionWidgets; }

   signals:
      void ReportOptionsPressed(std::string aOptionName);

   private:
      void RestoreDefaults() override;
      Ui::CommunicationType mUI;
      std::string mOptionName;
      std::array<OptionWidget*, 2> mOptionWidgets;
};
}
#endif // !COMMTYPE_HPP
