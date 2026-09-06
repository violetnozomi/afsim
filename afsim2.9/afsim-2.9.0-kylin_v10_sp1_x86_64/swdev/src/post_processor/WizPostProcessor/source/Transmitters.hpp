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

#ifndef TRANSMITTERS_HPP
#define TRANSMITTERS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace Ui
{
class Transmitters;
}

namespace PostProcessor
{
// Transmitters Widget for Communication Report
class Transmitters : public ReportWidget
{
      Q_OBJECT
   public:
      Transmitters();
      ~Transmitters() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddTransmitters(const QVector<ProxyInterface::PlatformPart>& aTransmitters);
      void RemoveTransmitters(const QVector<ProxyInterface::PlatformPart>& aTransmitters);

   private:
      void RestoreDefaults() override;
      Ui::Transmitters* mUIPtr;
};
}
#endif // !TRANSMITTERS_HPP