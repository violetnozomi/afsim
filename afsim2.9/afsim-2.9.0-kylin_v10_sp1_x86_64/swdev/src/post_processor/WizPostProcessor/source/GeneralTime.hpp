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

#ifndef GENERALTIME_HPP
#define GENERALTIME_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"
#include "WsfPProxyBasicValues.hpp"

#include "ui_GeneralTime.h"

class WsfPProxy;

namespace PostProcessor
{
// Time Options Widget
class GeneralTime : public ReportWidget
{
      Q_OBJECT
   public:
      GeneralTime();
      ~GeneralTime() override = default;

      void WriteData(std::ostream& aStream) override;
      void UpdateEndTime(const WsfProxy::Time& aEndTime);

   private:
      void RestoreDefaults() override;
      Ui::GeneralTime mUI;
      WsfProxy::Time mEndTime;
      WsfProxy::Time mSimTime;
};
}
#endif // !GENERALTIME_HPP
