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

#ifndef DETECTEDPLATFORMS_HPP
#define DETECTEDPLATFORMS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

#include "ui_DetectedPlatforms.h"

namespace PostProcessor
{
// Detected Platforms Widget for Detection Report
class DetectedPlatforms : public ReportWidget
{
      Q_OBJECT
   public:

      DetectedPlatforms();
      ~DetectedPlatforms() override = default;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddPlatforms(const QMap<ProxyInterface::Platform,
                        bool>& aPlatforms);
      void RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms);

   private:
      Ui::DetectedPlatforms mUI;
};
}
#endif // !DetectedPlatforms_HPP
