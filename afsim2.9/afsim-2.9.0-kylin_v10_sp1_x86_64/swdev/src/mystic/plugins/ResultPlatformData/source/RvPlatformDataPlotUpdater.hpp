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
#ifndef RVPLATFORMDATAPLOTUPDATER_HPP
#define RVPLATFORMDATAPLOTUPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"

namespace rv
{
class MsgEntityState;
}
namespace wkf
{
class PlatformGrouping;
}

namespace RvPlatformData
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup);
   ~PlotUpdater() override = default;
   QStringList                            GetPlatforms() const override;
   QStringList                            GetAxesX() const override { return GetAxes(); }
   QStringList                            GetAxesY() const override { return GetAxes(); }
   QString                                GetUnitsX(const QString& aAxis) const override { return GetUnits(aAxis); }
   QString                                GetUnitsY(const QString& aAxis) const override { return GetUnits(aAxis); }
   const QStringList&                     GetInitialSelection() const override;
   const QString&                         GetInitialXAxis() const override;
   const QString&                         GetInitialYAxis() const override;
   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

private:
   QStringList GetAxes() const;
   QString     GetUnits(const QString& aAxis) const;
   double GetData(const QString& aDesc, const rv::MsgEntityState* aState, const rv::MsgEntityState* aStartMessage) const;

   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
};
} // namespace RvPlatformData

#endif
