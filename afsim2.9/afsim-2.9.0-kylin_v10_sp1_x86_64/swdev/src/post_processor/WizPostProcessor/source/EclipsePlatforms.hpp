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

#ifndef ECLIPSEPLATFORMS_HPP
#define ECLIPSEPLATFORMS_HPP

// Qt
#include <QWidget>

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace Ui
{
class EclipsePlatforms;
}
namespace PostProcessor
{
// Eclipse Platforms Widget for Eclipse Report
class EclipsePlatforms : public ReportWidget
{
      Q_OBJECT
   public:

      EclipsePlatforms();
      ~EclipsePlatforms() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddPlatforms(const QMap<ProxyInterface::Platform, bool>& aPlatforms);
      void RemovePlatforms(const QVector<ProxyInterface::Platform>& aPlatforms);

   private:
      Ui::EclipsePlatforms* mUIPtr;
};
}
#endif // !ECLIPSEPLATFORMS_HPP
