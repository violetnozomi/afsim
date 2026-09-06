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

#ifndef DETECTINGSENSORS_HPP
#define DETECTINGSENSORS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace Ui
{
class DetectingSensors;
}

namespace PostProcessor
{
class DetectingSensors : public ReportWidget
{
      Q_OBJECT
   public:
      DetectingSensors();
      ~DetectingSensors() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddSensors(const QVector<ProxyInterface::PlatformPart>& aSensors);
      void RemoveSensors(const QVector<ProxyInterface::PlatformPart>& aSensors);

   private:
      Ui::DetectingSensors* mUIPtr;
};

}

#endif