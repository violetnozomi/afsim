// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RV_AUX_DATA_PLOT_UPDATER_HPP
#define RV_AUX_DATA_PLOT_UPDATER_HPP

#include <vector>

#include <QStringList>

#include "RvPlotUpdater.hpp"
#include "UtOptional.hpp"

namespace rv
{
class MsgAuxData;
class ResultPlatform;
} // namespace rv
namespace wkf
{
class PlatformGrouping;
}

namespace RvAuxData
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup);
   ~PlotUpdater() override = default;
   QStringList                            GetPlatforms() const override;
   QStringList                            GetAxesX() const override { return GetAxes(); }
   QStringList                            GetAxesY() const override { return GetAxes(); }
   const QStringList&                     GetInitialSelection() const override;
   const QString&                         GetInitialXAxis() const override;
   const QString&                         GetInitialYAxis() const override;
   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

private:
   QStringList          GetAxes() const;
   void                 DiscoverAuxDataKeys(rv::ResultPlatform& aPlatform, QStringList& aKeyList) const;
   ut::optional<double> GetData(const QString& aDesc, const rv::MsgAuxData* aState) const;

   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
};
} // namespace RvAuxData

#endif
