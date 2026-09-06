// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RELATIVEGEOMETRYPLOTUPDATER_HPP
#define RELATIVEGEOMETRYPLOTUPDATER_HPP

#include "RvPlotUpdater.hpp"

class UtEntity;

namespace rv
{
class MsgEntityState;
}
namespace wkf
{
class PlatformGrouping;
}

namespace RelativeGeometry
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX_Axis, const QString& aY_Axis, const QString& aFromPlatform, const QString& aToPlatform);
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
   bool                                   SupportsMultiplatform() const override { return false; }

private:
   double      GetData(const QString& aDatum, double aSimTime, const UtEntity& aFrom, const UtEntity& aTo) const;
   QStringList GetAxes() const;
   QString     GetUnits(const QString& aAxis) const;

   QStringList mInitialSelection;
   QString     mFromPlatformName;
   QString     mInitialX_Axis;
   QString     mInitialY_Axis;
};
} // namespace RelativeGeometry

#endif
