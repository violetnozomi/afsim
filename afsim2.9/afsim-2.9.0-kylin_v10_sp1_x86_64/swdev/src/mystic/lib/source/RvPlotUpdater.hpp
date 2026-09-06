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
#ifndef RVPLOTUPDATER_HPP
#define RVPLOTUPDATER_HPP

#include <vector>

#include <QString>
#include <QStringList>

#include "RvExport.hpp"

namespace rv
{
class RV_EXPORT PlotUpdater
{
public:
   virtual ~PlotUpdater() = default;

   virtual QStringList GetPlatforms() const = 0;
   virtual QStringList GetAxesX() const     = 0;
   virtual QStringList GetAxesY() const     = 0;

   virtual QString GetUnitsX(const QString& aAxis) const { return {}; }
   virtual QString GetUnitsY(const QString& aAxis) const { return {}; }

   virtual const QStringList& GetInitialSelection() const = 0;
   virtual const QString&     GetInitialXAxis() const     = 0;
   virtual const QString&     GetInitialYAxis() const     = 0;

   virtual std::vector<std::pair<double, double>> GetSeries(const QString& aItem,
                                                            const QString& aXAxis,
                                                            const QString& aYAxis) const = 0;

   virtual bool SupportsMultiplatform() const { return true; }

   double GetSampleRate() const { return mSampleRate; }
   void   SetSampleRate(double aRate) { mSampleRate = aRate; }
   double GetLastUpdate() const { return mLastUpdate; }
   void   SetLastUpdate(double aTime) { mLastUpdate = aTime; }

protected:
   mutable double mLastUpdate{0.0};

private:
   double mSampleRate{0.0};
};
} // namespace rv

#endif
