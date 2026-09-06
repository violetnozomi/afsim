//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef RVORBITALDATAPLOTUPDATERS_HPP
#define RVORBITALDATAPLOTUPDATERS_HPP

#include "RvPlotUpdater.hpp"

namespace rv
{
class MsgEntityState;
}
namespace wkf
{
class PlatformGrouping;
}

namespace RvOrbitalData
{
class PlotUpdater : public rv::PlotUpdater
{
public:
   PlotUpdater(const QString& aX, const QString& aY, const wkf::PlatformGrouping& aGroup);
   ~PlotUpdater() override = default;

   /** Get the list of platforms. */
   QStringList GetPlatforms() const override;

   /** Get the axes for X. */
   QStringList GetAxesX() const override { return GetAxes(); }

   /** Get the axes for Y. */
   QStringList GetAxesY() const override { return GetAxes(); }

   /** Get the units associated with the given axis. */
   QString GetUnitsX(const QString& aAxis) const override { return GetUnits(aAxis); }
   QString GetUnitsY(const QString& aAxis) const override { return GetUnits(aAxis); }

   /** Get the initial selection for the plot. */
   const QStringList& GetInitialSelection() const override { return mInitialSelection; };

   /** Get the initial X axis. */
   const QString& GetInitialXAxis() const override { return mInitialXAxis; };

   /** Get the initial Y axis */
   const QString& GetInitialYAxis() const override { return mInitialYAxis; };

   /** Get a series(plot) of X vs. Y.
    * @param aItem platform name
    * @param aXAxis name of the x axis to plot
    * @param aYAxis name of the y axis to plot
    */
   std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                    const QString& aXAxis,
                                                    const QString& aYAxis) const override;

private:
   /** Helper function to get the available axes. */
   QStringList GetAxes() const;

   /** Helper function to get the units for an axis. */
   QString GetUnits(const QString& aAxis) const;

   /** Get Orbital Element Data, based on variable name
    * @param aDesc description/label for variable
    * @param aState Msg to read data from
    */
   double GetOrbitalData(const QString& aDesc, const rv::MsgEntityState* aState) const;

   QStringList mInitialSelection;
   QString     mInitialXAxis;
   QString     mInitialYAxis;
};
} // namespace RvOrbitalData

#endif
