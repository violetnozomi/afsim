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

#ifndef RECEIVERS_HPP
#define RECEIVERS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace Ui
{
class Receivers;
}

namespace PostProcessor
{

// Receivers Widget for Communication Report
class Receivers : public ReportWidget
{
      Q_OBJECT
   public:
      Receivers();
      ~Receivers() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddReceivers(const QVector<ProxyInterface::PlatformPart>& aReceivers);
      void RemoveReceivers(const QVector<ProxyInterface::PlatformPart>& aReceivers);


   private:
      void RestoreDefaults() override;
      Ui::Receivers* mUIPtr;
};

}
#endif // !RECEIVERS_HPP
