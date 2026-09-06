// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVSCRIPTDATAPLOTUPDATER_HPP
#define RVSCRIPTDATAPLOTUPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"

namespace wkf
{
class PlatformGrouping;
}

namespace RvScriptDataFeed
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup);
   ~PlotUpdater() override = default;
   QStringList                            GetPlatforms() const override;
   QStringList                            GetAxesX() const override;
   QStringList                            GetAxesY() const override;
   QString                                GetUnitsX(const QString& aAxis) const override;
   QString                                GetUnitsY(const QString& aAxis) const override;
   const QStringList&                     GetInitialSelection() const override;
   const QString&                         GetInitialXAxis() const override;
   const QString&                         GetInitialYAxis() const override;
   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

private:
   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
};
} // namespace RvScriptDataFeed

#endif
